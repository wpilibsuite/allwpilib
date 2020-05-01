/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PDPGui.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <memory>

#include <hal/Ports.h>
#include <imgui.h>
#include <mockdata/PDPData.h>

#include "HALSimGui.h"
#include "IniSaver.h"
#include "IniSaverInfo.h"

using namespace halsimgui;

static IniSaver<NameInfo> gChannels{"PDP"};

static void DisplayPDP() {
  bool hasAny = false;
  static int numPDP = HAL_GetNumPDPModules();
  static int numChannels = HAL_GetNumPDPChannels();
  static auto channelCurrents = std::make_unique<double[]>(numChannels);
  for (int i = 0; i < numPDP; ++i) {
    if (HALSIM_GetPDPInitialized(i)) {
      hasAny = true;

      char name[128];
      std::snprintf(name, sizeof(name), "PDP[%d]", i);
      if (ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PushID(i);

        // temperature
        double temp = HALSIM_GetPDPTemperature(i);
        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
        if (ImGui::InputDouble("Temp", &temp, 0, 0, "%.3f"))
          HALSIM_SetPDPTemperature(i, temp);

        // voltage
        double volts = HALSIM_GetPDPVoltage(i);
        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
        if (ImGui::InputDouble("Voltage", &volts, 0, 0, "%.3f"))
          HALSIM_SetPDPVoltage(i, volts);

        // channel currents; show as two columns laid out like PDP
        HALSIM_GetPDPAllCurrents(i, channelCurrents.get());
        ImGui::Text("Channel Current (A)");
        ImGui::Columns(2, "channels", false);
        float maxWidth = ImGui::GetFontSize() * 13;
        for (int left = 0, right = numChannels - 1; left < right;
             ++left, --right) {
          double val;

          auto& leftInfo = gChannels[i * numChannels + left];
          leftInfo.GetName(name, sizeof(name), "", left);
          val = channelCurrents[left];
          ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
          if (ImGui::InputDouble(name, &val, 0, 0, "%.3f"))
            HALSIM_SetPDPCurrent(i, left, val);
          float leftWidth = ImGui::GetItemRectSize().x;
          leftInfo.PopupEditName(left);
          ImGui::NextColumn();

          auto& rightInfo = gChannels[i * numChannels + right];
          rightInfo.GetName(name, sizeof(name), "", right);
          val = channelCurrents[right];
          ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
          if (ImGui::InputDouble(name, &val, 0, 0, "%.3f"))
            HALSIM_SetPDPCurrent(i, right, val);
          float rightWidth = ImGui::GetItemRectSize().x;
          rightInfo.PopupEditName(right);
          ImGui::NextColumn();

          float width = (std::max)(leftWidth, rightWidth) * 2;
          if (width > maxWidth) maxWidth = width;
        }
        ImGui::Columns(1);
        ImGui::Dummy(ImVec2(maxWidth, 0));
        ImGui::PopID();
      }
    }
  }
  if (!hasAny) ImGui::Text("No PDPs");
}

void PDPGui::Initialize() {
  gChannels.Initialize();
  HALSimGui::AddWindow("PDP", DisplayPDP, ImGuiWindowFlags_AlwaysAutoResize);
  // hide it by default
  HALSimGui::SetWindowVisibility("PDP", HALSimGui::kHide);
  HALSimGui::SetDefaultWindowPos("PDP", 245, 155);
}
