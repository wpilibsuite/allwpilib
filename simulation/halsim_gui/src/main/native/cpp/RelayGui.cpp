/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "RelayGui.h"

#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

#include <hal/Ports.h>
#include <hal/simulation/RelayData.h>
#include <imgui.h>

#include "ExtraGuiWidgets.h"
#include "GuiDataSource.h"
#include "HALSimGui.h"
#include "IniSaver.h"
#include "IniSaverInfo.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED(RelayForward, "RelayFwd");
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED(RelayReverse, "RelayRev");

class RelayNameAccessor {
 public:
  void GetLabel(char* buf, size_t size, const char* defaultName,
                int index) const {
    const char* displayName = HALSIM_GetRelayDisplayName(index);
    if (displayName[0] != '\0') {
      std::snprintf(buf, size, "%s", displayName);
    } else {
      std::snprintf(buf, size, "%s[%d]###Name%d", defaultName, index, index);
    }
  }
};
}  // namespace

static std::vector<RelayNameAccessor> gRelays;
static std::vector<std::unique_ptr<RelayForwardSource>> gRelayForwardSources;
static std::vector<std::unique_ptr<RelayReverseSource>> gRelayReverseSources;

static void UpdateRelaySources() {
  for (int i = 0, iend = gRelayForwardSources.size(); i < iend; ++i) {
    auto& source = gRelayForwardSources[i];
    if (HALSIM_GetRelayInitializedForward(i)) {
      if (!source) {
        source = std::make_unique<RelayForwardSource>(i);
      }
    } else {
      source.reset();
    }
  }
  for (int i = 0, iend = gRelayReverseSources.size(); i < iend; ++i) {
    auto& source = gRelayReverseSources[i];
    if (HALSIM_GetRelayInitializedReverse(i)) {
      if (!source) {
        source = std::make_unique<RelayReverseSource>(i);
      }
    } else {
      source.reset();
    }
  }
}

static void DisplayRelays() {
  bool hasOutputs = false;
  bool first = true;
  for (int i = 0, iend = gRelayForwardSources.size(); i < iend; ++i) {
    auto forwardSource = gRelayForwardSources[i].get();
    auto reverseSource = gRelayReverseSources[i].get();

    if (forwardSource || reverseSource) {
      hasOutputs = true;

      if (!first)
        ImGui::Separator();
      else
        first = false;

      bool forward = false;
      bool reverse = false;
      if (!HALSimGui::AreOutputsDisabled()) {
        if (forwardSource) forward = forwardSource->GetValue();
        if (reverseSource) reverse = reverseSource->GetValue();
      }

      auto& info = gRelays[i];

      char label[128];
      info.GetLabel(label, sizeof(label), "Relay", i);
      ImGui::Text("%s", label);
      ImGui::SameLine();

      // show forward and reverse as LED indicators
      static const ImU32 colors[] = {IM_COL32(255, 255, 102, 255),
                                     IM_COL32(255, 0, 0, 255),
                                     IM_COL32(128, 128, 128, 255)};
      int values[2] = {reverseSource ? (reverse ? 2 : -2) : -3,
                       forwardSource ? (forward ? 1 : -1) : -3};
      GuiDataSource* sources[2] = {reverseSource, forwardSource};
      ImGui::PushID(i);
      DrawLEDSources(values, sources, 2, 2, colors);
      ImGui::PopID();
    }
  }
  if (!hasOutputs) ImGui::Text("No relays");
}

void RelayGui::Initialize() {
  int numRelays = HAL_GetNumRelayHeaders();
  gRelays.resize(numRelays);
  gRelayForwardSources.resize(numRelays);
  gRelayReverseSources.resize(numRelays);
  HALSimGui::AddExecute(UpdateRelaySources);
  HALSimGui::AddWindow("Relays", DisplayRelays,
                       ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::SetDefaultWindowPos("Relays", 180, 20);
}
