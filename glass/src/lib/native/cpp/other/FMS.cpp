// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/other/FMS.h"

#include <string>

#include <imgui.h>
#include <imgui_stdlib.h>
#include <wpi/SmallString.h>

#include "glass/DataSource.h"

using namespace glass;

static const char* stations[] = {"Invalid", "Red 1",  "Red 2", "Red 3",
                                 "Blue 1",  "Blue 2", "Blue 3"};

void glass::DisplayFMS(FMSModel* model, bool editableDsAttached) {
  if (!model->Exists() || model->IsReadOnly()) {
    return DisplayFMSReadOnly(model);
  }

  // FMS Attached
  if (auto data = model->GetFmsAttachedData()) {
    bool val = data->GetValue();
    if (ImGui::Checkbox("FMS Attached", &val)) {
      model->SetFmsAttached(val);
    }
    data->EmitDrag();
  }

  // DS Attached
  if (auto data = model->GetDsAttachedData()) {
    bool val = data->GetValue();
    if (editableDsAttached) {
      if (ImGui::Checkbox("DS Attached", &val)) {
        model->SetDsAttached(val);
      }
      data->EmitDrag();
    } else {
      ImGui::Selectable("DS Attached: ");
      data->EmitDrag();
      ImGui::SameLine();
      ImGui::TextUnformatted(val ? "Yes" : "No");
    }
  }

  // Alliance Station
  if (auto data = model->GetAllianceStationIdData()) {
    int val = data->GetValue();
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
    if (ImGui::Combo("Alliance Station", &val, stations, 7)) {
      model->SetAllianceStationId(val);
    }
    data->EmitDrag();
  }

  // Match Time
  if (auto data = model->GetMatchTimeData()) {
    double val = data->GetValue();
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
    if (ImGui::InputDouble("Match Time", &val, 0, 0, "%.1f")) {
      model->SetMatchTime(val);
    }
    data->EmitDrag();
    bool enabled = false;
    if (auto enabledData = model->GetEnabledData()) {
      enabled = enabledData->GetValue();
    }
    ImGui::SameLine();
    if (ImGui::Button("Auto") && !enabled) {
      model->SetMatchTime(15.0);
    }
    ImGui::SameLine();
    if (ImGui::Button("Teleop") && !enabled) {
      model->SetMatchTime(135.0);
    }
  }

  // Game Specific Message
  wpi::SmallString<64> gameSpecificMessageBuf;
  std::string gameSpecificMessage{
      model->GetGameSpecificMessage(gameSpecificMessageBuf)};
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
  if (ImGui::InputText("Game Specific", &gameSpecificMessage)) {
    model->SetGameSpecificMessage(gameSpecificMessage);
  }
}

void glass::DisplayFMSReadOnly(FMSModel* model) {
  bool exists = model->Exists();
  if (!exists) {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
  }

  if (auto data = model->GetEStopData()) {
    ImGui::Selectable("E-Stopped: ");
    data->EmitDrag();
    ImGui::SameLine();
    ImGui::TextUnformatted(exists ? (data->GetValue() ? "Yes" : "No") : "?");
  }
  if (auto data = model->GetEnabledData()) {
    ImGui::Selectable("Robot Enabled: ");
    data->EmitDrag();
    ImGui::SameLine();
    ImGui::TextUnformatted(exists ? (data->GetValue() ? "Yes" : "No") : "?");
  }
  if (auto data = model->GetTestData()) {
    ImGui::Selectable("Test Mode: ");
    data->EmitDrag();
    ImGui::SameLine();
    ImGui::TextUnformatted(exists ? (data->GetValue() ? "Yes" : "No") : "?");
  }
  if (auto data = model->GetAutonomousData()) {
    ImGui::Selectable("Autonomous Mode: ");
    data->EmitDrag();
    ImGui::SameLine();
    ImGui::TextUnformatted(exists ? (data->GetValue() ? "Yes" : "No") : "?");
  }
  if (auto data = model->GetFmsAttachedData()) {
    ImGui::Selectable("FMS Attached: ");
    data->EmitDrag();
    ImGui::SameLine();
    ImGui::TextUnformatted(exists ? (data->GetValue() ? "Yes" : "No") : "?");
  }
  if (auto data = model->GetDsAttachedData()) {
    ImGui::Selectable("DS Attached: ");
    data->EmitDrag();
    ImGui::SameLine();
    ImGui::TextUnformatted(exists ? (data->GetValue() ? "Yes" : "No") : "?");
  }
  if (auto data = model->GetAllianceStationIdData()) {
    ImGui::Selectable("Alliance Station: ");
    data->EmitDrag();
    ImGui::SameLine();
    ImGui::TextUnformatted(exists ? stations[static_cast<int>(data->GetValue())]
                                  : "?");
  }
  if (auto data = model->GetMatchTimeData()) {
    ImGui::Selectable("Match Time: ");
    data->EmitDrag();
    ImGui::SameLine();
    if (exists) {
      ImGui::Text("%.1f", data->GetValue());
    } else {
      ImGui::TextUnformatted("?");
    }
  }

  wpi::SmallString<64> gameSpecificMessageBuf;
  std::string_view gameSpecificMessage =
      model->GetGameSpecificMessage(gameSpecificMessageBuf);
  ImGui::Text("Game Specific: %s", exists ? gameSpecificMessage.data() : "?");

  if (!exists) {
    ImGui::PopStyleColor();
  }
}
