/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SolenoidGui.h"

#include <cstdio>
#include <cstring>

#include <hal/Ports.h>
#include <imgui.h>
#include <mockdata/PCMData.h>
#include <wpi/SmallVector.h>

#include "ExtraGuiWidgets.h"
#include "HALSimGui.h"
#include "IniSaver.h"
#include "IniSaverInfo.h"

using namespace halsimgui;

static IniSaver<OpenInfo> gPCMs{"PCM"};
static IniSaver<NameInfo> gSolenoids{"Solenoid"};

static void DisplaySolenoids() {
  bool hasOutputs = false;
  static const int numPCM = HAL_GetNumPCMModules();
  static const int numChannels = HAL_GetNumSolenoidChannels();
  for (int i = 0; i < numPCM; ++i) {
    bool anyInit = false;
    wpi::SmallVector<int, 16> channels;
    channels.resize(numChannels);
    for (int j = 0; j < numChannels; ++j) {
      if (HALSIM_GetPCMSolenoidInitialized(i, j)) {
        anyInit = true;
        channels[j] = (!HALSimGui::AreOutputsDisabled() &&
                       HALSIM_GetPCMSolenoidOutput(i, j))
                          ? 1
                          : -1;
      } else {
        channels[j] = -2;
      }
    }

    if (!anyInit) continue;
    hasOutputs = true;

    char name[128];
    std::snprintf(name, sizeof(name), "PCM[%d]", i);
    auto& pcmInfo = gPCMs[i];
    bool open = ImGui::CollapsingHeader(
        name, pcmInfo.IsOpen() ? ImGuiTreeNodeFlags_DefaultOpen : 0);
    pcmInfo.SetOpen(open);
    ImGui::SetItemAllowOverlap();
    ImGui::SameLine();

    // show channels as LED indicators
    static const ImU32 colors[] = {IM_COL32(255, 255, 102, 255),
                                   IM_COL32(128, 128, 128, 255)};
    DrawLEDs(channels.data(), channels.size(), channels.size(), colors);

    if (open) {
      ImGui::PushID(i);
      ImGui::PushItemWidth(ImGui::GetFontSize() * 4);
      for (int j = 0; j < numChannels; ++j) {
        if (channels[j] == -2) continue;
        auto& info = gSolenoids[i * numChannels + j];
        info.GetName(name, sizeof(name), "Solenoid", j);
        ImGui::LabelText(name, "%s", channels[j] == 1 ? "On" : "Off");
        info.PopupEditName(j);
      }
      ImGui::PopItemWidth();
      ImGui::PopID();
    }
  }
  if (!hasOutputs) ImGui::Text("No solenoids");
}

void SolenoidGui::Initialize() {
  gPCMs.Initialize();
  gSolenoids.Initialize();
  HALSimGui::AddWindow("Solenoids", DisplaySolenoids,
                       ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::SetDefaultWindowPos("Solenoids", 290, 20);
}
