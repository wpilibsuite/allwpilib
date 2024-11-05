// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sysid/analysis/FilteringUtils.h"

#include <algorithm>
#include <functional>
#include <limits>
#include <numbers>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

#include <fmt/format.h>
#include <frc/filter/LinearFilter.h>
#include <frc/filter/MedianFilter.h>
#include <units/math.h>
#include <wpi/MathExtras.h>
#include <wpi/StringExtras.h>

using namespace sysid;

/**
 * Helper function that throws if it detects that the data vector is too small
 * for an operation of a certain window size.
 *
 * @param data The data that is being used.
 * @param window The window size for the operation.
 * @param operation The operation we're checking the size for (for error
 *                  throwing purposes).
 */
static void CheckSize(const std::vector<PreparedData>& data, size_t window,
                      std::string_view operation) {
  if (data.size() < window) {
    throw sysid::InvalidDataError(
        fmt::format("Not enough data to run {} which has a window size of {}.",
                    operation, window));
  }
}

/**
 * Helper function that determines if a certain key is storing raw data.
 *
 * @param key The key of the dataset
 *
 * @return True, if the key corresponds to a raw dataset.
 */
static bool IsRaw(std::string_view key) {
  return wpi::contains(key, "raw") && !wpi::contains(key, "original");
}

/**
 * Helper function that determines if a certain key is storing filtered data.
 *
 * @param key The key of the dataset
 *
 * @return True, if the key corresponds to a filtered dataset.
 */
static bool IsFiltered(std::string_view key) {
  return !wpi::contains(key, "raw") && !wpi::contains(key, "original");
}

/**
 * Fills in the rest of the PreparedData Structs for a PreparedData Vector.
 *
 * @param data A reference to a vector of the raw data.
 * @param unit The units that the data is in (rotations, radians, or degrees)
 *             for arm mechanisms.
 */
static void PrepareMechData(std::vector<PreparedData>* data,
                            std::string_view unit = "") {
  constexpr size_t kWindow = 3;

  CheckSize(*data, kWindow, "Acceleration Calculation");

  // Calculates the cosine of the position data for single jointed arm analysis
  for (size_t i = 0; i < data->size(); ++i) {
    auto& pt = data->at(i);

    double cos = 0.0;
    double sin = 0.0;
    if (unit == "Radians") {
      cos = std::cos(pt.position);
      sin = std::sin(pt.position);
    } else if (unit == "Degrees") {
      cos = std::cos(pt.position * std::numbers::pi / 180.0);
      sin = std::sin(pt.position * std::numbers::pi / 180.0);
    } else if (unit == "Rotations") {
      cos = std::cos(pt.position * 2 * std::numbers::pi);
      sin = std::sin(pt.position * 2 * std::numbers::pi);
    }
    pt.cos = cos;
    pt.sin = sin;
  }

  auto derivative =
      CentralFiniteDifference<1, kWindow>(GetMeanTimeDelta(*data));

  // Load the derivative filter with the first value for accurate initial
  // behavior
  for (size_t i = 0; i < kWindow; ++i) {
    derivative.Calculate(data->at(0).velocity);
  }

  for (size_t i = (kWindow - 1) / 2; i < data->size(); ++i) {
    data->at(i - (kWindow - 1) / 2).acceleration =
        derivative.Calculate(data->at(i).velocity);
  }

  // Fill in accelerations past end of derivative filter
  for (size_t i = data->size() - (kWindow - 1) / 2; i < data->size(); ++i) {
    data->at(i).acceleration = 0.0;
  }
}

