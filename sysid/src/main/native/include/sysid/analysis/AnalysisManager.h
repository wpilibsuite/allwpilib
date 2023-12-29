// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <array>
#include <exception>
#include <limits>
#include <numeric>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include <units/time.h>
#include <wpi/Logger.h>
#include <wpi/json.h>

#include "sysid/analysis/AnalysisType.h"
#include "sysid/analysis/FeedbackAnalysis.h"
#include "sysid/analysis/FeedbackControllerPreset.h"
#include "sysid/analysis/FeedforwardAnalysis.h"
#include "sysid/analysis/Storage.h"

namespace sysid {

/**
 * Manages analysis of data. Each instance of this class represents a JSON file
 * that is read from storage.
 */
class AnalysisManager {
 public:
  /**
   * Represents settings for an instance of the analysis manager. This contains
   * information about the feedback controller preset, loop type, motion
   * threshold, acceleration window size, LQR parameters, and the selected
   * dataset.
   */
  struct Settings {
    enum class DrivetrainDataset { kCombined = 0, kLeft = 1, kRight = 2 };
    /**
     * The feedback controller preset used to calculate gains.
     */
    FeedbackControllerPreset preset = presets::kDefault;

    /**
     * The feedback controller loop type (position or velocity).
     */
    FeedbackControllerLoopType type = FeedbackControllerLoopType::kVelocity;

    /**
     * LQR parameters used for feedback gain calculation.
     */
    LQRParameters lqr{1, 1.5, 7};

    /**
     * The motion threshold (units/s) for trimming quasistatic test data.
     */
    double motionThreshold = 0.2;

    /**
     * The window size for the median filter.
     */
    int medianWindow = 1;

    /**
     * The duration of the dynamic test that should be considered. A value of
     * zero indicates it needs to be set to the default.
     */
    units::second_t stepTestDuration = 0_s;

    /**
     * The conversion factor of counts per revolution.
     */
    int cpr = 1440;

    /**
     * The conversion factor of gearing.
     */
    double gearing = 1;

    /**
     * Whether or not the gains should be in the encoder's units (mainly for use
     * in a smart motor controller).
     */
    bool convertGainsToEncTicks = false;

    DrivetrainDataset dataset = DrivetrainDataset::kCombined;
  };

  /**
   * Stores feedforward.
   */
  struct FeedforwardGains {
    /**
     * Stores the Feedforward gains.
     */
    OLSResult ffGains;

    /**
     * Stores the trackwidth for angular drivetrain tests.
     */
    std::optional<double> trackWidth;
  };

  /**
   * Exception for File Reading Errors.
   */
  class FileReadingError : public std::exception {
   public:
    /**
     * Creates a FileReadingError object
     *
     * @param path The path of the file attempted to open
     */
    explicit FileReadingError(std::string_view path) {
      msg = fmt::format("Unable to read: {}", path);
    }

    const char* what() const noexcept override { return msg.c_str(); }

   private:
    /**
     * The path of the file that was opened.
     */
    std::string msg;
  };

  /**
   * The keys (which contain sysid data) that are in the JSON to analyze.
   */
  static constexpr const char* kJsonDataKeys[] = {
      "slow-forward", "slow-backward", "fast-forward", "fast-backward"};

  /**
   * Concatenates a list of vectors. The contents of the source vectors are
   * copied (not moved) into the new vector. Also sorts the datapoints by
   * timestamp to assist with future simulation.
   *
   * @param sources The source vectors.
   * @return The concatenated vector
   */
  template <typename... Sources>
  static std::vector<PreparedData> DataConcat(const Sources&... sources) {
    std::vector<PreparedData> result;
    (result.insert(result.end(), sources.begin(), sources.end()), ...);

    // Sort data by timestamp to remove the possibility of negative dts in
    // future simulations.
    std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) {
      return a.timestamp < b.timestamp;
    });

