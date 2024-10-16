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

  // TODO: Icons? Colors?

  // show higher severity alerts on top
  for (auto&& error : errors) {
    ImGui::Text("E: %s", error.c_str());
  }
  for (auto&& warning : warnings) {
    ImGui::Text("W: %s", warning.c_str());
  }
  for (auto&& info : infos) {
    ImGui::Text("I: %s", info.c_str());
  }
}