std::tuple<units::second_t, units::second_t, units::second_t>
sysid::TrimStepVoltageData(std::vector<PreparedData>* data,
                           AnalysisManager::Settings* settings,
                           units::second_t minStepTime,
                           units::second_t maxStepTime) {
  auto voltageBegins =
      std::find_if(data->begin(), data->end(),
                   [](auto& datum) { return std::abs(datum.voltage) > 0; });

  units::second_t firstTimestamp = voltageBegins->timestamp;
  double firstPosition = voltageBegins->position;

  auto motionBegins = std::find_if(
      data->begin(), data->end(), [settings, firstPosition](auto& datum) {
        return std::abs(datum.position - firstPosition) >
               (settings->velocityThreshold * datum.dt.value());
      });

  units::second_t positionDelay;
  if (motionBegins != data->end()) {
    positionDelay = motionBegins->timestamp - firstTimestamp;
  } else {
    positionDelay = 0_s;
  }

  auto maxAccel = std::max_element(
      data->begin(), data->end(), [](const auto& a, const auto& b) {
        // Since we don't know if its a forward or backwards test here, we use
        // the sign of each point's velocity to determine how to compare their
        // accelerations.
        return wpi::sgn(a.velocity) * a.acceleration <
               wpi::sgn(b.velocity) * b.acceleration;
      });

  // Current limiting can delay onset of the peak acceleration, so we need to
  // find the first acceleration *near* the max.  Magic number tolerance here
  // because this whole file is tech debt already
  auto accelBegins = std::find_if(
      data->begin(), data->end(), [&maxAccel](const auto& measurement) {
        return wpi::sgn(measurement.velocity) * measurement.acceleration >
               0.8 * wpi::sgn(maxAccel->velocity) * maxAccel->acceleration;
      });

  units::second_t velocityDelay;
  if (accelBegins != data->end()) {
    velocityDelay = accelBegins->timestamp - firstTimestamp;

    // Trim data before max acceleration
    data->erase(data->begin(), maxAccel);
  } else {
    velocityDelay = 0_s;
  }

  minStepTime = std::min(data->at(0).timestamp - firstTimestamp, minStepTime);

  // If step test duration not yet specified, calculate default
  if (settings->stepTestDuration == 0_s) {
    // Find maximum speed reached
    const auto maxSpeed =
        GetMaxSpeed(*data, [](auto&& pt) { return pt.velocity; });
    // Find place where 90% of maximum speed exceeded
    auto endIt = std::find_if(
        data->begin(), data->end(), [&](const PreparedData& entry) {
          return std::abs(entry.velocity) > maxSpeed * 0.9;
        });

    if (endIt != data->end()) {
      settings->stepTestDuration =
          std::min(endIt->timestamp - data->front().timestamp + minStepTime,
                   maxStepTime);
    }
  }

  // Find first entry greater than the step test duration
  auto maxIt =
      std::find_if(data->begin(), data->end(), [&](PreparedData entry) {
        return entry.timestamp - data->front().timestamp >
               settings->stepTestDuration;
      });

  // Trim data beyond desired step test duration
  if (maxIt != data->end()) {
    data->erase(maxIt, data->end());
  }

  return std::make_tuple(minStepTime, positionDelay, velocityDelay);
}

double sysid::GetNoiseFloor(
    const std::vector<PreparedData>& data, int window,
    std::function<double(const PreparedData&)> accessorFunction) {
  double sum = 0.0;
  size_t step = window / 2;
  auto averageFilter = frc::LinearFilter<double>::MovingAverage(window);
  for (size_t i = 0; i < data.size(); i++) {
    double mean = averageFilter.Calculate(accessorFunction(data[i]));
    if (i >= step) {
      sum += std::pow(accessorFunction(data[i - step]) - mean, 2);
    }
  }
  return std::sqrt(sum / (data.size() - step));
}

double sysid::GetMaxSpeed(
    const std::vector<PreparedData>& data,
    std::function<double(const PreparedData&)> accessorFunction) {
  double max = 0.0;
  for (size_t i = 0; i < data.size(); i++) {
    max = std::max(max, std::abs(accessorFunction(data[i])));
  }
  return max;
}

units::second_t sysid::GetMeanTimeDelta(const std::vector<PreparedData>& data) {
  std::vector<units::second_t> dts;

  for (const auto& pt : data) {
    if (pt.dt > 0_s && pt.dt < 500_ms) {
      dts.emplace_back(pt.dt);
    }
  }

  return std::accumulate(dts.begin(), dts.end(), 0_s) / dts.size();
}

units::second_t sysid::GetMeanTimeDelta(const Storage& data) {
  std::vector<units::second_t> dts;

  for (const auto& pt : data.slowForward) {
    if (pt.dt > 0_s && pt.dt < 500_ms) {
      dts.emplace_back(pt.dt);
    }
  }

  for (const auto& pt : data.slowBackward) {
    if (pt.dt > 0_s && pt.dt < 500_ms) {
      dts.emplace_back(pt.dt);
    }
  }

  for (const auto& pt : data.fastForward) {
    if (pt.dt > 0_s && pt.dt < 500_ms) {
      dts.emplace_back(pt.dt);
    }
  }

  for (const auto& pt : data.fastBackward) {
    if (pt.dt > 0_s && pt.dt < 500_ms) {
      dts.emplace_back(pt.dt);
    }
  }

  return std::accumulate(dts.begin(), dts.end(), 0_s) / dts.size();
}

void sysid::ApplyMedianFilter(std::vector<PreparedData>* data, int window) {
  CheckSize(*data, window, "Median Filter");

  frc::MedianFilter<double> medianFilter(window);

  // Load the median filter with the first value for accurate initial behavior
  for (int i = 0; i < window; i++) {
    medianFilter.Calculate(data->at(0).velocity);
  }

  for (size_t i = (window - 1) / 2; i < data->size(); i++) {
    data->at(i - (window - 1) / 2).velocity =
        medianFilter.Calculate(data->at(i).velocity);
  }

  // Run the median filter for the last half window of datapoints by loading the
  // median filter with the last recorded velocity value
  for (size_t i = data->size() - (window - 1) / 2; i < data->size(); i++) {
    data->at(i).velocity =
        medianFilter.Calculate(data->at(data->size() - 1).velocity);
  }
}

