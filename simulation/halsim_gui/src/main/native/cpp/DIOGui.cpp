/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DIOGui.h"

#include <hal/Ports.h>
#include <imgui.h>
#include <mockdata/DIOData.h>
#include <mockdata/DigitalPWMData.h>
#include <mockdata/DutyCycleData.h>
#include <mockdata/EncoderData.h>
#include <mockdata/SimDeviceData.h>

#include "HALSimGui.h"
#include "IniSaver.h"
#include "IniSaverInfo.h"

using namespace halsimgui;

static IniSaver<NameInfo> gDIO{"DIO"};

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
  static int numDutyCycle = HAL_GetNumDutyCycles();
  static auto pwmMap = std::make_unique<int[]>(numDIO);
  static auto encoderMap = std::make_unique<int[]>(numDIO);
  static auto dutyCycleMap = std::make_unique<int[]>(numDIO);

  std::memset(pwmMap.get(), 0, numDIO * sizeof(pwmMap[0]));
  std::memset(encoderMap.get(), 0, numDIO * sizeof(encoderMap[0]));
  std::memset(dutyCycleMap.get(), 0, numDIO * sizeof(dutyCycleMap[0]));

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

  for (int i = 0; i < numDutyCycle; ++i) {
    if (HALSIM_GetDutyCycleInitialized(i)) {
      int channel = HALSIM_GetDutyCycleDigitalChannel(i);
      if (channel >= 0 && channel < numDIO) dutyCycleMap[channel] = i + 1;
    }
  }

  ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
  for (int i = 0; i < numDIO; ++i) {
    if (HALSIM_GetDIOInitialized(i)) {
      hasAny = true;
      auto& info = gDIO[i];
      char name[128];
      if (pwmMap[i] > 0) {
        info.GetName(name, sizeof(name), "PWM", i);
        if (auto simDevice = HALSIM_GetDIOSimDevice(i)) {
          LabelSimDevice(name, simDevice);
        } else {
          ImGui::LabelText(name, "%0.3f",
                           HALSIM_GetDigitalPWMDutyCycle(pwmMap[i] - 1));
        }
      } else if (encoderMap[i] > 0) {
        info.GetName(name, sizeof(name), " In", i);
        if (auto simDevice = HALSIM_GetEncoderSimDevice(encoderMap[i] - 1)) {
          LabelSimDevice(name, simDevice);
        } else {
          ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
          ImGui::LabelText(name, "Encoder[%d,%d]",
                           HALSIM_GetEncoderDigitalChannelA(encoderMap[i] - 1),
                           HALSIM_GetEncoderDigitalChannelB(encoderMap[i] - 1));
          ImGui::PopStyleColor();
        }
      } else if (dutyCycleMap[i] > 0) {
        info.GetName(name, sizeof(name), "Dty", i);
        if (auto simDevice =
                HALSIM_GetDutyCycleSimDevice(dutyCycleMap[i] - 1)) {
          LabelSimDevice(name, simDevice);
        } else {
          double val = HALSIM_GetDutyCycleOutput(dutyCycleMap[i] - 1);
          if (ImGui::InputDouble(name, &val))
            HALSIM_SetDutyCycleOutput(dutyCycleMap[i] - 1, val);
        }
      } else if (!HALSIM_GetDIOIsInput(i)) {
        info.GetName(name, sizeof(name), "Out", i);
        if (auto simDevice = HALSIM_GetDIOSimDevice(i)) {
          LabelSimDevice(name, simDevice);
        } else {
          ImGui::LabelText(name, "%s",
                           HALSIM_GetDIOValue(i) ? "1 (high)" : "0 (low)");
        }
      } else {
        info.GetName(name, sizeof(name), " In", i);
        if (auto simDevice = HALSIM_GetDIOSimDevice(i)) {
          LabelSimDevice(name, simDevice);
        } else {
          static const char* options[] = {"0 (low)", "1 (high)"};
          int val = HALSIM_GetDIOValue(i) ? 1 : 0;
          if (ImGui::Combo(name, &val, options, 2)) HALSIM_SetDIOValue(i, val);
        }
      }
      info.PopupEditName(i);
    }
  }
  ImGui::PopItemWidth();
  if (!hasAny) ImGui::Text("No Digital I/O");
}

void DIOGui::Initialize() {
  gDIO.Initialize();
  HALSimGui::AddWindow("DIO", DisplayDIO, ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::SetDefaultWindowPos("DIO", 470, 20);
}
