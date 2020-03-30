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

#include <hal/Ports.h>
#include <imgui.h>
#include <mockdata/AddressableLEDData.h>
#include <mockdata/PWMData.h>

#include "HALSimGui.h"
#include "IniSaver.h"
#include "IniSaverInfo.h"

using namespace halsimgui;

static IniSaver<NameInfo> gPWM{"PWM"};

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
  ImGui::PushItemWidth(ImGui::GetFontSize() * 4);
  for (int i = 0; i < numPWM; ++i) {
    if (HALSIM_GetPWMInitialized(i)) {
      hasOutputs = true;

      if (!first)
        ImGui::Separator();
      else
        first = false;

      char name[128];
      auto& info = gPWM[i];
      info.GetName(name, sizeof(name), "PWM", i);
      if (ledMap[i] > 0) {
        ImGui::LabelText(name, "LED[%d]", ledMap[i] - 1);
      } else {
        float val = HALSimGui::AreOutputsDisabled() ? 0 : HALSIM_GetPWMSpeed(i);
        ImGui::LabelText(name, "%0.3f", val);
      }
      info.PopupEditName(i);

      // lazily build history storage
      // if (static_cast<unsigned int>(i) > history.size())
      //  history.resize(i + 1);
      // if (!history[i]) history[i] = std::make_unique<History>();

      // save history

      // ImGui::PlotLines(labels[i].c_str(), values.data(), values.size(),
      // );
    }
  }
  ImGui::PopItemWidth();
  if (!hasOutputs) ImGui::Text("No PWM outputs");
}

void PWMGui::Initialize() {
  gPWM.Initialize();
  HALSimGui::AddWindow("PWM Outputs", DisplayPWMs,
                       ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::SetDefaultWindowPos("PWM Outputs", 910, 20);
}
