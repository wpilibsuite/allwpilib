// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/other/Alerts.h"

#include <IconsFontAwesome6.h>
#include <imgui.h>

using namespace glass;

void glass::DisplayAlerts(AlertsModel* model) {
  auto& infos = model->GetInfos();
  auto& warnings = model->GetWarnings();
  auto& errors = model->GetErrors();

  const ImVec4 kWarningColor{0.85f, 0.56f, 0.0f, 1.0f};
  const ImVec4 kErrorColor{0.9f, 0.25f, 0.25f, 1.0f};

  // show higher severity alerts on top
  for (auto&& error : errors) {
    ImGui::TextColored(kErrorColor, "%s %s", ICON_FA_CIRCLE_XMARK,
                       error.c_str());
  }
  for (auto&& warning : warnings) {
    ImGui::TextColored(kWarningColor, "%s %s", ICON_FA_TRIANGLE_EXCLAMATION,
                       warning.c_str());
  }
  for (auto&& info : infos) {
    ImGui::Text("%s %s", ICON_FA_CIRCLE_INFO, info.c_str());
  }
}