    return result;
  }

  /**
   * Constructs an instance of the analysis manager for theoretical analysis,
   * containing settings and gains but no data.
   *
   * @param settings The settings for this instance of the analysis manager.
   * @param logger The logger instance to use for log data.
   */
  AnalysisManager(Settings& settings, wpi::Logger& logger);

  /**
   * Constructs an instance of the analysis manager with the given path (to the
   * JSON) and analysis manager settings.
   *
   * @param path     The path to the JSON containing the sysid data.
   * @param settings The settings for this instance of the analysis manager.
   * @param logger   The logger instance to use for log data.
   */
  AnalysisManager(std::string_view path, Settings& settings,
                  wpi::Logger& logger);

  /**
   * Prepares data from the JSON and stores the output in Storage member
   * variables.
   */
  void PrepareData();

  /**
   * Calculates the gains with the latest data (from the pointers in the
   * settings struct that this instance was constructed with).
   *
   * @return The latest feedforward gains and trackwidth (if needed).
   */
  FeedforwardGains CalculateFeedforward();

  /**
   * Calculates feedback gains from the given feedforward gains.
   *
   * @param ff The feedforward gains.
   * @return The calculated feedback gains.
   */
  FeedbackGains CalculateFeedback(std::vector<double> ff);

  /**
   * Overrides the units in the JSON with the user-provided ones.
   *
   * @param unit             The unit to output gains in.
   * @param unitsPerRotation The conversion factor between rotations and the
   *                         selected unit.
   */
  void OverrideUnits(std::string_view unit, double unitsPerRotation);

  /**
   * Resets the units back to those defined in the JSON.
   */
  void ResetUnitsFromJSON();

  /**
   * Returns the analysis type of the current instance (read from the JSON).
   *
   * @return The analysis type.
   */
  const AnalysisType& GetAnalysisType() const { return m_type; }

  /**
   * Returns the units of analysis.
   *
   * @return The units of analysis.
   */
  std::string_view GetUnit() const { return m_unit; }

  /**
   * Returns the factor (a.k.a. units per rotation) for analysis.
   *
   * @return The factor (a.k.a. units per rotation) for analysis.
   */
  double GetFactor() const { return m_factor; }

  /**
   * Returns a reference to the iterator of the currently selected raw datset.
   * Unfortunately, due to ImPlot internals, the reference cannot be const so
   * the user should be careful not to change any data.
   *
   * @return A reference to the raw internal data.
   */
  Storage& GetRawData() {
    return m_rawDataset[static_cast<int>(m_settings.dataset)];
  }

  /**
   * Returns a reference to the iterator of the currently selected filtered
   * datset. Unfortunately, due to ImPlot internals, the reference cannot be
   * const so the user should be careful not to change any data.
   *
   * @return A reference to the filtered internal data.
   */
  Storage& GetFilteredData() {
    return m_filteredDataset[static_cast<int>(m_settings.dataset)];
  }

  /**
   * Returns the original dataset.
   *
   * @return The original (untouched) dataset
   */
  Storage& GetOriginalData() {
    return m_originalDataset[static_cast<int>(m_settings.dataset)];
  }

  /**
   * Returns the minimum duration of the Step Voltage Test of the currently
   * stored data.
   *
   * @return The minimum step test duration.
   */
  units::second_t GetMinStepTime() const { return m_minStepTime; }

  /**
   * Returns the maximum duration of the Step Voltage Test of the currently
   * stored data.
   *
   * @return  Maximum step test duration
   */
  units::second_t GetMaxStepTime() const { return m_maxStepTime; }

  /**
   * Returns the estimated time delay of the measured position, including
   * CAN delays.
   *
   * @return Position delay in milliseconds
   */
  units::millisecond_t GetPositionDelay() const {
    return std::accumulate(m_positionDelays.begin(), m_positionDelays.end(),
                           0_s) /
           m_positionDelays.size();
  }

  /**
   * Returns the estimated time delay of the measured velocity, including
   * CAN delays.
   *
   * @return Velocity delay in milliseconds
   */
  units::millisecond_t GetVelocityDelay() const {
    return std::accumulate(m_velocityDelays.begin(), m_velocityDelays.end(),
                           0_s) /
           m_positionDelays.size();
  }

  /**
   * Returns the different start times of the recorded tests.
   *
   * @return The start times for each test
   */
  const std::array<units::second_t, 4>& GetStartTimes() const {
    return m_startTimes;
  }

  bool HasData() const {
    return !m_originalDataset[static_cast<int>(
                                  Settings::DrivetrainDataset::kCombined)]
                .empty();
  }

 private:
  wpi::Logger& m_logger;

  // This is used to store the various datasets (i.e. Combined, Forward,
  // Backward, etc.)
  wpi::json m_json;

  std::array<Storage, 3> m_originalDataset;
  std::array<Storage, 3> m_rawDataset;
  std::array<Storage, 3> m_filteredDataset;

  // Stores the various start times of the different tests.
  std::array<units::second_t, 4> m_startTimes;

  // The settings for this instance. This contains pointers to the feedback
  // controller preset, LQR parameters, acceleration window size, etc.
  Settings& m_settings;

  // Miscellaneous data from the JSON -- the analysis type, the units, and the
  // units per rotation.
  AnalysisType m_type;
  std::string m_unit;
  double m_factor;

  units::second_t m_minStepTime{0};
  units::second_t m_maxStepTime{std::numeric_limits<double>::infinity()};
  std::vector<units::second_t> m_positionDelays;
  std::vector<units::second_t> m_velocityDelays;

  // Stores an optional track width if we are doing the drivetrain angular test.
  std::optional<double> m_trackWidth;

  void PrepareGeneralData();

  void PrepareAngularDrivetrainData();

  void PrepareLinearDrivetrainData();
};
}  // namespace sysid
