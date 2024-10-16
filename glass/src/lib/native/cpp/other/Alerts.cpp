// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/other/Alerts.h"

#include <imgui.h>

using namespace glass;

void glass::DisplayAlerts(AlertsModel* model) {
  auto& infos = model->GetInfos();
  auto& warnings = model->GetWarnings();
  auto& errors = model->GetErrors();

  const ImVec4 kInfoColor{1.0, 1.0, 1.0, 1.0};
  const ImVec4 kWarningColor{1.0, 0.66, 0.0, 1.0};
  const ImVec4 kErrorColor{1.0, 0.33, 0.33, 1.0};

  // show higher severity alerts on top

  for (auto&& error : errors) {
    ImGui::TextColored(kErrorColor, "Error: %s", error.c_str());
  }
  for (auto&& warning : warnings) {
    ImGui::TextColored(kWarningColor, "Warning: %s", warning.c_str());
  }
  for (auto&& info : infos) {
    ImGui::TextColored(kInfoColor, "Info: %s", info.c_str());
  }
}
