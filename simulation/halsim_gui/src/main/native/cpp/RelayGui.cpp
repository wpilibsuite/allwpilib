/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "RelayGui.h"

#include <cstdio>
#include <cstring>

#include <hal/Ports.h>
#include <imgui.h>
#include <mockdata/RelayData.h>

#include "ExtraGuiWidgets.h"
#include "HALSimGui.h"
#include "IniSaver.h"
#include "IniSaverInfo.h"

using namespace halsimgui;

static IniSaver<NameInfo> gRelays{"Relay"};

static void DisplayRelays() {
  bool hasOutputs = false;
  bool first = true;
  static const int numRelay = HAL_GetNumRelayHeaders();
  for (int i = 0; i < numRelay; ++i) {
    bool forwardInit = HALSIM_GetRelayInitializedForward(i);
    bool reverseInit = HALSIM_GetRelayInitializedReverse(i);

    if (forwardInit || reverseInit) {
      hasOutputs = true;

      if (!first)
        ImGui::Separator();
      else
        first = false;

      bool forward = false;
      bool reverse = false;
      if (!HALSimGui::AreOutputsDisabled()) {
        reverse = HALSIM_GetRelayReverse(i);
        forward = HALSIM_GetRelayForward(i);
      }

      auto& info = gRelays[i];
      info.PushEditNameId(i);
      if (info.HasName())
        ImGui::Text("%s [%d]", info.GetName(), i);
      else
        ImGui::Text("Relay[%d]", i);
      ImGui::PopID();
      info.PopupEditName(i);
      ImGui::SameLine();

      // show forward and reverse as LED indicators
      static const ImU32 colors[] = {IM_COL32(255, 255, 102, 255),
                                     IM_COL32(255, 0, 0, 255),
                                     IM_COL32(128, 128, 128, 255)};
      int values[2] = {reverseInit ? (reverse ? 2 : -2) : -3,
                       forwardInit ? (forward ? 1 : -1) : -3};
      DrawLEDs(values, 2, 2, colors);
    }
  }
  if (!hasOutputs) ImGui::Text("No relays");
}

void RelayGui::Initialize() {
  gRelays.Initialize();
  HALSimGui::AddWindow("Relays", DisplayRelays,
                       ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::SetDefaultWindowPos("Relays", 180, 20);
}
