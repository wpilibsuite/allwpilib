// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sysid/view/Analyzer.h"

#include <algorithm>
#include <exception>
#include <memory>
#include <numbers>
#include <string>
#include <thread>
#include <vector>

#include <fmt/format.h>
#include <glass/Context.h>
#include <glass/Storage.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <wpi/json.h>

#include "sysid/Util.h"
#include "sysid/analysis/AnalysisManager.h"
#include "sysid/analysis/AnalysisType.h"
#include "sysid/analysis/FeedbackControllerPreset.h"
#include "sysid/analysis/FilteringUtils.h"
#include "sysid/view/UILayout.h"

using namespace sysid;

Analyzer::Analyzer(glass::Storage& storage, wpi::Logger& logger)
    : m_logger(logger) {
  // Fill the StringMap with preset values.
  m_presets["Default"] = presets::kDefault;
  m_presets["WPILib"] = presets::kWPILib;
  m_presets["CTRE Phoenix 5"] = presets::kCTREv5;
  m_presets["CTRE Phoenix 6"] = presets::kCTREv6;
  m_presets["REV Brushless Encoder Port"] = presets::kREVNEOBuiltIn;
  m_presets["REV Brushed Encoder Port"] = presets::kREVNonNEO;
  m_presets["REV Data Port"] = presets::kREVNonNEO;
  m_presets["Venom"] = presets::kVenom;

  ResetData();
  UpdateFeedbackGains();
}

void Analyzer::UpdateFeedforwardGains() {
  WPI_INFO(m_logger, "{}", "Gain calc");
  try {
    const auto& feedforwardGains = m_manager->CalculateFeedforward();
    m_feedforwardGains = feedforwardGains;
    m_accelRSquared = feedforwardGains.olsResult.rSquared;
    m_accelRMSE = feedforwardGains.olsResult.rmse;
    m_settings.preset.measurementDelay =
        m_settings.type == FeedbackControllerLoopType::kPosition
            // Clamp feedback measurement delay to ≥ 0
            ? units::math::max(0_s, m_manager->GetPositionDelay())
            : units::math::max(0_s, m_manager->GetVelocityDelay());
    PrepareGraphs();
  } catch (const sysid::InvalidDataError& e) {
    m_state = AnalyzerState::kGeneralDataError;
    HandleError(e.what());
  } catch (const sysid::NoQuasistaticDataError& e) {
    m_state = AnalyzerState::kVelocityThresholdError;
    HandleError(e.what());
  } catch (const sysid::NoDynamicDataError& e) {
    m_state = AnalyzerState::kTestDurationError;
    HandleError(e.what());
  } catch (const AnalysisManager::FileReadingError& e) {
    m_state = AnalyzerState::kFileError;
    HandleError(e.what());
  } catch (const wpi::json::exception& e) {
    m_state = AnalyzerState::kFileError;
    HandleError(e.what());
  } catch (const std::exception& e) {
    m_state = AnalyzerState::kFileError;
    HandleError(e.what());
  }
}

void Analyzer::UpdateFeedbackGains() {
  WPI_INFO(m_logger, "{}", "Updating feedback gains");

  const auto& Kv = m_feedforwardGains.Kv;
  const auto& Ka = m_feedforwardGains.Ka;
  if (Kv.isValidGain && Ka.isValidGain) {
    const auto& fb = m_manager->CalculateFeedback(Kv, Ka);
    m_timescale = units::second_t{Ka.gain / Kv.gain};
    m_timescaleValid = true;
    m_Kp = fb.Kp;
    m_Kd = fb.Kd;
  } else {
    m_timescaleValid = false;
  }
}

bool Analyzer::DisplayDouble(const char* text, double* data,
                             bool readOnly = true) {
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 5);
  if (readOnly) {
    return ImGui::InputDouble(text, data, 0.0, 0.0, "%.5G",
                              ImGuiInputTextFlags_ReadOnly);
  } else {
    return ImGui::InputDouble(text, data, 0.0, 0.0, "%.5G");
  }
}

