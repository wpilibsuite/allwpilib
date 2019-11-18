/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PWMGui.h"

#include <cstdio>
#include <cstring>
#include <memory>

#include <hal/Ports.h>
#include <imgui.h>
#include <mockdata/AddressableLEDData.h>
#include <mockdata/PWMData.h>

#include "HALSimGui.h"

using namespace halsimgui;

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

  // struct History {
  //  History() { std::memset(data, 0, 90 * sizeof(float)); }
  //  History(const History&) = delete;
  //  History& operator=(const History&) = delete;
  //  float data[90];
  //  int display_offset = 0;
  //  int save_offset = 0;
  //};
  // static std::vector<std::unique_ptr<History>> history;
  bool first = true;
  for (int i = 0; i < numPWM; ++i) {
    if (HALSIM_GetPWMInitialized(i)) {
      hasOutputs = true;

      if (!first)
        ImGui::Separator();
      else
        first = false;

      char name[32];
      std::snprintf(name, sizeof(name), "PWM[%d]", i);
      if (ledMap[i] > 0) {
        ImGui::Text("%s: LED[%d]", name, ledMap[i] - 1);
      } else {
        float val = HALSimGui::AreOutputsDisabled() ? 0 : HALSIM_GetPWMSpeed(i);
        ImGui::Value(name, val, "%0.3f");
      }

      // lazily build history storage
      // if (static_cast<unsigned int>(i) > history.size())
      //  history.resize(i + 1);
      // if (!history[i]) history[i] = std::make_unique<History>();

      // save history

      // ImGui::PlotLines(labels[i].c_str(), values.data(), values.size(),
      // );
    }
  }
  if (!hasOutputs) ImGui::Text("No PWM outputs");
}

void PWMGui::Initialize() {
  HALSimGui::AddWindow("PWM Outputs", DisplayPWMs,
                       ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::SetDefaultWindowPos("PWM Outputs", 910, 20);
}
