/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogInputGui.h"

#include <cstdio>

#include <hal/Ports.h>
#include <imgui.h>
#include <mockdata/AnalogGyroData.h>
#include <mockdata/AnalogInData.h>
#include <mockdata/SimDeviceData.h>

#include "HALSimGui.h"

using namespace halsimgui;

static void DisplayAnalogInputs() {
  ImGui::Text("(Use Ctrl+Click to edit value)");
  bool hasInputs = false;
  static int numAnalog = HAL_GetNumAnalogInputs();
  static int numAccum = HAL_GetNumAccumulators();
  for (int i = 0; i < numAnalog; ++i) {
    if (HALSIM_GetAnalogInInitialized(i)) {
      hasInputs = true;
      char name[32];
      std::snprintf(name, sizeof(name), "In[%d]", i);
      if (i < numAccum && HALSIM_GetAnalogGyroInitialized(i)) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
        ImGui::LabelText(name, "AnalogGyro[%d]", i);
        ImGui::PopStyleColor();
      } else if (auto simDevice = HALSIM_GetAnalogInSimDevice(i)) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
        ImGui::LabelText(name, "%s", HALSIM_GetSimDeviceName(simDevice));
        ImGui::PopStyleColor();
      } else {
        float val = HALSIM_GetAnalogInVoltage(i);
        if (ImGui::SliderFloat(name, &val, 0.0, 5.0))
          HALSIM_SetAnalogInVoltage(i, val);
      }
    }
  }
  if (!hasInputs) ImGui::Text("No analog inputs");
}

void AnalogInputGui::Initialize() {
  HALSimGui::AddWindow("Analog Inputs", DisplayAnalogInputs,
                       ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::SetDefaultWindowPos("Analog Inputs", 640, 20);
}