static void SetPosition(double beginX, double beginY, double xShift,
                        double yShift) {
  ImGui::SetCursorPos(ImVec2(beginX + xShift * 10 * ImGui::GetFontSize(),
                             beginY + yShift * 1.75 * ImGui::GetFontSize()));
}

bool Analyzer::IsErrorState() {
  return m_state == AnalyzerState::kVelocityThresholdError ||
         m_state == AnalyzerState::kTestDurationError ||
         m_state == AnalyzerState::kGeneralDataError ||
         m_state == AnalyzerState::kFileError;
}

bool Analyzer::IsDataErrorState() {
  return m_state == AnalyzerState::kVelocityThresholdError ||
         m_state == AnalyzerState::kTestDurationError ||
         m_state == AnalyzerState::kGeneralDataError;
}

void Analyzer::ResetData() {
  m_plot.ResetData();
  m_manager = std::make_unique<AnalysisManager>(m_settings, m_logger);
  m_feedforwardGains = AnalysisManager::FeedforwardGains{};
  UpdateFeedbackGains();
}

bool Analyzer::DisplayResetAndUnitOverride() {
  auto type = m_manager->GetAnalysisType();
  auto unit = m_manager->GetUnit();

  float width = ImGui::GetContentRegionAvail().x;
  ImGui::SameLine(width - ImGui::CalcTextSize("Reset").x);
  if (ImGui::Button("Reset")) {
    ResetData();
    m_state = AnalyzerState::kWaitingForData;
    return true;
  }

  ImGui::Spacing();
  ImGui::Text(
      "Units:              %s\n"
      "Type:               %s",
      std::string(unit).c_str(), type.name);

  if (ImGui::Button("Override Units")) {
    ImGui::OpenPopup("Override Units");
  }

  auto size = ImGui::GetIO().DisplaySize;
  ImGui::SetNextWindowSize(ImVec2(size.x / 4, size.y * 0.2));
  if (ImGui::BeginPopupModal("Override Units")) {
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 7);
    ImGui::Combo("Units", &m_selectedOverrideUnit, kUnits,
                 IM_ARRAYSIZE(kUnits));
    unit = kUnits[m_selectedOverrideUnit];

    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 7);

    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
      m_manager->OverrideUnits(unit);
      PrepareData();
    }

    ImGui::EndPopup();
  }

  ImGui::SameLine();
  if (ImGui::Button("Reset Units from JSON")) {
    m_manager->ResetUnitsFromJSON();
    PrepareData();
  }

  return false;
}

void Analyzer::ConfigParamsOnFileSelect() {
  WPI_INFO(m_logger, "{}", "Configuring Params");
  m_stepTestDuration = m_settings.stepTestDuration.to<float>();

  // Estimate qp as 1/10 native distance unit
  m_settings.lqr.qp = 0.1;
  // Estimate qv as 1/4 * max velocity = 1/4 * (12V - kS) / kV
  m_settings.lqr.qv =
      0.25 * (12.0 - m_feedforwardGains.Ks.gain) / m_feedforwardGains.Kv.gain;
}

