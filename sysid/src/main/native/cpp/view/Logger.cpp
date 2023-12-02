// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "sysid/view/Logger.h"

#include <exception>
#include <numbers>

#include <glass/Context.h>
#include <glass/Storage.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <networktables/NetworkTable.h>
#include <units/angle.h>
#include <wpigui.h>

#include "sysid/Util.h"
#include "sysid/analysis/AnalysisType.h"
#include "sysid/view/UILayout.h"

using namespace sysid;

Logger::Logger(glass::Storage& storage, wpi::Logger& logger)
    : m_logger{logger}, m_ntSettings{"sysid", storage} {
  wpi::gui::AddEarlyExecute([&] { m_ntSettings.Update(); });

  m_ntSettings.EnableServerOption(false);
}

void Logger::Display() {
  // Get the current width of the window. This will be used to scale
  // our UI elements.
  float width = ImGui::GetContentRegionAvail().x;

  // Add team number input and apply button for NT connection.
  m_ntSettings.Display();

  // Reset and clear the internal manager state.
  ImGui::SameLine();
  if (ImGui::Button("Reset Telemetry")) {
    m_settings = TelemetryManager::Settings{};
    m_manager = std::make_unique<TelemetryManager>(m_settings, m_logger);
    m_settings.mechanism = analysis::FromName(kTypes[m_selectedType]);
  }

  // Add NT connection indicator.
  static ImVec4 kColorDisconnected{1.0f, 0.4f, 0.4f, 1.0f};
  static ImVec4 kColorConnected{0.2f, 1.0f, 0.2f, 1.0f};
  ImGui::SameLine();
  bool ntConnected = nt::NetworkTableInstance::GetDefault().IsConnected();
  ImGui::TextColored(ntConnected ? kColorConnected : kColorDisconnected,
                     ntConnected ? "NT Connected" : "NT Disconnected");

  // Create a Section for project configuration
  ImGui::Separator();
  ImGui::Spacing();
  ImGui::Text("Project Parameters");

  // Add a dropdown for mechanism type.
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * kTextBoxWidthMultiple);

  if (ImGui::Combo("Mechanism", &m_selectedType, kTypes,
                   IM_ARRAYSIZE(kTypes))) {
    m_settings.mechanism = analysis::FromName(kTypes[m_selectedType]);
  }

  // Add Dropdown for Units
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * kTextBoxWidthMultiple);
  if (ImGui::Combo("Unit Type", &m_selectedUnit, kUnits,
                   IM_ARRAYSIZE(kUnits))) {
    m_settings.units = kUnits[m_selectedUnit];
  }

  sysid::CreateTooltip(
      "This is the type of units that your gains will be in. For example, if "
      "you want your flywheel gains in terms of radians, then use the radians "
      "unit. On the other hand, if your drivetrain will use gains in meters, "
      "choose meters.");

  // Rotational units have fixed Units per rotations
  m_isRotationalUnits =
      (m_settings.units == "Rotations" || m_settings.units == "Degrees" ||
       m_settings.units == "Radians");
  if (m_settings.units == "Degrees") {
    m_settings.unitsPerRotation = 360.0;
  } else if (m_settings.units == "Radians") {
    m_settings.unitsPerRotation = 2 * std::numbers::pi;
  } else if (m_settings.units == "Rotations") {
    m_settings.unitsPerRotation = 1.0;
  }

  // Units Per Rotations entry
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * kTextBoxWidthMultiple);
  ImGui::InputDouble("Units Per Rotation", &m_settings.unitsPerRotation, 0.0f,
                     0.0f, "%.4f",
                     m_isRotationalUnits ? ImGuiInputTextFlags_ReadOnly
                                         : ImGuiInputTextFlags_None);
  sysid::CreateTooltip(
      "The logger assumes that the code will be sending recorded motor shaft "
      "rotations over NetworkTables. This value will then be multiplied by the "
      "units per rotation to get the measurement in the units you "
      "specified.\n\nFor non-rotational units (e.g. meters), this value is "
      "usually the wheel diameter times pi (should not include gearing).");
  // Create a section for voltage parameters.
  ImGui::Separator();
  ImGui::Spacing();
  ImGui::Text("Voltage Parameters");

  auto CreateVoltageParameters = [this](const char* text, double* data,
                                        float min, float max) {
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6);
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled,
                        m_manager && m_manager->IsActive());
    float value = static_cast<float>(*data);
    if (ImGui::SliderFloat(text, &value, min, max, "%.2f")) {
      *data = value;
    }
    ImGui::PopItemFlag();
  };

  CreateVoltageParameters("Quasistatic Ramp Rate (V/s)",
                          &m_settings.quasistaticRampRate, 0.10f, 0.60f);
  sysid::CreateTooltip(
      "This is the rate at which the voltage will increase during the "
      "quasistatic test.");

  CreateVoltageParameters("Dynamic Step Voltage (V)", &m_settings.stepVoltage,
                          0.0f, 10.0f);
  sysid::CreateTooltip(
      "This is the voltage that will be applied for the "
      "dynamic voltage (acceleration) tests.");

  // Create a section for tests.
  ImGui::Separator();
  ImGui::Spacing();
  ImGui::Text("Tests");

  auto CreateTest = [this, width](const char* text, const char* itext) {
    // Display buttons if we have an NT connection.
    if (nt::NetworkTableInstance::GetDefault().IsConnected()) {
      // Create button to run tests.
      if (ImGui::Button(text)) {
        // Open the warning message.
        ImGui::OpenPopup("Warning");
        m_manager->BeginTest(itext);
        m_opened = text;
      }
      if (m_opened == text && ImGui::BeginPopupModal("Warning")) {
        ImGui::TextWrapped("%s", m_popupText.c_str());
        if (ImGui::Button(m_manager->IsActive() ? "End Test" : "Close")) {
          m_manager->EndTest();
          ImGui::CloseCurrentPopup();
          m_opened = "";
        }
        ImGui::EndPopup();
      }
    } else {
      // Show disabled text when there is no connection.
      ImGui::TextDisabled("%s", text);
    }

    // Show whether the tests were run or not.
    bool run = m_manager->HasRunTest(itext);
    ImGui::SameLine(width * 0.7);
    ImGui::Text(run ? "Run" : "Not Run");
  };

  CreateTest("Quasistatic Forward", "slow-forward");
  CreateTest("Quasistatic Backward", "slow-backward");
  CreateTest("Dynamic Forward", "fast-forward");
  CreateTest("Dynamic Backward", "fast-backward");

  m_manager->RegisterDisplayCallback(
      [this](const auto& str) { m_popupText = str; });

  // Display the path to where the JSON will be saved and a button to select the
  // location.
  ImGui::Separator();
  ImGui::Spacing();
  ImGui::Text("Save Location");
  if (ImGui::Button("Choose")) {
    m_selector = std::make_unique<pfd::select_folder>("Select Folder");
  }
  ImGui::SameLine();
  ImGui::InputText("##savelocation", &m_jsonLocation,
                   ImGuiInputTextFlags_ReadOnly);

  // Add button to save.
  ImGui::SameLine(width * 0.9);
  if (ImGui::Button("Save")) {
    try {
      m_manager->SaveJSON(m_jsonLocation);
    } catch (const std::exception& e) {
      ImGui::OpenPopup("Exception Caught!");
      m_exception = e.what();
    }
  }

  // Handle exceptions.
  if (ImGui::BeginPopupModal("Exception Caught!")) {
    ImGui::Text("%s", m_exception.c_str());
    if (ImGui::Button("Close")) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }

  // Run periodic methods.
  SelectDataFolder();
  m_ntSettings.Update();
  m_manager->Update();
}

void Logger::SelectDataFolder() {
  // If the selector exists and is ready with a result, we can store it.
  if (m_selector && m_selector->ready()) {
    m_jsonLocation = m_selector->result();
    m_selector.reset();
  }
}
