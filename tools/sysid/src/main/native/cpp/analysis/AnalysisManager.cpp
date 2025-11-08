// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sysid/analysis/AnalysisManager.h"

#include <cmath>
#include <limits>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <units/angle.h>
#include <wpi/MathExtras.h>
#include <wpi/StringExtras.h>
#include <wpi/StringMap.h>

#include "sysid/analysis/FeedforwardAnalysis.h"
#include "sysid/analysis/FilteringUtils.h"

using namespace sysid;

static double Lerp(units::second_t time,
                   std::vector<MotorData::Run::Sample<double>>& data) {
  auto next = std::find_if(data.begin(), data.end(), [&](const auto& entry) {
    return entry.time > time;
  });

  if (next == data.begin()) {
    next++;
  }

  if (next == data.end()) {
    next--;
  }

  const auto prev = next - 1;

  return wpi::Lerp(prev->measurement, next->measurement,
                   (time - prev->time) / (next->time - prev->time));
}

/**
 * Converts a raw data vector into a PreparedData vector with only the
 * timestamp, voltage, position, and velocity fields filled out.
 *
 * @tparam S The size of the arrays in the raw data vector
 * @tparam Timestamp The index of the Timestamp data in the raw data vector
 * arrays
 * @tparam Voltage The index of the Voltage data in the raw data vector arrays
 * @tparam Position The index of the Position data in the raw data vector arrays
 * @tparam Velocity The index of the Velocity data in the raw data vector arrays
 *
 * @param data A raw data vector
 *
 * @return A PreparedData vector
 */
static std::vector<PreparedData> ConvertToPrepared(const MotorData& data) {
  std::vector<PreparedData> prepared;
  // assume we've selected down to a single contiguous run by this point
  auto run = data.runs[0];

  for (int i = 0; i < static_cast<int>(run.voltage.size()) - 1; ++i) {
    const auto& currentVoltage = run.voltage[i];
    const auto& nextVoltage = run.voltage[i + 1];

    auto currentPosition = Lerp(currentVoltage.time, run.position);

    auto currentVelocity = Lerp(currentVoltage.time, run.velocity);

    prepared.emplace_back(PreparedData{currentVoltage.time,
                                       currentVoltage.measurement.value(),
                                       currentPosition, currentVelocity,
                                       nextVoltage.time - currentVoltage.time});
  }

  return prepared;
}

/**
 * To preserve a raw copy of the data, this method saves a raw version
 * in the dataset StringMap where the key of the raw data starts with "raw-"
 * before the name of the original data.
 *
 * @tparam S The size of the array data that's being used
 *
 * @param dataset A reference to the dataset being used
 */
static void CopyRawData(wpi::StringMap<MotorData>* dataset) {
  auto& data = *dataset;
  // Loads the Raw Data
  for (auto& it : data) {
    auto& key = it.first;
    auto& motorData = it.second;

    if (!wpi::contains(key, "raw")) {
      data[fmt::format("raw-{}", key)] = motorData;
      data[fmt::format("original-raw-{}", key)] = motorData;
    }
  }
}

/**
 * Assigns the combines the various datasets into a single one for analysis.
 *
 * @param slowForward The slow forward dataset
 * @param slowBackward The slow backward dataset
 * @param fastForward The fast forward dataset
 * @param fastBackward The fast backward dataset
 */
static Storage CombineDatasets(const std::vector<PreparedData>& slowForward,
                               const std::vector<PreparedData>& slowBackward,
                               const std::vector<PreparedData>& fastForward,
                               const std::vector<PreparedData>& fastBackward) {
  return Storage{slowForward, slowBackward, fastForward, fastBackward};
}

void AnalysisManager::PrepareGeneralData() {
  wpi::StringMap<std::vector<PreparedData>> preparedData;

  WPI_INFO(m_logger, "{}", "Preprocessing raw data.");

  WPI_INFO(m_logger, "{}", "Copying raw data.");
  CopyRawData(&m_data.motorData);

  WPI_INFO(m_logger, "{}", "Converting raw data to PreparedData struct.");
  // Convert data to PreparedData structs
  for (auto& it : m_data.motorData) {
    auto key = it.first;
    preparedData[key] = ConvertToPrepared(m_data.motorData[key]);
    WPI_INFO(m_logger, "SAMPLES {}", preparedData[key].size());
  }

  // Store the original datasets
  m_originalDataset =
      CombineDatasets(preparedData["original-raw-quasistatic-forward"],
                      preparedData["original-raw-quasistatic-reverse"],
                      preparedData["original-raw-dynamic-forward"],
                      preparedData["original-raw-dynamic-reverse"]);

  WPI_INFO(m_logger, "{}", "Initial trimming and filtering.");
  sysid::InitialTrimAndFilter(&preparedData, &m_settings, m_positionDelays,
                              m_velocityDelays, m_minStepTime, m_maxStepTime,
                              m_data.distanceUnit);

  WPI_INFO(m_logger, "{}", m_minStepTime);
  WPI_INFO(m_logger, "{}", m_maxStepTime);

  WPI_INFO(m_logger, "{}", "Acceleration filtering.");
  sysid::AccelFilter(&preparedData);

  WPI_INFO(m_logger, "{}", "Storing datasets.");
  // Store the raw datasets
  m_rawDataset = CombineDatasets(preparedData["raw-quasistatic-forward"],
                                 preparedData["raw-quasistatic-reverse"],
                                 preparedData["raw-dynamic-forward"],
                                 preparedData["raw-dynamic-reverse"]);

  // Store the filtered datasets
  m_filteredDataset = CombineDatasets(
      preparedData["quasistatic-forward"], preparedData["quasistatic-reverse"],
      preparedData["dynamic-forward"], preparedData["dynamic-reverse"]);

  m_startTimes = {preparedData["raw-quasistatic-forward"][0].timestamp,
                  preparedData["raw-quasistatic-reverse"][0].timestamp,
                  preparedData["raw-dynamic-forward"][0].timestamp,
                  preparedData["raw-dynamic-reverse"][0].timestamp};
}

