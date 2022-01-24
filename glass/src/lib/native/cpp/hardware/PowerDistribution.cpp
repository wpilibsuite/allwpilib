// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "glass/hardware/PowerDistribution.h"

#include <algorithm>
#include <cstdio>

#include <imgui.h>

#include "glass/Context.h"
#include "glass/DataSource.h"
#include "glass/support/NameSetting.h"

using namespace glass;

static float DisplayChannel(PowerDistributionModel& pdp, int channel) {
  float width = 0;
  if (auto currentData = pdp.GetCurrentData(channel)) {
    ImGui::PushID(channel);
    NameSetting leftName{currentData->GetName()};
    char name[64];
    leftName.GetLabel(name, sizeof(name), "", channel);
    double val = currentData->GetValue();
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
    if (currentData->InputDouble(name, &val, 0, 0, "%.3f")) {
      pdp.SetCurrent(channel, val);
    }
    width = ImGui::GetItemRectSize().x;
    leftName.PopupEditName(channel);
    ImGui::PopID();
  }
  return width;
}

void glass::DisplayPowerDistribution(PowerDistributionModel* model, int index) {
  char name[128];
  std::snprintf(name, sizeof(name), "PowerDistribution[%d]", index);
  if (CollapsingHeader(name)) {
    // temperature
    if (auto tempData = model->GetTemperatureData()) {
      double value = tempData->GetValue();
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
      if (tempData->InputDouble("Temp", &value, 0, 0, "%.3f")) {
        model->SetTemperature(value);
      }
    }

    // voltage
    if (auto voltageData = model->GetVoltageData()) {
      double value = voltageData->GetValue();
      ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
      if (voltageData->InputDouble("Voltage", &value, 0, 0, "%.3f")) {
        model->SetVoltage(value);
      }
    }

    // channel currents; show as two columns laid out like PowerDistribution
    const int numChannels = model->GetNumChannels();
    ImGui::Text("Channel Current (A)");
    ImGui::Columns(2, "channels", false);
    float maxWidth = ImGui::GetFontSize() * 13;
    for (int left = 0, right = numChannels - 1; left < right; ++left, --right) {
      float leftWidth = DisplayChannel(*model, left);
      ImGui::NextColumn();

      float rightWidth = DisplayChannel(*model, right);
      ImGui::NextColumn();

      float width =
          (std::max)(leftWidth, rightWidth) * 2 + ImGui::GetFontSize() * 4;
      if (width > maxWidth) {
        maxWidth = width;
      }
    }
    ImGui::Columns(1);
    ImGui::Dummy(ImVec2(maxWidth, 0));
  }
}

void glass::DisplayPowerDistributions(PowerDistributionsModel* model,
                                      std::string_view noneMsg) {
  bool hasAny = false;
  model->ForEachPowerDistribution([&](PowerDistributionModel& pdp, int i) {
    hasAny = true;
    PushID(i);
    DisplayPowerDistribution(&pdp, i);
    PopID();
  });
  if (!hasAny && !noneMsg.empty()) {
    ImGui::TextUnformatted(noneMsg.data(), noneMsg.data() + noneMsg.size());
  }
}
