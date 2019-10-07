/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DIOGui.h"

#include <cstdio>
#include <cstring>
#include <memory>

#include <hal/Ports.h>
#include <imgui.h>
#include <mockdata/DIOData.h>
#include <mockdata/DigitalPWMData.h>
#include <mockdata/EncoderData.h>
#include <mockdata/SimDeviceData.h>

#include "HALSimGui.h"

using namespace halsimgui;

static void LabelSimDevice(const char* name, HAL_SimDeviceHandle simDevice) {
  ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
  ImGui::LabelText(name, "%s", HALSIM_GetSimDeviceName(simDevice));
  ImGui::PopStyleColor();
}

static void DisplayDIO() {
  bool hasAny = false;
  static int numDIO = HAL_GetNumDigitalChannels();
  static int numPWM = HAL_GetNumDigitalPWMOutputs();
  static int numEncoder = HAL_GetNumEncoders();
  static auto pwmMap = std::make_unique<int[]>(numDIO);
  static auto encoderMap = std::make_unique<int[]>(numDIO);

  std::memset(pwmMap.get(), 0, numDIO * sizeof(pwmMap[0]));
  std::memset(encoderMap.get(), 0, numDIO * sizeof(encoderMap[0]));

  for (int i = 0; i < numPWM; ++i) {
    if (HALSIM_GetDigitalPWMInitialized(i)) {
      int channel = HALSIM_GetDigitalPWMPin(i);
      if (channel >= 0 && channel < numDIO) pwmMap[channel] = i + 1;
    }
  }

  for (int i = 0; i < numEncoder; ++i) {
    if (HALSIM_GetEncoderInitialized(i)) {
      int channel;
      channel = HALSIM_GetEncoderDigitalChannelA(i);
      if (channel >= 0 && channel < numDIO) encoderMap[channel] = i + 1;
      channel = HALSIM_GetEncoderDigitalChannelB(i);
      if (channel >= 0 && channel < numDIO) encoderMap[channel] = i + 1;
    }
  }

  ImGui::PushItemWidth(100);
  for (int i = 0; i < numDIO; ++i) {
    if (HALSIM_GetDIOInitialized(i)) {
      hasAny = true;
      char name[32];
      if (pwmMap[i] > 0) {
        std::snprintf(name, sizeof(name), "PWM[%d]", i);
        if (auto simDevice = HALSIM_GetDIOSimDevice(i)) {
          LabelSimDevice(name, simDevice);
        } else {
          ImGui::LabelText(name, "%0.3f",
                           HALSIM_GetDigitalPWMDutyCycle(pwmMap[i] - 1));
        }
      } else if (encoderMap[i] > 0) {
        std::snprintf(name, sizeof(name), " In[%d]", i);
        if (auto simDevice = HALSIM_GetEncoderSimDevice(encoderMap[i] - 1)) {
          LabelSimDevice(name, simDevice);
        } else {
          ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
          ImGui::LabelText(name, "Encoder[%d,%d]",
                           HALSIM_GetEncoderDigitalChannelA(encoderMap[i] - 1),
                           HALSIM_GetEncoderDigitalChannelB(encoderMap[i] - 1));
          ImGui::PopStyleColor();
        }
      } else if (!HALSIM_GetDIOIsInput(i)) {
        std::snprintf(name, sizeof(name), "Out[%d]", i);
        if (auto simDevice = HALSIM_GetDIOSimDevice(i)) {
          LabelSimDevice(name, simDevice);
        } else {
          ImGui::LabelText(name, "%s",
                           HALSIM_GetDIOValue(i) ? "1 (high)" : "0 (low)");
        }
      } else {
        std::snprintf(name, sizeof(name), " In[%d]", i);
        if (auto simDevice = HALSIM_GetDIOSimDevice(i)) {
          LabelSimDevice(name, simDevice);
        } else {
          static const char* options[] = {"0 (low)", "1 (high)"};
          int val = HALSIM_GetDIOValue(i) ? 1 : 0;
          if (ImGui::Combo(name, &val, options, 2)) HALSIM_SetDIOValue(i, val);
        }
      }
    }
  }
  ImGui::PopItemWidth();
  if (!hasAny) ImGui::Text("No Digital I/O");
}

void DIOGui::Initialize() {
  HALSimGui::AddWindow("DIO", DisplayDIO, ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::SetDefaultWindowPos("DIO", 470, 20);
}
