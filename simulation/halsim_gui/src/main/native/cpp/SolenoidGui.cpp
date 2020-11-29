/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SolenoidGui.h"

#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

#include <hal/Ports.h>
#include <hal/simulation/PCMData.h>
#include <imgui.h>
#include <wpi/SmallVector.h>

#include "ExtraGuiWidgets.h"
#include "GuiDataSource.h"
#include "HALSimGui.h"
#include "IniSaver.h"
#include "IniSaverInfo.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED2(PCMSolenoidOutput, "Solenoid");
struct PCMSource {
  explicit PCMSource(int numChannels) : solenoids(numChannels) {}
  std::vector<std::unique_ptr<PCMSolenoidOutputSource>> solenoids;
  int initCount = 0;
};

class SolenoidNameAccessor {
 public:
  void GetLabel(char* buf, size_t size, int pcmIndex, int solenoidIndex) const {
    const char* displayName =
        HALSIM_GetSolenoidDisplayName(pcmIndex, solenoidIndex);
    if (displayName[0] != '\0') {
      std::snprintf(buf, size, "%s", displayName);
    } else {
      std::snprintf(buf, size, "%s[%d]###Name%d", "Solenoid", solenoidIndex,
                    solenoidIndex);
    }
  }
};

}  // namespace

static IniSaver<OpenInfo> gPCMs{"PCM"};
static std::vector<SolenoidNameAccessor> gSolenoids;
static std::vector<PCMSource> gPCMSources;

static void UpdateSolenoidSources() {
  for (int i = 0, iend = gPCMSources.size(); i < iend; ++i) {
    auto& pcmSource = gPCMSources[i];
    int numChannels = pcmSource.solenoids.size();
    pcmSource.initCount = 0;
    for (int j = 0; j < numChannels; ++j) {
      auto& source = pcmSource.solenoids[j];
      if (HALSIM_GetPCMSolenoidInitialized(i, j)) {
        if (!source) {
          source = std::make_unique<PCMSolenoidOutputSource>(i, j);
        }
        ++pcmSource.initCount;
      } else {
        source.reset();
      }
    }
  }
}

static void DisplaySolenoids() {
  bool hasOutputs = false;
  for (int i = 0, iend = gPCMSources.size(); i < iend; ++i) {
    auto& pcmSource = gPCMSources[i];
    if (pcmSource.initCount == 0) continue;
    hasOutputs = true;

    int numChannels = pcmSource.solenoids.size();
    wpi::SmallVector<int, 16> channels;
    channels.resize(numChannels);
    for (int j = 0; j < numChannels; ++j) {
      if (pcmSource.solenoids[j]) {
        channels[j] = (!HALSimGui::AreOutputsDisabled() &&
                       pcmSource.solenoids[j]->GetValue())
                          ? 1
                          : -1;
      } else {
        channels[j] = -2;
      }
    }

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
        if (!pcmSource.solenoids[j]) continue;
        auto& info = gSolenoids[i * numChannels + j];
        info.GetLabel(name, sizeof(name), i, j);
        ImGui::PushID(j);
        pcmSource.solenoids[j]->LabelText(name, "%s",
                                          channels[j] == 1 ? "On" : "Off");
        ImGui::PopID();
      }
      ImGui::PopItemWidth();
      ImGui::PopID();
    }
  }
  if (!hasOutputs) ImGui::Text("No solenoids");
}

void SolenoidGui::Initialize() {
  gPCMs.Initialize();
  const int numModules = HAL_GetNumPCMModules();
  const int numChannels = HAL_GetNumSolenoidChannels();
  gPCMSources.reserve(numModules);
  gSolenoids.resize(numModules * numChannels);
  for (int i = 0; i < numModules; ++i) gPCMSources.emplace_back(numChannels);

  HALSimGui::AddExecute(UpdateSolenoidSources);
  HALSimGui::AddWindow("Solenoids", DisplaySolenoids,
                       ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::SetDefaultWindowPos("Solenoids", 290, 20);
}
