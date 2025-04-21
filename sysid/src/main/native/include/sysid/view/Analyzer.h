// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <glass/View.h>
#include <implot.h>
#include <portable-file-dialogs.h>
#include <units/time.h>
#include <units/voltage.h>
#include <wpi/Logger.h>
#include <wpi/StringMap.h>

#include "sysid/analysis/AnalysisManager.h"
#include "sysid/analysis/FeedbackAnalysis.h"
#include "sysid/analysis/FeedbackControllerPreset.h"
#include "sysid/view/AnalyzerPlot.h"

struct ImPlotPoint;

namespace glass {
class Storage;
}  // namespace glass

namespace sysid {
/**
 * The Analyzer GUI takes care of providing the user with a user interface to
 * load their data, visualize the data, adjust certain variables, and then view
 * the calculated gains.
 */
class Analyzer : public glass::View {
 public:
  TestData m_data;
  /**
   * The different display and processing states for the GUI
   */
  enum class AnalyzerState {
    kWaitingForData,
    kNominalDisplay,
    kVelocityThresholdError,
    kTestDurationError,
    kGeneralDataError,
    kMissingTestsError,
    kFileError
  };
  /**
   * The different motor controller timing presets that can be used.
   */
  static constexpr const char* kPresetNames[] = {"Default",
                                                 "WPILib",
                                                 "CTRE Phoenix 5 CANcoder",
                                                 "CTRE Phoenix 5",
                                                 "CTRE Phoenix 6",
                                                 "REV Brushless Encoder Port",
                                                 "REV Brushed Encoder Port",
                                                 "REV Data Port",
                                                 "Venom"};

  /**
   * The different control loops that can be used.
   */
  static constexpr const char* kLoopTypes[] = {"Position", "Velocity"};

  /**
   * Linear drivetrain analysis subsets
   */
  static constexpr const char* kDatasets[] = {"Combined", "Left", "Right"};

  /**
   * Creates the Analyzer widget
   *
   * @param storage Glass Storage
   * @param logger The program logger
   */
  Analyzer(glass::Storage& storage, wpi::Logger& logger);

  /**
   * Displays the analyzer widget
   */
  void Display() override;

  ~Analyzer() override { AbortDataPrep(); };

  /**
   * Analyzes the selected data.
   */
  void AnalyzeData();

  /**
   * Used by DataSelector to import any missing tests.
   */
  void SetMissingTests(const std::vector<std::string>& missingTests);

 private:
  /**
   * Kills the data preparation thread
   */
  void AbortDataPrep();

  /**
   * Displays the settings to adjust trimming and filtering for feedforward
   * gains.
   */
  void DisplayFeedforwardParameters(float beginX, float beginY);

  /**
   * Displays the graphs of the data.
   */
  void DisplayGraphs();

  /**
   * Resets the current analysis data.
   */
  void ResetData();

  /**
   * Sets up the reset button and Unit override buttons.
   *
   * @return True if the tool had been reset.
   */
  bool DisplayResetAndUnitOverride();

  /**
   * Prepares the data for analysis.
   */
  void PrepareData();

  /**
   * Sets up the graphs to display Raw Data.
   */
  void PrepareRawGraphs();

  /**
   * Sets up the graphs to display filtered/processed data.
   */
  void PrepareGraphs();

  /**
   * True if the stored state is associated with an error.
   */
  bool IsErrorState();

  /**
   * True if the stored state is associated with a data processing error.
   */
  bool IsDataErrorState();

  /**
   * Displays inputs to allow the collecting of theoretical feedforward gains.
   */
  void CollectFeedforwardGains(float beginX, float beginY);

  /**
   * Displays calculated feedforward gains.
   */
  void DisplayFeedforwardGains(float beginX, float beginY);

  /**
   * Displays calculated feedback gains.
   */
  void DisplayFeedbackGains();

  /**
   * Estimates ideal step test duration, qp, and qv for the LQR based off of the
   * data given
   */
  void ConfigParamsOnFileSelect();

  /**
   * Updates feedforward gains from the analysis manager.
   */
  void UpdateFeedforwardGains();

  /**
   * Updates feedback gains from the analysis manager.
   */
  void UpdateFeedbackGains();

  /**
   * Handles logic of displaying a double on ImGui.
   */
  bool DisplayDouble(const char* text, double* data, bool readOnly);

  /**
   * Displays a Feedforward gain, including the gain itself along with its
   * validity and message.
   */
  void DisplayFeedforwardGain(const char* text,
                              AnalysisManager::FeedforwardGain& ffGain,
                              bool readOnly);

  /**
   * Handles errors when they pop up.
   */
  void HandleError(std::string_view msg);

  // State of the Display GUI
  AnalyzerState m_state = AnalyzerState::kWaitingForData;

  // Stores the exception message.
  std::string m_exception;
  std::vector<std::string> m_missingTests;

  bool m_calcDefaults = false;

  // This is true if the error popup needs to be displayed
  bool m_errorPopup = false;

  // Everything related to feedback controller calculations.
  AnalysisManager::Settings m_settings;
  wpi::StringMap<FeedbackControllerPreset> m_presets;

  int m_selectedLoopType = 1;
  int m_selectedPreset = 0;

  // Feedforward and feedback gains.
  AnalysisManager::FeedforwardGains m_feedforwardGains;
  double m_accelRSquared;
  double m_accelRMSE;
  double m_Kp;
  double m_Kd;
  units::millisecond_t m_timescale;
  bool m_timescaleValid = false;

  // Units
  int m_selectedOverrideUnit = 0;

  // Data analysis
  std::unique_ptr<AnalysisManager> m_manager;
  int m_dataset = 0;
  int m_window = 8;
  double m_threshold = 0.2;
  float m_stepTestDuration = 0;

  bool combinedGraphFit = false;

  // Logger
  wpi::Logger& m_logger;

  // Plot
  AnalyzerPlot m_plot{m_logger};
  bool m_prevPlotsLoaded = false;

  // Stores graph scroll bar position and states for keeping track of scroll
  // positions after loading graphs
  float m_graphScroll = 0;

  std::atomic<bool> m_abortDataPrep{false};
  std::thread m_dataThread;
};
}  // namespace sysid
