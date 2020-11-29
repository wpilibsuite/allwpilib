/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DIOGui.h"

#include <memory>
#include <vector>

#include <hal/Ports.h>
#include <hal/simulation/DIOData.h>
#include <hal/simulation/DigitalPWMData.h>
#include <hal/simulation/DutyCycleData.h>
#include <hal/simulation/EncoderData.h>
#include <hal/simulation/SimDeviceData.h>
#include <imgui.h>

#include "GuiDataSource.h"
#include "HALSimGui.h"
#include "IniSaver.h"
#include "IniSaverInfo.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED(DIOValue, "DIO");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(DigitalPWMDutyCycle, "DPWM");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(DutyCycleOutput, "DutyCycle");

class DigitalIONameAccessor {
 public:
  void GetLabel(char* buf, size_t size, const char* defaultName,
                int index) const {
    const char* displayName = HALSIM_GetDIODisplayName(index);
    if (displayName[0] != '\0') {
      std::snprintf(buf, size, "%s", displayName);
    } else {
      std::snprintf(buf, size, "%s[%d]###Name%d", defaultName, index, index);
    }
  }
};

}  // namespace

static std::vector<DigitalIONameAccessor> gDIO;
static std::vector<std::unique_ptr<DIOValueSource>> gDIOSources;
static std::vector<std::unique_ptr<DigitalPWMDutyCycleSource>> gDPWMSources;
static std::vector<std::unique_ptr<DutyCycleOutputSource>> gDutyCycleSources;

static void LabelSimDevice(const char* name, HAL_SimDeviceHandle simDevice) {
  ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
  ImGui::LabelText(name, "%s", HALSIM_GetSimDeviceName(simDevice));
  ImGui::PopStyleColor();
}

static void UpdateDIOSources() {
  for (int i = 0, iend = gDIOSources.size(); i < iend; ++i) {
    auto& source = gDIOSources[i];
    if (HALSIM_GetDIOInitialized(i)) {
      if (!source) {
        source = std::make_unique<DIOValueSource>(i);
      }
    } else {
      source.reset();
    }
  }
}

static void UpdateDPWMSources() {
  const int numDIO = gDIOSources.size();
  for (int i = 0, iend = gDPWMSources.size(); i < iend; ++i) {
    auto& source = gDPWMSources[i];
    if (HALSIM_GetDigitalPWMInitialized(i)) {
      if (!source) {
        int channel = HALSIM_GetDigitalPWMPin(i);
        if (channel >= 0 && channel < numDIO) {
          source = std::make_unique<DigitalPWMDutyCycleSource>(i, channel);
        }
      }
    } else {
      source.reset();
    }
  }
}

static void UpdateDutyCycleSources() {
  const int numDIO = gDIOSources.size();
  for (int i = 0, iend = gDutyCycleSources.size(); i < iend; ++i) {
    auto& source = gDutyCycleSources[i];
    if (HALSIM_GetDutyCycleInitialized(i)) {
      if (!source) {
        int channel = HALSIM_GetDutyCycleDigitalChannel(i);
        if (channel >= 0 && channel < numDIO) {
          source = std::make_unique<DutyCycleOutputSource>(i, channel);
        }
      }
    } else {
      source.reset();
    }
  }
}