/**
 * Removes a substring from a string reference
 *
 * @param str The std::string_view that needs modification
 * @param removeStr The substring that needs to be removed
 *
 * @return an std::string without the specified substring
 */
static std::string RemoveStr(std::string_view str, std::string_view removeStr) {
  size_t idx = str.find(removeStr);
  if (idx == std::string_view::npos) {
    return std::string{str};
  } else {
    return fmt::format("{}{}", str.substr(0, idx),
                       str.substr(idx + removeStr.size()));
  }
}

/**
 * Figures out the max duration of the Dynamic tests
 *
 * @param data The raw data String Map
 *
 * @return The maximum duration of the Dynamic Tests
 */
static units::second_t GetMaxStepTime(
    wpi::StringMap<std::vector<PreparedData>>& data) {
  auto maxStepTime = 0_s;
  for (auto& it : data) {
    auto& key = it.first;
    auto& dataset = it.second;

    if (IsRaw(key) && wpi::contains(key, "dynamic")) {
      if (!dataset.empty()) {
        auto duration = dataset.back().timestamp - dataset.front().timestamp;
        if (duration > maxStepTime) {
          maxStepTime = duration;
        }
      }
    }
  }
  return maxStepTime;
}

void sysid::InitialTrimAndFilter(
    wpi::StringMap<std::vector<PreparedData>>* data,
    AnalysisManager::Settings* settings,
    std::vector<units::second_t>& positionDelays,
    std::vector<units::second_t>& velocityDelays, units::second_t& minStepTime,
    units::second_t& maxStepTime, std::string_view unit) {
  auto& preparedData = *data;

  // Find the maximum Step Test Duration of the dynamic tests
  maxStepTime = GetMaxStepTime(preparedData);

  // Calculate Velocity Threshold if it hasn't been set yet
  if (settings->velocityThreshold == std::numeric_limits<double>::infinity()) {
    for (auto& it : preparedData) {
      auto& key = it.first;
      auto& dataset = it.second;
      if (wpi::contains(key, "quasistatic")) {
        settings->velocityThreshold =
            std::min(settings->velocityThreshold,
                     GetNoiseFloor(dataset, kNoiseMeanWindow,
                                   [](auto&& pt) { return pt.velocity; }));
      }
    }
  }

  for (auto& it : preparedData) {
    auto& key = it.first;
    auto& dataset = it.second;

    // Trim quasistatic test data to remove all points where voltage is zero or
    // velocity < velocity threshold.
    if (wpi::contains(key, "quasistatic")) {
      dataset.erase(std::remove_if(dataset.begin(), dataset.end(),
                                   [&](const auto& pt) {
                                     return std::abs(pt.voltage) <= 0 ||
                                            std::abs(pt.velocity) <
                                                settings->velocityThreshold;
                                   }),
                    dataset.end());

      // Confirm there's still data
      if (dataset.empty()) {
        throw sysid::NoQuasistaticDataError();
      }
    }

    // Apply Median filter
    if (IsFiltered(key) && settings->medianWindow > 1) {
      ApplyMedianFilter(&dataset, settings->medianWindow);
    }

    // Recalculate Accel and Cosine
    PrepareMechData(&dataset, unit);

    // Trims filtered Dynamic Test Data
    if (IsFiltered(key) && wpi::contains(key, "dynamic")) {
      // Get the filtered dataset name
      auto filteredKey = RemoveStr(key, "raw-");

      // Trim Filtered Data
      auto [tempMinStepTime, positionDelay, velocityDelay] =
          TrimStepVoltageData(&preparedData[filteredKey], settings, minStepTime,
                              maxStepTime);

      positionDelays.emplace_back(positionDelay);
      velocityDelays.emplace_back(velocityDelay);

      // Set the Raw Data to start at the same time as the Filtered Data
      auto startTime = preparedData[filteredKey].front().timestamp;
      auto rawStart =
          std::find_if(preparedData[key].begin(), preparedData[key].end(),
                       [&](auto&& pt) { return pt.timestamp == startTime; });
      preparedData[key].erase(preparedData[key].begin(), rawStart);

      // Confirm there's still data
      if (preparedData[key].empty()) {
        throw sysid::NoDynamicDataError();
      }
    }
  }
}

void sysid::AccelFilter(wpi::StringMap<std::vector<PreparedData>>* data) {
  auto& preparedData = *data;

  // Remove points with acceleration = 0
  for (auto& it : preparedData) {
    auto& dataset = it.second;

    for (size_t i = 0; i < dataset.size(); i++) {
      if (dataset.at(i).acceleration == 0.0) {
        dataset.erase(dataset.begin() + i);
        i--;
      }
    }
  }

  // Confirm there's still data
  if (std::any_of(preparedData.begin(), preparedData.end(),
                  [](const auto& it) { return it.second.empty(); })) {
    throw sysid::InvalidDataError(
        "Acceleration filtering has removed all data.");
  }
}
