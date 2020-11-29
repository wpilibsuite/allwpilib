/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PWMGui.h"

#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

#include <hal/Ports.h>
#include <hal/simulation/AddressableLEDData.h>
#include <hal/simulation/PWMData.h>
#include <imgui.h>

#include "GuiDataSource.h"
#include "HALSimGui.h"
#include "IniSaver.h"
#include "IniSaverInfo.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(PWMSpeed, "PWM");

class PwmNameAccessor {
 public:
  void GetLabel(char* buf, size_t size, const char* defaultName,
                int index) const {
    const char* displayName = HALSIM_GetPWMDisplayName(index);
    if (displayName[0] != '\0') {
      std::snprintf(buf, size, "%s", displayName);
    } else {
      std::snprintf(buf, size, "%s[%d]###Name%d", defaultName, index, index);
    }
  }
};
}  // namespace

static std::vector<PwmNameAccessor> gPWM;
static std::vector<std::unique_ptr<PWMSpeedSource>> gPWMSources;

static void UpdatePWMSources() {
  static const int numPWM = HAL_GetNumPWMChannels();
  if (static_cast<size_t>(numPWM) != gPWMSources.size())
    gPWMSources.resize(numPWM);

  for (int i = 0; i < numPWM; ++i) {
    auto& source = gPWMSources[i];
    if (HALSIM_GetPWMInitialized(i)) {
      if (!source) {
        source = std::make_unique<PWMSpeedSource>(i);
      }
    } else {
      source.reset();
    }
  }
}

static void DisplayPWMs() {
  bool hasOutputs = false;
  static const int numPWM = HAL_GetNumPWMChannels();
  static const int numLED = HAL_GetNumAddressableLEDs();
  static auto ledMap = std::make_unique<int[]>(numPWM);

  std::memset(ledMap.get(), 0, numPWM * sizeof(ledMap[0]));

  for (int i = 0; i < numLED; ++i) {
    if (HALSIM_GetAddressableLEDInitialized(i)) {
      int channel = HALSIM_GetAddressableLEDOutputPort(i);
      if (channel >= 0 && channel < numPWM) ledMap[channel] = i + 1;
    }
  }

  bool first = true;
  ImGui::PushItemWidth(ImGui::GetFontSize() * 4);
  for (int i = 0; i < numPWM; ++i) {
    if (auto source = gPWMSources[i].get()) {
      ImGui::PushID(i);
      hasOutputs = true;

      if (!first)
        ImGui::Separator();
      else
        first = false;

      auto& info = gPWM[i];
      char label[128];
      info.GetLabel(label, sizeof(label), "PWM", i);
      if (ledMap[i] > 0) {
        ImGui::LabelText(label, "LED[%d]", ledMap[i] - 1);
      } else {
        float val = HALSimGui::AreOutputsDisabled() ? 0 : HALSIM_GetPWMSpeed(i);
        source->LabelText(label, "%0.3f", val);
      }
      ImGui::PopID();
    }
  }
  ImGui::PopItemWidth();
  if (!hasOutputs) ImGui::Text("No PWM outputs");
}

void PWMGui::Initialize() {
  gPWM.resize(HAL_GetNumPWMChannels());
  HALSimGui::AddExecute(UpdatePWMSources);
  HALSimGui::AddWindow("PWM Outputs", DisplayPWMs,
                       ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::SetDefaultWindowPos("PWM Outputs", 910, 20);
}
