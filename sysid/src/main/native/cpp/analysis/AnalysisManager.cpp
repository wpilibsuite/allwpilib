// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sysid/analysis/AnalysisManager.h"

#include <cmath>
#include <cstddef>
#include <functional>
#include <stdexcept>

#include <fmt/format.h>
#include <units/angle.h>
#include <units/math.h>
#include <wpi/StringExtras.h>
#include <wpi/StringMap.h>
#include <wpi/raw_istream.h>

#include "sysid/Util.h"
#include "sysid/analysis/FilteringUtils.h"
#include "sysid/analysis/JSONConverter.h"
#include "sysid/analysis/TrackWidthAnalysis.h"

using namespace sysid;

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
template <size_t S, size_t Timestamp, size_t Voltage, size_t Position,
          size_t Velocity>
static std::vector<PreparedData> ConvertToPrepared(
    const std::vector<std::array<double, S>>& data) {
  std::vector<PreparedData> prepared;
  for (int i = 0; i < static_cast<int>(data.size()) - 1; ++i) {
    const auto& pt1 = data[i];
    const auto& pt2 = data[i + 1];
    prepared.emplace_back(PreparedData{
        units::second_t{pt1[Timestamp]}, pt1[Voltage], pt1[Position],
        pt1[Velocity], units::second_t{pt2[Timestamp] - pt1[Timestamp]}});
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
template <size_t S>
static void CopyRawData(
    wpi::StringMap<std::vector<std::array<double, S>>>* dataset) {
  auto& data = *dataset;
  // Loads the Raw Data
  for (auto& it : data) {
    auto key = it.first();
    auto& dataset = it.getValue();

    if (!wpi::contains(key, "raw")) {
      data[fmt::format("raw-{}", key)] = dataset;
      data[fmt::format("original-raw-{}", key)] = dataset;
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
  using Data = std::array<double, 4>;
  wpi::StringMap<std::vector<Data>> data;
  wpi::StringMap<std::vector<PreparedData>> preparedData;

  // Store the raw data columns.
  static constexpr size_t kTimeCol = 0;
  static constexpr size_t kVoltageCol = 1;
  static constexpr size_t kPosCol = 2;
  static constexpr size_t kVelCol = 3;

  WPI_INFO(m_logger, "{}", "Reading JSON data.");
  // Get the major components from the JSON and store them inside a StringMap.
  for (auto&& key : AnalysisManager::kJsonDataKeys) {
    data[key] = m_json.at(key).get<std::vector<Data>>();
  }

  WPI_INFO(m_logger, "{}", "Preprocessing raw data.");
  // Ensure that voltage and velocity have the same sign. Also multiply
  // positions and velocities by the factor.
  for (auto it = data.begin(); it != data.end(); ++it) {
    for (auto&& pt : it->second) {
      pt[kVoltageCol] = std::copysign(pt[kVoltageCol], pt[kVelCol]);
      pt[kPosCol] *= m_factor;
      pt[kVelCol] *= m_factor;
    }
  }

  WPI_INFO(m_logger, "{}", "Copying raw data.");
  CopyRawData(&data);

  WPI_INFO(m_logger, "{}", "Converting raw data to PreparedData struct.");
  // Convert data to PreparedData structs
  for (auto& it : data) {
    auto key = it.first();
    preparedData[key] =
        ConvertToPrepared<4, kTimeCol, kVoltageCol, kPosCol, kVelCol>(
            data[key]);
  }

  // Store the original datasets
  m_originalDataset[static_cast<int>(
      AnalysisManager::Settings::DrivetrainDataset::kCombined)] =
      CombineDatasets(preparedData["original-raw-slow-forward"],
                      preparedData["original-raw-slow-backward"],
                      preparedData["original-raw-fast-forward"],
                      preparedData["original-raw-fast-backward"]);

  WPI_INFO(m_logger, "{}", "Initial trimming and filtering.");
  sysid::InitialTrimAndFilter(&preparedData, &m_settings, m_positionDelays,
                              m_velocityDelays, m_minStepTime, m_maxStepTime,
                              m_unit);

  WPI_INFO(m_logger, "{}", "Acceleration filtering.");
  sysid::AccelFilter(&preparedData);

  WPI_INFO(m_logger, "{}", "Storing datasets.");
  // Store the raw datasets
  m_rawDataset[static_cast<int>(
      AnalysisManager::Settings::DrivetrainDataset::kCombined)] =
      CombineDatasets(
          preparedData["raw-slow-forward"], preparedData["raw-slow-backward"],
          preparedData["raw-fast-forward"], preparedData["raw-fast-backward"]);

  // Store the filtered datasets
  m_filteredDataset[static_cast<int>(
      AnalysisManager::Settings::DrivetrainDataset::kCombined)] =
      CombineDatasets(
          preparedData["slow-forward"], preparedData["slow-backward"],
          preparedData["fast-forward"], preparedData["fast-backward"]);

  m_startTimes = {preparedData["raw-slow-forward"][0].timestamp,
                  preparedData["raw-slow-backward"][0].timestamp,
                  preparedData["raw-fast-forward"][0].timestamp,
                  preparedData["raw-fast-backward"][0].timestamp};
}

void AnalysisManager::PrepareAngularDrivetrainData() {
  using Data = std::array<double, 9>;
  wpi::StringMap<std::vector<Data>> data;
  wpi::StringMap<std::vector<PreparedData>> preparedData;

  // Store the relevant raw data columns.
  static constexpr size_t kTimeCol = 0;
  static constexpr size_t kLVoltageCol = 1;
  static constexpr size_t kRVoltageCol = 2;
  static constexpr size_t kLPosCol = 3;
  static constexpr size_t kRPosCol = 4;
  static constexpr size_t kLVelCol = 5;
  static constexpr size_t kRVelCol = 6;
  static constexpr size_t kAngleCol = 7;
  static constexpr size_t kAngularRateCol = 8;

  WPI_INFO(m_logger, "{}", "Reading JSON data.");
  // Get the major components from the JSON and store them inside a StringMap.
  for (auto&& key : AnalysisManager::kJsonDataKeys) {
    data[key] = m_json.at(key).get<std::vector<Data>>();
  }

  WPI_INFO(m_logger, "{}", "Preprocessing raw data.");
  // Ensure that voltage and velocity have the same sign. Also multiply
  // positions and velocities by the factor.
  for (auto it = data.begin(); it != data.end(); ++it) {
    for (auto&& pt : it->second) {
      pt[kLPosCol] *= m_factor;
      pt[kRPosCol] *= m_factor;
      pt[kLVelCol] *= m_factor;
      pt[kRVelCol] *= m_factor;

      // Stores the average voltages in the left voltage column.
      // This aggregates the left and right voltages into a single voltage
      // column for the ConvertToPrepared() method. std::copysign() ensures the
      // polarity of the voltage matches the direction the robot turns.
      pt[kLVoltageCol] = std::copysign(
          (std::abs(pt[kLVoltageCol]) + std::abs(pt[kRVoltageCol])) / 2,
          pt[kAngularRateCol]);

      // ω = (v_r - v_l) / trackwidth
      // v = ωr => v = ω * trackwidth / 2
      // (v_r - v_l) / trackwidth * (trackwidth / 2) = (v_r - v_l) / 2
      // However, since we know this is an angular test, the left and right
      // wheel velocities will have opposite signs, allowing us to add their
      // absolute values and get the same result (in terms of magnitude).
      // std::copysign() is used to make sure the direction of the wheel
      // velocities matches the direction the robot turns.
      pt[kAngularRateCol] =
          std::copysign((std::abs(pt[kRVelCol]) + std::abs(pt[kLVelCol])) / 2,
                        pt[kAngularRateCol]);
    }
  }

  WPI_INFO(m_logger, "{}", "Calculating trackwidth");
  // Aggregating all the deltas from all the tests
  double leftDelta = 0.0;
  double rightDelta = 0.0;
  double angleDelta = 0.0;
  for (const auto& it : data) {
    auto key = it.first();
    auto& trackWidthData = data[key];
    leftDelta += std::abs(trackWidthData.back()[kLPosCol] -
                          trackWidthData.front()[kLPosCol]);
    rightDelta += std::abs(trackWidthData.back()[kRPosCol] -
                           trackWidthData.front()[kRPosCol]);
    angleDelta += std::abs(trackWidthData.back()[kAngleCol] -
                           trackWidthData.front()[kAngleCol]);
  }
  m_trackWidth = sysid::CalculateTrackWidth(leftDelta, rightDelta,
                                            units::radian_t{angleDelta});

  WPI_INFO(m_logger, "{}", "Copying raw data.");
  CopyRawData(&data);

  WPI_INFO(m_logger, "{}", "Converting to PreparedData struct.");
  // Convert raw data to prepared data
  for (const auto& it : data) {
    auto key = it.first();
    preparedData[key] = ConvertToPrepared<9, kTimeCol, kLVoltageCol, kAngleCol,
                                          kAngularRateCol>(data[key]);
  }

  // Create the distinct datasets and store them
  m_originalDataset[static_cast<int>(
      AnalysisManager::Settings::DrivetrainDataset::kCombined)] =
      CombineDatasets(preparedData["original-raw-slow-forward"],
                      preparedData["original-raw-slow-backward"],
                      preparedData["original-raw-fast-forward"],
                      preparedData["original-raw-fast-backward"]);

  WPI_INFO(m_logger, "{}", "Applying trimming and filtering.");
  sysid::InitialTrimAndFilter(&preparedData, &m_settings, m_positionDelays,
                              m_velocityDelays, m_minStepTime, m_maxStepTime);

  WPI_INFO(m_logger, "{}", "Acceleration filtering.");
  sysid::AccelFilter(&preparedData);

  WPI_INFO(m_logger, "{}", "Storing datasets.");
  // Create the distinct datasets and store them
  m_rawDataset[static_cast<int>(
      AnalysisManager::Settings::DrivetrainDataset::kCombined)] =
      CombineDatasets(
          preparedData["raw-slow-forward"], preparedData["raw-slow-backward"],
          preparedData["raw-fast-forward"], preparedData["raw-fast-backward"]);
  m_filteredDataset[static_cast<int>(
      AnalysisManager::Settings::DrivetrainDataset::kCombined)] =
      CombineDatasets(
          preparedData["slow-forward"], preparedData["slow-backward"],
          preparedData["fast-forward"], preparedData["fast-backward"]);

  m_startTimes = {preparedData["slow-forward"][0].timestamp,
                  preparedData["slow-backward"][0].timestamp,
                  preparedData["fast-forward"][0].timestamp,
                  preparedData["fast-backward"][0].timestamp};
}

void AnalysisManager::PrepareLinearDrivetrainData() {
  using Data = std::array<double, 9>;
  wpi::StringMap<std::vector<Data>> data;
  wpi::StringMap<std::vector<PreparedData>> preparedData;

  // Store the relevant raw data columns.
  static constexpr size_t kTimeCol = 0;
  static constexpr size_t kLVoltageCol = 1;
  static constexpr size_t kRVoltageCol = 2;
  static constexpr size_t kLPosCol = 3;
  static constexpr size_t kRPosCol = 4;
  static constexpr size_t kLVelCol = 5;
  static constexpr size_t kRVelCol = 6;

  // Get the major components from the JSON and store them inside a StringMap.
  WPI_INFO(m_logger, "{}", "Reading JSON data.");
  for (auto&& key : AnalysisManager::kJsonDataKeys) {
    data[key] = m_json.at(key).get<std::vector<Data>>();
  }

  // Ensure that voltage and velocity have the same sign. Also multiply
  // positions and velocities by the factor.
  WPI_INFO(m_logger, "{}", "Preprocessing raw data.");
  for (auto it = data.begin(); it != data.end(); ++it) {
    for (auto&& pt : it->second) {
      pt[kLVoltageCol] = std::copysign(pt[kLVoltageCol], pt[kLVelCol]);
      pt[kRVoltageCol] = std::copysign(pt[kRVoltageCol], pt[kRVelCol]);
      pt[kLPosCol] *= m_factor;
      pt[kRPosCol] *= m_factor;
      pt[kLVelCol] *= m_factor;
      pt[kRVelCol] *= m_factor;
    }
  }

  WPI_INFO(m_logger, "{}", "Copying raw data.");
  CopyRawData(&data);

  // Convert data to PreparedData
  WPI_INFO(m_logger, "{}", "Converting to PreparedData struct.");
  for (auto& it : data) {
    auto key = it.first();

    preparedData[fmt::format("left-{}", key)] =
        ConvertToPrepared<9, kTimeCol, kLVoltageCol, kLPosCol, kLVelCol>(
            data[key]);
    preparedData[fmt::format("right-{}", key)] =
        ConvertToPrepared<9, kTimeCol, kRVoltageCol, kRPosCol, kRVelCol>(
            data[key]);
  }

  // Create the distinct raw datasets and store them
  auto originalSlowForward = AnalysisManager::DataConcat(
      preparedData["left-original-raw-slow-forward"],
      preparedData["right-original-raw-slow-forward"]);
  auto originalSlowBackward = AnalysisManager::DataConcat(
      preparedData["left-original-raw-slow-backward"],
      preparedData["right-original-raw-slow-backward"]);
  auto originalFastForward = AnalysisManager::DataConcat(
      preparedData["left-original-raw-fast-forward"],
      preparedData["right-original-raw-fast-forward"]);
  auto originalFastBackward = AnalysisManager::DataConcat(
      preparedData["left-original-raw-fast-backward"],
      preparedData["right-original-raw-fast-backward"]);
  m_originalDataset[static_cast<int>(
      AnalysisManager::Settings::DrivetrainDataset::kCombined)] =
      CombineDatasets(originalSlowForward, originalSlowBackward,
                      originalFastForward, originalFastBackward);
  m_originalDataset[static_cast<int>(
      AnalysisManager::Settings::DrivetrainDataset::kLeft)] =
      CombineDatasets(preparedData["left-original-raw-slow-forward"],
                      preparedData["left-original-raw-slow-backward"],
                      preparedData["left-original-raw-fast-forward"],
                      preparedData["left-original-raw-fast-backward"]);
  m_originalDataset[static_cast<int>(
      AnalysisManager::Settings::DrivetrainDataset::kRight)] =
      CombineDatasets(preparedData["right-original-raw-slow-forward"],
                      preparedData["right-original-raw-slow-backward"],
                      preparedData["right-original-raw-fast-forward"],
                      preparedData["right-original-raw-fast-backward"]);

  WPI_INFO(m_logger, "{}", "Applying trimming and filtering.");
  sysid::InitialTrimAndFilter(&preparedData, &m_settings, m_positionDelays,
                              m_velocityDelays, m_minStepTime, m_maxStepTime);

  auto slowForward = AnalysisManager::DataConcat(
      preparedData["left-slow-forward"], preparedData["right-slow-forward"]);
  auto slowBackward = AnalysisManager::DataConcat(
      preparedData["left-slow-backward"], preparedData["right-slow-backward"]);
  auto fastForward = AnalysisManager::DataConcat(
      preparedData["left-fast-forward"], preparedData["right-fast-forward"]);
  auto fastBackward = AnalysisManager::DataConcat(
      preparedData["left-fast-backward"], preparedData["right-fast-backward"]);

  WPI_INFO(m_logger, "{}", "Acceleration filtering.");
  sysid::AccelFilter(&preparedData);

  WPI_INFO(m_logger, "{}", "Storing datasets.");

  // Create the distinct raw datasets and store them
  auto rawSlowForward =
      AnalysisManager::DataConcat(preparedData["left-raw-slow-forward"],
                                  preparedData["right-raw-slow-forward"]);
  auto rawSlowBackward =
      AnalysisManager::DataConcat(preparedData["left-raw-slow-backward"],
                                  preparedData["right-raw-slow-backward"]);
  auto rawFastForward =
      AnalysisManager::DataConcat(preparedData["left-raw-fast-forward"],
                                  preparedData["right-raw-fast-forward"]);
  auto rawFastBackward =
      AnalysisManager::DataConcat(preparedData["left-raw-fast-backward"],
                                  preparedData["right-raw-fast-backward"]);

  m_rawDataset[static_cast<int>(
      AnalysisManager::Settings::DrivetrainDataset::kCombined)] =
      CombineDatasets(rawSlowForward, rawSlowBackward, rawFastForward,
                      rawFastBackward);
  m_rawDataset[static_cast<int>(
      AnalysisManager::Settings::DrivetrainDataset::kLeft)] =
      CombineDatasets(preparedData["left-raw-slow-forward"],
                      preparedData["left-raw-slow-backward"],
                      preparedData["left-raw-fast-forward"],
                      preparedData["left-raw-fast-backward"]);
  m_rawDataset[static_cast<int>(
      AnalysisManager::Settings::DrivetrainDataset::kRight)] =
      CombineDatasets(preparedData["right-raw-slow-forward"],
                      preparedData["right-raw-slow-backward"],
                      preparedData["right-raw-fast-forward"],
                      preparedData["right-raw-fast-backward"]);

  // Create the distinct filtered datasets and store them
  m_filteredDataset[static_cast<int>(
      AnalysisManager::Settings::DrivetrainDataset::kCombined)] =
      CombineDatasets(slowForward, slowBackward, fastForward, fastBackward);
  m_filteredDataset[static_cast<int>(
      AnalysisManager::Settings::DrivetrainDataset::kLeft)] =
      CombineDatasets(preparedData["left-slow-forward"],
                      preparedData["left-slow-backward"],
                      preparedData["left-fast-forward"],
                      preparedData["left-fast-backward"]);
  m_filteredDataset[static_cast<int>(
      AnalysisManager::Settings::DrivetrainDataset::kRight)] =
      CombineDatasets(preparedData["right-slow-forward"],
                      preparedData["right-slow-backward"],
                      preparedData["right-fast-forward"],
                      preparedData["right-fast-backward"]);

  m_startTimes = {
      rawSlowForward.front().timestamp, rawSlowBackward.front().timestamp,
      rawFastForward.front().timestamp, rawFastBackward.front().timestamp};
}

AnalysisManager::AnalysisManager(Settings& settings, wpi::Logger& logger)
    : m_logger{logger},
      m_settings{settings},
      m_type{analysis::kSimple},
      m_unit{"Meters"},
      m_factor{1} {}

AnalysisManager::AnalysisManager(std::string_view path, Settings& settings,
                                 wpi::Logger& logger)
    : m_logger{logger}, m_settings{settings} {
  {
    // Read JSON from the specified path
    std::error_code ec;
    wpi::raw_fd_istream is{path, ec};

    if (ec) {
      throw FileReadingError(path);
    }

    is >> m_json;

    WPI_INFO(m_logger, "Read {}", path);
  }

  // Check that we have a sysid JSON
  if (m_json.find("sysid") == m_json.end()) {
    // If it's not a sysid JSON, try converting it from frc-char format
    std::string newPath = sysid::ConvertJSON(path, logger);

    // Read JSON from the specified path
    std::error_code ec;
    wpi::raw_fd_istream is{newPath, ec};

    if (ec) {
      throw FileReadingError(newPath);
    }

    is >> m_json;

    WPI_INFO(m_logger, "Read {}", newPath);
  }

  WPI_INFO(m_logger, "Parsing initial data of {}", path);
  // Get the analysis type from the JSON.
  m_type = sysid::analysis::FromName(m_json.at("test").get<std::string>());

  // Get the rotation -> output units factor from the JSON.
  m_unit = m_json.at("units").get<std::string>();
  m_factor = m_json.at("unitsPerRotation").get<double>();
  WPI_DEBUG(m_logger, "Parsing units per rotation as {} {} per rotation",
            m_factor, m_unit);

  // Reset settings for Dynamic Test Limits
  m_settings.stepTestDuration = units::second_t{0.0};
  m_settings.motionThreshold = std::numeric_limits<double>::infinity();
}

void AnalysisManager::PrepareData() {
  WPI_INFO(m_logger, "Preparing {} data", m_type.name);
  if (m_type == analysis::kDrivetrain) {
    PrepareLinearDrivetrainData();
  } else if (m_type == analysis::kDrivetrainAngular) {
    PrepareAngularDrivetrainData();
  } else {
    PrepareGeneralData();
  }
  WPI_INFO(m_logger, "{}", "Finished Preparing Data");
}

AnalysisManager::FeedforwardGains AnalysisManager::CalculateFeedforward() {
  if (m_filteredDataset.empty()) {
    throw sysid::InvalidDataError(
        "There is no data to perform gain calculation on.");
  }

  WPI_INFO(m_logger, "{}", "Calculating Gains");
  // Calculate feedforward gains from the data.
  const auto& ff = sysid::CalculateFeedforwardGains(GetFilteredData(), m_type);
  FeedforwardGains ffGains = {ff, m_trackWidth};

  const auto& Ks = std::get<0>(ff)[0];
  const auto& Kv = std::get<0>(ff)[1];
  const auto& Ka = std::get<0>(ff)[2];

  if (Ka <= 0 || Kv < 0) {
    throw InvalidDataError(
        fmt::format("The calculated feedforward gains of kS: {}, Kv: {}, Ka: "
                    "{} are erroneous. Your Ka should be > 0 while your Kv and "
                    "Ks constants should both >= 0. Try adjusting the "
                    "filtering and trimming settings or collect better data.",
                    Ks, Kv, Ka));
  }

  return ffGains;
}

sysid::FeedbackGains AnalysisManager::CalculateFeedback(
    std::vector<double> ff) {
  const auto& Kv = ff[1];
  const auto& Ka = ff[2];
  FeedbackGains fb;
  if (m_settings.type == FeedbackControllerLoopType::kPosition) {
    fb = sysid::CalculatePositionFeedbackGains(
        m_settings.preset, m_settings.lqr, Kv, Ka,
        m_settings.convertGainsToEncTicks
            ? m_settings.gearing * m_settings.cpr * m_factor
            : 1);
  } else {
    fb = sysid::CalculateVelocityFeedbackGains(
        m_settings.preset, m_settings.lqr, Kv, Ka,
        m_settings.convertGainsToEncTicks
            ? m_settings.gearing * m_settings.cpr * m_factor
            : 1);
  }

  return fb;
}

void AnalysisManager::OverrideUnits(std::string_view unit,
                                    double unitsPerRotation) {
  m_unit = unit;
  m_factor = unitsPerRotation;
}

void AnalysisManager::ResetUnitsFromJSON() {
  m_unit = m_json.at("units").get<std::string>();
  m_factor = m_json.at("unitsPerRotation").get<double>();
}