void Analyzer::Display() {
  DisplayGraphs();

  switch (m_state) {
    case AnalyzerState::kWaitingForData: {
      ImGui::Text(
          "SysId is currently in theoretical analysis mode.\n"
          "To analyze recorded test data, select a "
          "data file (.wpilog).");
      sysid::CreateTooltip(
          "Theoretical feedback gains can be calculated from a "
          "physical model of the mechanism being controlled. "
          "Theoretical gains for several common mechanisms can "
          "be obtained from ReCalc (https://reca.lc).");
      ImGui::Spacing();
      ImGui::Spacing();

      ImGui::SetNextItemOpen(true, ImGuiCond_Once);
      if (ImGui::CollapsingHeader("Feedforward Gains (Theoretical)")) {
        float beginX = ImGui::GetCursorPosX();
        float beginY = ImGui::GetCursorPosY();
        CollectFeedforwardGains(beginX, beginY);
      }
      ImGui::SetNextItemOpen(true, ImGuiCond_Once);
      if (ImGui::CollapsingHeader("Feedback Analysis")) {
        DisplayFeedbackGains();
      }
      break;
    }
    case AnalyzerState::kNominalDisplay: {  // Allow the user to select which
                                            // data set they want analyzed and
                                            // add a
      // reset button. Also show the units and the units per rotation.
      if (DisplayResetAndUnitOverride()) {
        return;
      }
      ImGui::Spacing();
      ImGui::Spacing();

      ImGui::SetNextItemOpen(true, ImGuiCond_Once);
      if (ImGui::CollapsingHeader("Feedforward Analysis")) {
        float beginX = ImGui::GetCursorPosX();
        float beginY = ImGui::GetCursorPosY();
        DisplayFeedforwardGains(beginX, beginY);
      }
      ImGui::SetNextItemOpen(true, ImGuiCond_Once);
      if (ImGui::CollapsingHeader("Feedback Analysis")) {
        DisplayFeedbackGains();
      }
      break;
    }
    case AnalyzerState::kFileError: {
      CreateErrorPopup(m_errorPopup, m_exception);
      if (!m_errorPopup) {
        m_state = AnalyzerState::kWaitingForData;
        return;
      }
      break;
    }
    case AnalyzerState::kMissingTestsError: {
      CreateErrorPopup(m_errorPopup, m_exception);
      if (!m_errorPopup) {
        m_state = AnalyzerState::kWaitingForData;
      }
      break;
    }
    case AnalyzerState::kGeneralDataError:
    case AnalyzerState::kTestDurationError:
    case AnalyzerState::kVelocityThresholdError: {
      CreateErrorPopup(m_errorPopup, m_exception);
      if (DisplayResetAndUnitOverride()) {
        return;
      }
      float beginX = ImGui::GetCursorPosX();
      float beginY = ImGui::GetCursorPosY();
      DisplayFeedforwardParameters(beginX, beginY);
      break;
    }
  }
}

void Analyzer::PrepareData() {
  WPI_INFO(m_logger, "{}", "Preparing data");
  try {
    if (m_missingTests.size() > 0) {
      throw sysid::MissingTestsError{m_missingTests};
    }
    m_manager->PrepareData();
    UpdateFeedforwardGains();
    UpdateFeedbackGains();
  } catch (const sysid::InvalidDataError& e) {
    m_state = AnalyzerState::kGeneralDataError;
    HandleError(e.what());
  } catch (const sysid::NoQuasistaticDataError& e) {
    m_state = AnalyzerState::kVelocityThresholdError;
    HandleError(e.what());
  } catch (const sysid::NoDynamicDataError& e) {
    m_state = AnalyzerState::kTestDurationError;
    HandleError(e.what());
  } catch (const sysid::MissingTestsError& e) {
    m_state = AnalyzerState::kMissingTestsError;
    HandleError(e.what());
  } catch (const AnalysisManager::FileReadingError& e) {
    m_state = AnalyzerState::kFileError;
    HandleError(e.what());
  } catch (const wpi::json::exception& e) {
    m_state = AnalyzerState::kFileError;
    HandleError(e.what());
  } catch (const std::exception& e) {
    m_state = AnalyzerState::kFileError;
    HandleError(e.what());
  }
}

void Analyzer::PrepareRawGraphs() {
  if (m_manager->HasData()) {
    AbortDataPrep();
    m_dataThread = std::thread([&] {
      m_plot.SetRawData(m_manager->GetOriginalData(), m_manager->GetUnit(),
                        m_abortDataPrep);
    });
  }
}

void Analyzer::PrepareGraphs() {
  if (m_manager->HasData()) {
    WPI_INFO(m_logger, "{}", "Graph state");
    AbortDataPrep();
    m_dataThread = std::thread([&] {
      m_plot.SetData(m_manager->GetRawData(), m_manager->GetFilteredData(),
                     m_manager->GetUnit(), m_feedforwardGains,
                     m_manager->GetStartTimes(), m_manager->GetAnalysisType(),
                     m_abortDataPrep);
    });
    UpdateFeedbackGains();
    m_state = AnalyzerState::kNominalDisplay;
  }
}

