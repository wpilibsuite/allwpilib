// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <array>
#include <exception>
#include <limits>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

#include <units/time.h>
#include <wpi/Logger.h>
#include <wpi/StringMap.h>
#include <wpi/json.h>

#include "sysid/analysis/AnalysisType.h"
#include "sysid/analysis/FeedbackAnalysis.h"
#include "sysid/analysis/FeedbackControllerPreset.h"
#include "sysid/analysis/OLS.h"
#include "sysid/analysis/Storage.h"

namespace sysid {

/**
 * Manages analysis of data. Each instance of this class represents a JSON file
 * that is read from storage.
 */
class AnalysisManager {
 public:
  // This contains data for each test (e.g. quasistatic-forward,
  // quasistatic-backward, etc) indexed by test name
  TestData m_data;
  /**
   * Represents settings for an instance of the analysis manager. This contains
   * information about the feedback controller preset, loop type, velocity
   * threshold, acceleration window size, LQR parameters, and the selected
   * dataset.
   */
  struct Settings {
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
     * The velocity threshold (units/s) for trimming quasistatic test data.
     */
    double velocityThreshold = 0.2;

    /**
     * The window size for the median filter.
     */
    int medianWindow = 1;

    /**
     * The duration of the dynamic test that should be considered. A value of
     * zero indicates it needs to be set to the default.
     */
    units::second_t stepTestDuration = 0_s;
  };

  struct FeedforwardGain {
    /**
     * The feedforward gain.
     */
    double gain = 1;

    /**
     * Descriptor attached to the feedforward gain.
     */
    std::string descriptor = "Feedforward gain.";

    /**
     * Whether the feedforward gain is valid.
     */
    bool isValidGain = true;

    /**
     * Error message attached to the feedforward gain.
     */
    std::string errorMessage = "No error.";
  };

  /**
   * Stores feedforward gains.
   */
  struct FeedforwardGains {
    /**
     * Stores the raw OLSResult from analysis.
     */
    OLSResult olsResult;

    /**
     * The static gain Ks.
     */
    FeedforwardGain Ks = {};

    /**
     * The velocity gain kV.
     */
    FeedforwardGain Kv = {};

    /**
     * The acceleration gain kA.
     */
    FeedforwardGain Ka = {};

    /**
     * The gravity gain Kg.
     */
    FeedforwardGain Kg = {};

    /**
     * The offset (arm).
     */
    FeedforwardGain offset = {};
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
      "quasistatic-forward", "quasistatic-reverse", "dynamic-forward",
      "dynamic-reverse"};

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
   * @param data     The data from the SysId routine.
   * @param settings The settings for this instance of the analysis manager.
   * @param logger   The logger instance to use for log data.
   */
  AnalysisManager(TestData data, Settings& settings, wpi::Logger& logger);

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
  FeedbackGains CalculateFeedback(const FeedforwardGain& Kv,
                                  const FeedforwardGain& Ka);

  /**
   * Overrides the units in the JSON with the user-provided ones.
   *
   * @param unit             The unit to output gains in.
   */
  void OverrideUnits(std::string_view unit);

  /**
   * Resets the units back to those defined in the JSON.
   */
  void ResetUnitsFromJSON();

  /**
   * Returns the analysis type of the current instance (read from the JSON).
   *
   * @return The analysis type.
   */
  const AnalysisType& GetAnalysisType() const { return m_data.mechanismType; }

  /**
   * Returns the units of analysis.
   *
   * @return The units of analysis.
   */
  std::string_view GetUnit() const { return m_data.distanceUnit; }

  /**
   * Returns a reference to the iterator of the currently selected raw dataset.
   * Unfortunately, due to ImPlot internals, the reference cannot be const so
   * the user should be careful not to change any data.
   *
   * @return A reference to the raw internal data.
   */
  Storage& GetRawData() { return m_rawDataset; }

  /**
   * Returns a reference to the iterator of the currently selected filtered
   * dataset. Unfortunately, due to ImPlot internals, the reference cannot be
   * const so the user should be careful not to change any data.
   *
   * @return A reference to the filtered internal data.
   */
  Storage& GetFilteredData() { return m_filteredDataset; }

  /**
   * Returns the original dataset.
   *
   * @return The original (untouched) dataset
   */
  Storage& GetOriginalData() { return m_originalDataset; }

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

  bool HasData() const { return !m_originalDataset.empty(); }

 private:
  wpi::Logger& m_logger;

  Storage m_originalDataset;
  Storage m_rawDataset;
  Storage m_filteredDataset;

  // Stores the various start times of the different tests.
  std::array<units::second_t, 4> m_startTimes;

  // The settings for this instance. This contains pointers to the feedback
  // controller preset, LQR parameters, acceleration window size, etc.
  Settings& m_settings;

  units::second_t m_minStepTime{0};
  units::second_t m_maxStepTime{std::numeric_limits<double>::infinity()};
  std::vector<units::second_t> m_positionDelays;
  std::vector<units::second_t> m_velocityDelays;

  void PrepareGeneralData();
};
}  // namespace sysid