static void DisplayDIO() {
  bool hasAny = false;
  const int numDIO = gDIOSources.size();
  const int numPWM = gDPWMSources.size();
  static const int numEncoder = HAL_GetNumEncoders();
  const int numDutyCycle = gDutyCycleSources.size();
  static auto pwmMap = std::make_unique<int[]>(numDIO);
  static auto encoderMap = std::make_unique<int[]>(numDIO);
  static auto dutyCycleMap = std::make_unique<int[]>(numDIO);

  std::memset(pwmMap.get(), 0, numDIO * sizeof(pwmMap[0]));
  std::memset(encoderMap.get(), 0, numDIO * sizeof(encoderMap[0]));
  std::memset(dutyCycleMap.get(), 0, numDIO * sizeof(dutyCycleMap[0]));

  for (int i = 0; i < numPWM; ++i) {
    if (auto source = gDPWMSources[i].get()) {
      int channel = source->GetChannel();
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
    if (auto source = gDutyCycleSources[i].get()) {
      int channel = source->GetChannel();
      if (channel >= 0 && channel < numDIO) dutyCycleMap[channel] = i + 1;
    }
  }

  ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
  for (int i = 0; i < numDIO; ++i) {
    if (auto dioSource = gDIOSources[i].get()) {
      ImGui::PushID(i);
      hasAny = true;
      DigitalPWMDutyCycleSource* dpwmSource = nullptr;
      DutyCycleOutputSource* dutyCycleSource = nullptr;
      auto& info = gDIO[i];
      char label[128];
      if (pwmMap[i] > 0) {
        dpwmSource = gDPWMSources[pwmMap[i] - 1].get();
        info.GetLabel(label, sizeof(label), "PWM", i);
        if (auto simDevice = HALSIM_GetDIOSimDevice(i)) {
          LabelSimDevice(label, simDevice);
        } else {
          dpwmSource->LabelText(label, "%0.3f", dpwmSource->GetValue());
        }
      } else if (encoderMap[i] > 0) {
        info.GetLabel(label, sizeof(label), " In", i);
        if (auto simDevice = HALSIM_GetEncoderSimDevice(encoderMap[i] - 1)) {
          LabelSimDevice(label, simDevice);
        } else {
          ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
          ImGui::LabelText(label, "Encoder[%d,%d]",
                           HALSIM_GetEncoderDigitalChannelA(encoderMap[i] - 1),
                           HALSIM_GetEncoderDigitalChannelB(encoderMap[i] - 1));
          ImGui::PopStyleColor();
        }
      } else if (dutyCycleMap[i] > 0) {
        dutyCycleSource = gDutyCycleSources[dutyCycleMap[i] - 1].get();
        info.GetLabel(label, sizeof(label), "Dty", i);
        if (auto simDevice =
                HALSIM_GetDutyCycleSimDevice(dutyCycleMap[i] - 1)) {
          LabelSimDevice(label, simDevice);
        } else {
          double val = dutyCycleSource->GetValue();
          if (dutyCycleSource->InputDouble(label, &val))
            HALSIM_SetDutyCycleOutput(dutyCycleMap[i] - 1, val);
        }
      } else if (!HALSIM_GetDIOIsInput(i)) {
        info.GetLabel(label, sizeof(label), "Out", i);
        if (auto simDevice = HALSIM_GetDIOSimDevice(i)) {
          LabelSimDevice(label, simDevice);
        } else {
          dioSource->LabelText(
              label, "%s", dioSource->GetValue() != 0 ? "1 (high)" : "0 (low)");
        }
      } else {
        info.GetLabel(label, sizeof(label), " In", i);
        if (auto simDevice = HALSIM_GetDIOSimDevice(i)) {
          LabelSimDevice(label, simDevice);
        } else {
          static const char* options[] = {"0 (low)", "1 (high)"};
          int val = dioSource->GetValue() != 0 ? 1 : 0;
          if (dioSource->Combo(label, &val, options, 2))
            HALSIM_SetDIOValue(i, val);
        }
      }
      ImGui::PopID();
    }
  }
  ImGui::PopItemWidth();
  if (!hasAny) ImGui::Text("No Digital I/O");
}

void DIOGui::Initialize() {
  gDIO.resize(HAL_GetNumDigitalChannels());
  gDIOSources.resize(HAL_GetNumDigitalChannels());
  gDPWMSources.resize(HAL_GetNumDigitalPWMOutputs());
  gDutyCycleSources.resize(HAL_GetNumDutyCycles());

  HALSimGui::AddExecute(UpdateDIOSources);
  HALSimGui::AddExecute(UpdateDPWMSources);
  HALSimGui::AddExecute(UpdateDutyCycleSources);
  HALSimGui::AddWindow("DIO", DisplayDIO, ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::SetDefaultWindowPos("DIO", 470, 20);
}