void Analyzer::HandleError(std::string_view msg) {
  m_exception = msg;
  m_errorPopup = true;
  PrepareRawGraphs();
}

void Analyzer::SetMissingTests(const std::vector<std::string>& missingTests) {
  m_missingTests = missingTests;
}

void Analyzer::DisplayGraphs() {
  ImGui::SetNextWindowPos(ImVec2{kDiagnosticPlotWindowPos},
                          ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2{kDiagnosticPlotWindowSize},
                           ImGuiCond_FirstUseEver);
  ImGui::Begin("Diagnostic Plots");

  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6);
  if (ImGui::SliderFloat("Point Size", &m_plot.m_pointSize, 1, 2, "%.2f")) {
    if (!IsErrorState()) {
      PrepareGraphs();
    } else {
      PrepareRawGraphs();
    }
  }

  ImGui::SameLine();
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6);
  const char* items[] = {"Forward", "Backward"};
  if (ImGui::Combo("Direction", &m_plot.m_direction, items, 2)) {
    if (!IsErrorState()) {
      PrepareGraphs();
    } else {
      PrepareRawGraphs();
    }
  }

  // If the plots were already loaded, store the scroll position. Else go to
  // the last recorded scroll position if they have just been initialized
  bool plotsLoaded = m_plot.DisplayPlots();
  if (plotsLoaded) {
    if (m_prevPlotsLoaded) {
      m_graphScroll = ImGui::GetScrollY();
    } else {
      ImGui::SetScrollY(m_graphScroll);
    }

    // If a JSON is selected
    if (m_state == AnalyzerState::kNominalDisplay) {
      DisplayDouble("Acceleration R²", &m_accelRSquared);
      CreateTooltip(
          "The coefficient of determination of the OLS fit of acceleration "
          "versus velocity and voltage.  Acceleration is extremely noisy, "
          "so this is generally quite small.");

      ImGui::SameLine();
      DisplayDouble("Acceleration RMSE", &m_accelRMSE);
      CreateTooltip(
          "The standard deviation of the residuals from the predicted "
          "acceleration."
          "This can be interpreted loosely as the mean measured disturbance "
          "from the \"ideal\" system equation.");

      DisplayDouble("Sim velocity R²", m_plot.GetSimRSquared());
      CreateTooltip(
          "The coefficient of determination the simulated velocity. "
          "Velocity is much less-noisy than acceleration, so this "
          "is pretty close to 1 for a decent fit.");

      ImGui::SameLine();
      DisplayDouble("Sim velocity RMSE", m_plot.GetSimRMSE());
      CreateTooltip(
          "The standard deviation of the residuals from the simulated velocity "
          "predictions - essentially the size of the mean error of the "
          "simulated model "
          "in the recorded velocity units.");
    }
  }
  m_prevPlotsLoaded = plotsLoaded;

  ImGui::End();
}

void Analyzer::AnalyzeData() {
  m_manager = std::make_unique<AnalysisManager>(m_data, m_settings, m_logger);
  PrepareData();
  m_dataset = 0;
  ConfigParamsOnFileSelect();
  UpdateFeedbackGains();
}

void Analyzer::AbortDataPrep() {
  if (m_dataThread.joinable()) {
    m_abortDataPrep = true;
    m_dataThread.join();
    m_abortDataPrep = false;
  }
}

