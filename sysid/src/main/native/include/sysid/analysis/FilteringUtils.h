// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <exception>
#include <functional>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <frc/filter/LinearFilter.h>
#include <units/time.h>
#include <wpi/StringMap.h>
#include <wpi/array.h>

#include "sysid/analysis/AnalysisManager.h"
#include "sysid/analysis/Storage.h"

namespace sysid {

constexpr int kNoiseMeanWindow = 9;

/**
 * Exception for Invalid Data Errors in which we can't pin the cause of error to
 * any one specific setting of the GUI.
 */
class InvalidDataError : public std::exception {
 public:
  /**
   * Creates an InvalidDataError Exception. It adds additional steps after the
   * initial error message to inform users in the ways that they could fix their
   * data.
   *
   * @param message The error message
   */
  explicit InvalidDataError(std::string_view message) {
    m_message = fmt::format(
        "{}. Please verify that your units and data is reasonable and then "
        "adjust your velocity threshold, test duration, and/or window size to "
        "try to fix this issue.",
        message);
  }

  const char* what() const noexcept override { return m_message.c_str(); }

 private:
  /**
   * Stores the error message
   */
  std::string m_message;
};

/**
 * Exception for Quasistatic Data being completely removed.
 */
class NoQuasistaticDataError : public std::exception {
 public:
  const char* what() const noexcept override {
    return "Quasistatic test trimming removed all data. Please adjust your "
           "velocity threshold and double check "
           "your units and test data to make sure that the robot is reporting "
           "reasonable values.";
  }
};

/**
 * Exception for not all tests being present.
 */
class MissingTestsError : public std::exception {
 public:
  explicit MissingTestsError(std::vector<std::string> MissingTests)
      : missingTests(std::move(MissingTests)) {
    errorString = fmt::format(
        "The following tests were not detected: {}. Make sure to perform all "
        "four tests as described in the SysId documentation.",
        fmt::join(missingTests, ", "));
  }
  const char* what() const noexcept override { return errorString.c_str(); }

 private:
  std::vector<std::string> missingTests;
  std::string errorString;
};

/**
 * Exception for Dynamic Data being completely removed.
 */
class NoDynamicDataError : public std::exception {
 public:
  const char* what() const noexcept override {
    return "Dynamic test trimming removed all data. Please adjust your test "
           "duration and double check "
           "your units and test data to make sure that the robot is reporting "
           "reasonable values.";
  }
};

/**
 * Calculates the expected acceleration noise to be used as the floor of the
 * Voltage Trim. This is done by taking the standard deviation from the moving
 * average values of each point.
 *
 * @param data the prepared data vector containing acceleration data
 * @param window the size of the window for the moving average
 * @param accessorFunction a function that accesses the desired data from the
 *                         PreparedData struct.
 * @return The expected acceleration noise
 */
double GetNoiseFloor(
    const std::vector<PreparedData>& data, int window,
    std::function<double(const PreparedData&)> accessorFunction);

double GetMaxSpeed(const std::vector<PreparedData>& data,
                   std::function<double(const PreparedData&)> accessorFunction);

/**
 * Reduces noise in velocity data by applying a median filter.
 *
 * @tparam S The size of the raw data array
 * @tparam Velocity The index of the velocity entry in the raw data.
 * @param data the vector of arrays representing sysid data (must contain
 *             velocity data)
 * @param window the size of the window of the median filter (must be odd)
 */
void ApplyMedianFilter(std::vector<PreparedData>* data, int window);

/**
 * Trims the step voltage data to discard all points before the maximum
 * acceleration and after reaching stead-state velocity. Also trims the end of
 * the test based off of user specified test durations, but it will determine a
 * default duration if the requested duration is less than the minimum step test
 * duration.
 *
 * @param data A pointer to the step voltage data.
 * @param settings A pointer to the settings of an analysis manager object.
 * @param minStepTime The current minimum step test duration as one of the
 *                    trimming procedures will remove this amount from the start
 *                    of the test.
 * @param maxStepTime The maximum step test duration.
 * @return The updated minimum step test duration.
 */
std::tuple<units::second_t, units::second_t, units::second_t>
TrimStepVoltageData(std::vector<PreparedData>* data,
                    AnalysisManager::Settings* settings,
                    units::second_t minStepTime, units::second_t maxStepTime);

/**
 * Compute the mean time delta of the given data.
 *
 * @param data A reference to all of the collected PreparedData
 * @return The mean time delta for all the data points
 */
units::second_t GetMeanTimeDelta(const std::vector<PreparedData>& data);

/**
 * Compute the mean time delta of the given data.
 *
 * @param data A reference to all of the collected PreparedData
 * @return The mean time delta for all the data points
 */
units::second_t GetMeanTimeDelta(const Storage& data);

/**
 * Creates a central finite difference filter that computes the nth
 * derivative of the input given the specified number of samples.
 *
 * Since this requires data from the future, it should only be used in offline
 * filtering scenarios.
 *
 * For example, a first derivative filter that uses two samples and a sample
 * period of 20 ms would be
 *
 * <pre><code>
 * CentralFiniteDifference<1, 2>(20_ms);
 * </code></pre>
 *
 * @tparam Derivative The order of the derivative to compute.
 * @tparam Samples    The number of samples to use to compute the given
 *                    derivative. This must be odd and one more than the order
 *                    of derivative or higher.
 * @param period      The period in seconds between samples taken by the user.
 */
template <int Derivative, int Samples>
frc::LinearFilter<double> CentralFiniteDifference(units::second_t period) {
  static_assert(Samples % 2 != 0, "Number of samples must be odd.");

  // Generate stencil points from -(samples - 1)/2 to (samples - 1)/2
  wpi::array<int, Samples> stencil{wpi::empty_array};
  for (int i = 0; i < Samples; ++i) {
    stencil[i] = -(Samples - 1) / 2 + i;
  }

  return frc::LinearFilter<double>::FiniteDifference<Derivative, Samples>(
      stencil, period);
}

/**
 * Trims the quasistatic tests, applies a median filter to the velocity data,
 * calculates acceleration and cosine (arm only) data, and trims the dynamic
 * tests.
 *
 * @param data A pointer to a data vector recently created by the
 *             ConvertToPrepared method
 * @param settings A reference to the analysis settings
 * @param positionDelays A reference to the vector of computed position signal
 * delays.
 * @param velocityDelays A reference to the vector of computed velocity signal
 * delays.
 * @param minStepTime A reference to the minimum dynamic test duration as one of
 *                    the trimming procedures will remove this amount from the
 *                    start of the test.
 * @param maxStepTime A reference to the maximum dynamic test duration
 * @param unit The angular unit that the arm test is in (only for calculating
 *             cosine data)
 */
void InitialTrimAndFilter(wpi::StringMap<std::vector<PreparedData>>* data,
                          AnalysisManager::Settings* settings,
                          std::vector<units::second_t>& positionDelays,
                          std::vector<units::second_t>& velocityDelays,
                          units::second_t& minStepTime,
                          units::second_t& maxStepTime,
                          std::string_view unit = "");

/**
 * Removes all points with acceleration = 0.
 *
 * @param data A pointer to a PreparedData vector
 */
void AccelFilter(wpi::StringMap<std::vector<PreparedData>>* data);

}  // namespace sysid
