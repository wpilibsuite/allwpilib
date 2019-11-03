/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PDPGui.h"

#include <cstdio>
#include <memory>

#include <hal/Ports.h>
#include <imgui.h>
#include <mockdata/PDPData.h>

#include "HALSimGui.h"

using namespace halsimgui;

static void DisplayPDP() {
  bool hasAny = false;
  static int numPDP = HAL_GetNumPDPModules();
  static int numChannels = HAL_GetNumPDPChannels();
  static auto channelCurrents = std::make_unique<double[]>(numChannels);
  ImGui::PushItemWidth(ImGui::GetFontSize() * 13);
  for (int i = 0; i < numPDP; ++i) {
    if (HALSIM_GetPDPInitialized(i)) {
      hasAny = true;

      char name[32];
      std::snprintf(name, sizeof(name), "PDP[%d]", i);
      if (ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PushID(i);

        // temperature
        double temp = HALSIM_GetPDPTemperature(i);
        if (ImGui::InputDouble("Temp", &temp))
          HALSIM_SetPDPTemperature(i, temp);

        // voltage
        double volts = HALSIM_GetPDPVoltage(i);
        if (ImGui::InputDouble("Voltage", &volts))
          HALSIM_SetPDPVoltage(i, volts);

        // channel currents; show as two columns laid out like PDP
        HALSIM_GetPDPAllCurrents(i, channelCurrents.get());
        ImGui::Text("Channel Current (A)");
        ImGui::Columns(2, "channels", false);
        for (int left = 0, right = numChannels - 1; left < right;
             ++left, --right) {
          double val;

          std::snprintf(name, sizeof(name), "[%d]", left);
          val = channelCurrents[left];
          if (ImGui::InputDouble(name, &val))
            HALSIM_SetPDPCurrent(i, left, val);
          ImGui::NextColumn();

          std::snprintf(name, sizeof(name), "[%d]", right);
          val = channelCurrents[right];
          if (ImGui::InputDouble(name, &val))
            HALSIM_SetPDPCurrent(i, right, val);
          ImGui::NextColumn();
        }
        ImGui::Columns(1);
        ImGui::PopID();
      }
    }
  }
  ImGui::PopItemWidth();
  if (!hasAny) ImGui::Text("No PDPs");
}

void PDPGui::Initialize() {
  HALSimGui::AddWindow("PDP", DisplayPDP, ImGuiWindowFlags_AlwaysAutoResize);
  // hide it by default
  HALSimGui::SetWindowVisibility("PDP", HALSimGui::kHide);
  HALSimGui::SetDefaultWindowPos("PDP", 245, 155);
}