void Analyzer::DisplayFeedforwardParameters(float beginX, float beginY) {
  // Increase spacing to not run into trackwidth in the normal analyzer view
  constexpr double kHorizontalOffset = 1.1;
  SetPosition(beginX, beginY, kHorizontalOffset, 0);

  bool displayAll =
      !IsErrorState() || m_state == AnalyzerState::kGeneralDataError;

  if (displayAll) {
    // Wait for enter before refresh so double digit entries like "15" don't
    // prematurely refresh with "1". That can cause display stuttering.
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
    int window = m_settings.medianWindow;
    if (ImGui::InputInt("Window Size", &window, 0, 0,
                        ImGuiInputTextFlags_EnterReturnsTrue)) {
      m_settings.medianWindow = std::clamp(window, 1, 15);
      PrepareData();
    }

    CreateTooltip(
        "The number of samples in the velocity median "
        "filter's sliding window.");
  }

  if (displayAll || m_state == AnalyzerState::kVelocityThresholdError) {
    // Wait for enter before refresh so decimal inputs like "0.2" don't
    // prematurely refresh with a velocity threshold of "0".
    SetPosition(beginX, beginY, kHorizontalOffset, 1);
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
    double threshold = m_settings.velocityThreshold;
    if (ImGui::InputDouble("Velocity Threshold", &threshold, 0.0, 0.0, "%.3f",
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
      m_settings.velocityThreshold = std::max(0.0, threshold);
      PrepareData();
    }
    CreateTooltip("Velocity data below this threshold will be ignored.");
  }

  if (displayAll || m_state == AnalyzerState::kTestDurationError) {
    SetPosition(beginX, beginY, kHorizontalOffset, 2);
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
    if (ImGui::SliderFloat("Test Duration", &m_stepTestDuration,
                           m_manager->GetMinStepTime().value(),
                           m_manager->GetMaxStepTime().value(), "%.2f")) {
      m_settings.stepTestDuration = units::second_t{m_stepTestDuration};
      PrepareData();
    }
  }
}

void Analyzer::CollectFeedforwardGains(float beginX, float beginY) {
  SetPosition(beginX, beginY, 0, 0);
  if (DisplayDouble("Kv", &m_feedforwardGains.Kv.gain, false)) {
    UpdateFeedbackGains();
  }

  SetPosition(beginX, beginY, 0, 1);
  if (DisplayDouble("Ka", &m_feedforwardGains.Ka.gain, false)) {
    UpdateFeedbackGains();
  }

  SetPosition(beginX, beginY, 0, 2);
  // Show Timescale
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
  DisplayDouble("Response Timescale (ms)",
                reinterpret_cast<double*>(&m_timescale));
  CreateTooltip(
      "The characteristic timescale of the system response in milliseconds. "
      "Both the control loop period and total signal delay should be "
      "at least 3-5 times shorter than this to optimally control the "
      "system.");
}

void Analyzer::DisplayFeedforwardGain(const char* text,
                                      AnalysisManager::FeedforwardGain& ffGain,
                                      bool readOnly = true) {
  DisplayDouble(text, &ffGain.gain, readOnly);
  if (!ffGain.isValidGain) {
    // Display invalid gain message with warning and tooltip
    CreateErrorTooltip(ffGain.errorMessage.c_str());
  }

  // Display descriptor message as tooltip, whether the gain is valid or not
  CreateTooltip(ffGain.descriptor.c_str());
}

void Analyzer::DisplayFeedforwardGains(float beginX, float beginY) {
  SetPosition(beginX, beginY, 0, 0);
  DisplayFeedforwardGain("Ks", m_feedforwardGains.Ks);

  SetPosition(beginX, beginY, 0, 1);
  DisplayFeedforwardGain("Kv", m_feedforwardGains.Kv);

  SetPosition(beginX, beginY, 0, 2);
  DisplayFeedforwardGain("Ka", m_feedforwardGains.Ka);

  SetPosition(beginX, beginY, 0, 3);
  // Show Timescale
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
  DisplayDouble("Response Timescale (ms)",
                reinterpret_cast<double*>(&m_timescale));
  if (!m_timescaleValid) {
    CreateErrorTooltip(
        "Response timescale calculation invalid. Ensure that calculated gains "
        "are valid.");
  }
  CreateTooltip(
      "The characteristic timescale of the system response in milliseconds. "
      "Both the control loop period and total signal delay should be "
      "at least 3-5 times shorter than this to optimally control the "
      "system.");

  SetPosition(beginX, beginY, 0, 4);
  auto positionDelay = m_manager->GetPositionDelay();
  DisplayDouble("Position Measurement Delay (ms)",
                reinterpret_cast<double*>(&positionDelay));
  CreateTooltip(
      "The average elapsed time between the first application of "
      "voltage and the first detected change in mechanism position "
      "in the step-voltage tests.  This includes CAN delays, and "
      "may overestimate the true delay for on-motor-controller "
      "feedback loops by up to 20ms.");

  SetPosition(beginX, beginY, 0, 5);
  auto velocityDelay = m_manager->GetVelocityDelay();
  DisplayDouble("Velocity Measurement Delay (ms)",
                reinterpret_cast<double*>(&velocityDelay));
  CreateTooltip(
      "The average elapsed time between the first application of "
      "voltage and the maximum calculated mechanism acceleration "
      "in the step-voltage tests.  This includes CAN delays, and "
      "may overestimate the true delay for on-motor-controller "
      "feedback loops by up to 20ms.");

  SetPosition(beginX, beginY, 0, 6);

  if (m_manager->GetAnalysisType() == analysis::kElevator) {
    DisplayFeedforwardGain("Kg", m_feedforwardGains.Kg);
  } else if (m_manager->GetAnalysisType() == analysis::kArm) {
    DisplayFeedforwardGain("Kg", m_feedforwardGains.Kg);

    double offset;
    auto unit = m_manager->GetUnit();
    if (unit == "Radians") {
      offset = m_feedforwardGains.offset.gain;
    } else if (unit == "Degrees") {
      offset = m_feedforwardGains.offset.gain / std::numbers::pi * 180.0;
    } else if (unit == "Rotations") {
      offset = m_feedforwardGains.offset.gain / (2 * std::numbers::pi);
    }
    DisplayDouble(
        fmt::format("Angle offset to horizontal ({})", GetAbbreviation(unit))
            .c_str(),
        &offset);
    CreateTooltip(
        "This is the angle offset which, when added to the angle measurement, "
        "zeroes it out when the arm is horizontal. This is needed for the arm "
        "feedforward to work.");
  }
  double endY = ImGui::GetCursorPosY();

  DisplayFeedforwardParameters(beginX, beginY);
  ImGui::SetCursorPosY(endY);
}

void Analyzer::DisplayFeedbackGains() {
  // Allow the user to select a feedback controller preset.
  ImGui::Spacing();
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * kTextBoxWidthMultiple);
  if (ImGui::Combo("Gain Preset", &m_selectedPreset, kPresetNames,
                   IM_ARRAYSIZE(kPresetNames))) {
    m_settings.preset = m_presets[kPresetNames[m_selectedPreset]];
    m_settings.type = FeedbackControllerLoopType::kVelocity;
    m_selectedLoopType =
        static_cast<int>(FeedbackControllerLoopType::kVelocity);
    UpdateFeedbackGains();
  }
  ImGui::SameLine();
  sysid::CreateTooltip(
      "Gain presets represent how feedback gains are calculated for your "
      "specific feedback controller:\n\n"
      "Default, WPILib (2020-): For use with the new WPILib PIDController "
      "class.\n"
      "WPILib (Pre-2020): For use with the old WPILib PIDController class.\n"
      "CTRE: For use with CTRE units. These are the default units that ship "
      "with CTRE motor controllers.\n"
      "REV (Brushless): For use with NEO and NEO 550 motors on a SPARK MAX.\n"
      "REV (Brushed): For use with brushed motors connected to a SPARK MAX.");

  if (m_settings.preset != m_presets[kPresetNames[m_selectedPreset]]) {
    ImGui::SameLine();
    ImGui::TextDisabled("(modified)");
  }

  // Show our feedback controller preset values.
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
  double value = m_settings.preset.outputConversionFactor * 12;
  if (ImGui::InputDouble("Max Controller Output", &value, 0.0, 0.0, "%.1f") &&
      value > 0) {
    m_settings.preset.outputConversionFactor = value / 12.0;
    UpdateFeedbackGains();
  }

  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
  value = m_settings.preset.outputVelocityTimeFactor;
  if (ImGui::InputDouble("Velocity Denominator Units (s)", &value, 0.0, 0.0,
                         "%.1f") &&
      value > 0) {
    m_settings.preset.outputVelocityTimeFactor = value;
    UpdateFeedbackGains();
  }

  sysid::CreateTooltip(
      "This represents the denominator of the velocity unit used by the "
      "feedback controller. For example, CTRE uses 100 ms = 0.1 s.");

  auto ShowPresetValue = [](const char* text, double* data,
                            float cursorX = 0.0f) {
    if (cursorX > 0) {
      ImGui::SetCursorPosX(cursorX);
    }

    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
    return ImGui::InputDouble(text, data, 0.0, 0.0, "%.5G");
  };

  // Show controller period.
  if (ShowPresetValue("Controller Period (ms)",
                      reinterpret_cast<double*>(&m_settings.preset.period))) {
    if (m_settings.preset.period > 0_s &&
        m_settings.preset.measurementDelay >= 0_s) {
      UpdateFeedbackGains();
    }
  }

  // Show whether the controller gains are time-normalized.
  if (ImGui::Checkbox("Time-Normalized?", &m_settings.preset.normalized)) {
    UpdateFeedbackGains();
  }

  // Show position/velocity measurement delay.
  if (ShowPresetValue(
          "Measurement Delay (ms)",
          reinterpret_cast<double*>(&m_settings.preset.measurementDelay))) {
    if (m_settings.preset.period > 0_s &&
        m_settings.preset.measurementDelay >= 0_s) {
      UpdateFeedbackGains();
    }
  }

  sysid::CreateTooltip(
      "The average measurement delay of the process variable in milliseconds. "
      "This may depend on your encoder settings and choice of motor "
      "controller. Default velocity filtering windows are quite long "
      "on many motor controllers, so be careful that this value is "
      "accurate if the characteristic timescale of the mechanism "
      "is small.");

  ImGui::Separator();
  ImGui::Spacing();

  // Allow the user to select a loop type.
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * kTextBoxWidthMultiple);
  if (ImGui::Combo("Loop Type", &m_selectedLoopType, kLoopTypes,
                   IM_ARRAYSIZE(kLoopTypes))) {
    m_settings.type =
        static_cast<FeedbackControllerLoopType>(m_selectedLoopType);
    if (m_state == AnalyzerState::kWaitingForData) {
      m_settings.preset.measurementDelay = 0_ms;
    } else {
      if (m_settings.type == FeedbackControllerLoopType::kPosition) {
        m_settings.preset.measurementDelay = m_manager->GetPositionDelay();
      } else {
        m_settings.preset.measurementDelay = m_manager->GetVelocityDelay();
      }
    }
    UpdateFeedbackGains();
  }

  ImGui::Spacing();

  // Show Kp and Kd.
  float beginY = ImGui::GetCursorPosY();
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
  DisplayDouble("Kp", &m_Kp);

  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
  DisplayDouble("Kd", &m_Kd);

  // Come back to the starting y pos.
  ImGui::SetCursorPosY(beginY);

  if (m_selectedLoopType == 0) {
    std::string unit;
    if (m_state != AnalyzerState::kWaitingForData) {
      unit = fmt::format(" ({})", GetAbbreviation(m_manager->GetUnit()));
    }

    ImGui::SetCursorPosX(ImGui::GetFontSize() * 9);
    if (DisplayDouble(fmt::format("Max Position Error{}", unit).c_str(),
                      &m_settings.lqr.qp, false)) {
      if (m_settings.lqr.qp > 0) {
        UpdateFeedbackGains();
      }
    }
  }

  std::string unit;
  if (m_state != AnalyzerState::kWaitingForData) {
    unit = fmt::format(" ({}/s)", GetAbbreviation(m_manager->GetUnit()));
  }

  ImGui::SetCursorPosX(ImGui::GetFontSize() * 9);
  if (DisplayDouble(fmt::format("Max Velocity Error{}", unit).c_str(),
                    &m_settings.lqr.qv, false)) {
    if (m_settings.lqr.qv > 0) {
      UpdateFeedbackGains();
    }
  }
  ImGui::SetCursorPosX(ImGui::GetFontSize() * 9);
  if (DisplayDouble("Max Control Effort (V)", &m_settings.lqr.r, false)) {
    if (m_settings.lqr.r > 0) {
      UpdateFeedbackGains();
    }
  }
}
