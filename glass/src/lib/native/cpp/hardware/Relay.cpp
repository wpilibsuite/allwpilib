// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/hardware/Relay.h"

#include <string>

#include <imgui.h>

#include "glass/Context.h"
#include "glass/DataSource.h"
#include "glass/Storage.h"
#include "glass/support/ExtraGuiWidgets.h"

using namespace glass;

void glass::DisplayRelay(RelayModel* model, int index, bool outputsEnabled) {
  auto forwardData = model->GetForwardData();
  auto reverseData = model->GetReverseData();

  if (!forwardData && !reverseData) {
    return;
  }

  bool forward = false;
  bool reverse = false;
  if (outputsEnabled) {
    if (forwardData) {
      forward = forwardData->GetValue();
    }
    if (reverseData) {
      reverse = reverseData->GetValue();
    }
  }

  std::string& name = GetStorage().GetString("name");
  ImGui::PushID("name");
  if (!name.empty()) {
    ImGui::Text("%s [%d]", name.c_str(), index);
  } else {
    ImGui::Text("Relay[%d]", index);
  }
  ImGui::PopID();
  if (PopupEditName("name", &name)) {
    if (forwardData) {
      forwardData->SetName(name);
    }
    if (reverseData) {
      reverseData->SetName(name);
    }
  }
  ImGui::SameLine();

  // show forward and reverse as LED indicators
  static const ImU32 colors[] = {IM_COL32(255, 255, 102, 255),
                                 IM_COL32(255, 0, 0, 255),
                                 IM_COL32(128, 128, 128, 255)};
  int values[2] = {reverseData ? (reverse ? 2 : -2) : -3,
                   forwardData ? (forward ? 1 : -1) : -3};
  DataSource* sources[2] = {reverseData, forwardData};
  DrawLEDSources(values, sources, 2, 2, colors);
}

void glass::DisplayRelays(RelaysModel* model, bool outputsEnabled,
                          std::string_view noneMsg) {
  bool hasAny = false;
  bool first = true;
  model->ForEachRelay([&](RelayModel& relay, int i) {
    hasAny = true;

    if (!first) {
      ImGui::Separator();
    } else {
      first = false;
    }

    PushID(i);
    DisplayRelay(&relay, i, outputsEnabled);
    PopID();
  });
  if (!hasAny && !noneMsg.empty()) {
    ImGui::TextUnformatted(noneMsg.data(), noneMsg.data() + noneMsg.size());
  }
}