AnalysisManager::AnalysisManager(Settings& settings, wpi::Logger& logger)
    : m_logger{logger}, m_settings{settings} {}

AnalysisManager::AnalysisManager(TestData data, Settings& settings,
                                 wpi::Logger& logger)
    : m_data{std::move(data)}, m_logger{logger}, m_settings{settings} {
  // Reset settings for Dynamic Test Limits
  m_settings.stepTestDuration = 0_s;
  m_settings.velocityThreshold = std::numeric_limits<double>::infinity();
}

void AnalysisManager::PrepareData() {
  //  WPI_INFO(m_logger, "Preparing {} data", m_data.mechanismType.name);

  PrepareGeneralData();

  WPI_INFO(m_logger, "{}", "Finished Preparing Data");
}

AnalysisManager::FeedforwardGains AnalysisManager::CalculateFeedforward() {
  if (m_filteredDataset.empty()) {
    throw sysid::InvalidDataError(
        "There is no data to perform gain calculation on.");
  }

  WPI_INFO(m_logger, "{}", "Calculating Gains");
  // Calculate feedforward gains from the data.
  const auto& analysisType = m_data.mechanismType;
  const auto& ff =
      sysid::CalculateFeedforwardGains(GetFilteredData(), analysisType, false);

  const auto& Ks = ff.coeffs[0];
  FeedforwardGain KsGain = {
      .gain = Ks, .descriptor = "Voltage needed to overcome static friction."};
  if (Ks < 0) {
    KsGain.isValidGain = false;
    KsGain.errorMessage = fmt::format(
        "Calculated Ks gain of: {0:.3f} is erroneous! Ks should be >= 0.", Ks);
  }

  const auto& Kv = ff.coeffs[1];
  FeedforwardGain KvGain = {
      .gain = Kv,
      .descriptor =
          "Voltage needed to hold/cruise at a constant velocity while "
          "overcoming the counter-electromotive force and any additional "
          "friction."};
  if (Kv < 0) {
    KvGain.isValidGain = false;
    KvGain.errorMessage = fmt::format(
        "Calculated Kv gain of: {0:.3f} is erroneous! Kv should be >= 0.", Kv);
  }

  const auto& Ka = ff.coeffs[2];
  FeedforwardGain KaGain = {
      .gain = Ka,
      .descriptor =
          "Voltage needed to induce a given acceleration in the motor shaft."};
  if (Ka <= 0) {
    KaGain.isValidGain = false;
    KaGain.errorMessage = fmt::format(
        "Calculated Ka gain of: {0:.3f} is erroneous! Ka should be > 0.", Ka);
  }

  if (analysisType == analysis::kSimple) {
    return FeedforwardGains{
        .olsResult = ff, .Ks = KsGain, .Kv = KvGain, .Ka = KaGain};
  }

  if (analysisType == analysis::kElevator || analysisType == analysis::kArm) {
    const auto& Kg = ff.coeffs[3];
    FeedforwardGain KgGain = {
        Kg, "Voltage needed to counteract the force of gravity."};
    if (Kg < 0) {
      KgGain.isValidGain = false;
      KgGain.errorMessage = fmt::format(
          "Calculated Kg gain of: {0:.3f} is erroneous! Kg should be >= 0.",
          Ka);
    }

    // Elevator analysis only requires Kg
    if (analysisType == analysis::kElevator) {
      return FeedforwardGains{.olsResult = ff,
                              .Ks = KsGain,
                              .Kv = KvGain,
                              .Ka = KaGain,
                              .Kg = KgGain};
    } else {
      // Arm analysis requires Kg and an angle offset
      FeedforwardGain offset = {
          .gain = ff.coeffs[4],
          .descriptor =
              "This is the angle offset which, when added to the angle "
              "measurement, zeroes it out when the arm is horizontal. This is "
              "needed for the arm feedforward to work."};
      return FeedforwardGains{ff, KsGain, KvGain, KaGain, KgGain, offset};
    }
  }

  return FeedforwardGains{.olsResult = ff};
}

sysid::FeedbackGains AnalysisManager::CalculateFeedback(
    const FeedforwardGain& Kv, const FeedforwardGain& Ka) {
  FeedbackGains fb;
  if (m_settings.type == FeedbackControllerLoopType::kPosition) {
    fb = sysid::CalculatePositionFeedbackGains(
        m_settings.preset, m_settings.lqr, Kv.gain, Ka.gain);
  } else {
    fb = sysid::CalculateVelocityFeedbackGains(
        m_settings.preset, m_settings.lqr, Kv.gain, Ka.gain);
  }

  return fb;
}

void AnalysisManager::OverrideUnits(std::string_view unit) {
  m_data.distanceUnit = unit;
}

void AnalysisManager::ResetUnitsFromJSON() {}
