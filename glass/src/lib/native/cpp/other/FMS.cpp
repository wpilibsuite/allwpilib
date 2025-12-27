// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/glass/other/FMS.hpp"

#include <string>

#include <imgui.h>
#include <imgui_stdlib.h>

#include "wpi/glass/DataSource.hpp"
#include "wpi/util/SmallString.hpp"

using namespace wpi::glass;

static const char* stations[] = {"Invalid", "Red 1",  "Red 2", "Red 3",
                                 "Blue 1",  "Blue 2", "Blue 3"};
static const char* robotModes[] = {"Unknown", "Autonomous", "Teleoperated",
                                   "Test"};

void wpi::glass::DisplayFMS(FMSModel* model, bool editableDsAttached) {
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
  if (auto data = model->GetGameSpecificMessageData()) {
    std::string gameSpecificMessage = data->GetValue();
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
    if (ImGui::InputText("Game Specific", &gameSpecificMessage)) {
      model->SetGameSpecificMessage(gameSpecificMessage);
    }
  }
}

void wpi::glass::DisplayFMSReadOnly(FMSModel* model) {
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
  if (auto data = model->GetRobotModeData()) {
    ImGui::Selectable("Robot Mode: ");
    data->EmitDrag();
    ImGui::SameLine();
    ImGui::TextUnformatted(
        exists ? robotModes[static_cast<int>(data->GetValue())] : "?");
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
  if (auto data = model->GetGameSpecificMessageData()) {
    if (exists) {
      wpi::util::SmallString<64> gsmBuf;
      std::string_view gsm = data->GetValue(gsmBuf);
      ImGui::Text("Game Specific: %.*s", static_cast<int>(gsm.size()),
                  gsm.data());
    } else {
      ImGui::TextUnformatted("Game Specific: ?");
    }
  }

  if (!exists) {
    ImGui::PopStyleColor();
  }
}
