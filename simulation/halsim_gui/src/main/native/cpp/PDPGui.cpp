/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "PDPGui.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

#include <hal/Ports.h>
#include <hal/simulation/PDPData.h>
#include <imgui.h>

#include "GuiDataSource.h"
#include "HALSimGui.h"
#include "IniSaver.h"
#include "IniSaverInfo.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(PDPTemperature, "PDP Temp");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(PDPVoltage, "PDP Voltage");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED2(PDPCurrent, "PDP Current");
struct PDPSource {
  explicit PDPSource(int32_t index) : temp{index}, voltage{index} {
    const int numChannels = HAL_GetNumPDPChannels();
    currents.reserve(numChannels);
    for (int i = 0; i < numChannels; ++i)
      currents.emplace_back(std::make_unique<PDPCurrentSource>(index, i));
  }
  PDPTemperatureSource temp;
  PDPVoltageSource voltage;
  std::vector<std::unique_ptr<PDPCurrentSource>> currents;
};

class PDPNameAccessor {
 public:
  void GetLabel(char* buf, size_t size, const char* defaultName,
                int index) const {
    const char* displayName = HALSIM_GetPDPDisplayName(index);
    if (displayName[0] != '\0') {
      std::snprintf(buf, size, "%s", displayName);
    } else {
      std::snprintf(buf, size, "%s[%d]###Name%d", defaultName, index, index);
    }
  }
};

}  // namespace

static std::vector<PDPNameAccessor> gChannels;
static std::vector<std::unique_ptr<PDPSource>> gPDPSources;

static void UpdatePDPSources() {
  for (int i = 0, iend = gPDPSources.size(); i < iend; ++i) {
    auto& source = gPDPSources[i];
    if (HALSIM_GetPDPInitialized(i)) {
      if (!source) {
        source = std::make_unique<PDPSource>(i);
      }
    } else {
      source.reset();
    }
  }
}

static void DisplayPDP() {
  bool hasAny = false;
  for (int i = 0, iend = gPDPSources.size(); i < iend; ++i) {
    if (auto source = gPDPSources[i].get()) {
      hasAny = true;

      char name[128];
      std::snprintf(name, sizeof(name), "PDP[%d]", i);
      if (ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::PushID(i);

        // temperature
        double temp = source->temp.GetValue();
        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
        if (source->temp.InputDouble("Temp", &temp, 0, 0, "%.3f"))
          HALSIM_SetPDPTemperature(i, temp);

        // voltage
        double volts = source->voltage.GetValue();
        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
        if (source->voltage.InputDouble("Voltage", &volts, 0, 0, "%.3f"))
          HALSIM_SetPDPVoltage(i, volts);

        // channel currents; show as two columns laid out like PDP
        const int numChannels = source->currents.size();
        ImGui::Text("Channel Current (A)");
        ImGui::Columns(2, "channels", false);
        float maxWidth = ImGui::GetFontSize() * 13;
        for (int left = 0, right = numChannels - 1; left < right;
             ++left, --right) {
          double val;

          ImGui::PushID(left);
          auto& leftInfo = gChannels[i * numChannels + left];
          leftInfo.GetLabel(name, sizeof(name), "", left);
          val = source->currents[left]->GetValue();
          ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
          if (source->currents[left]->InputDouble(name, &val, 0, 0, "%.3f"))
            HALSIM_SetPDPCurrent(i, left, val);
          float leftWidth = ImGui::GetItemRectSize().x;
          ImGui::PopID();
          ImGui::NextColumn();

          ImGui::PushID(right);
          auto& rightInfo = gChannels[i * numChannels + right];
          rightInfo.GetLabel(name, sizeof(name), "", right);
          val = source->currents[right]->GetValue();
          ImGui::SetNextItemWidth(ImGui::GetFontSize() * 4);
          if (source->currents[right]->InputDouble(name, &val, 0, 0, "%.3f"))
            HALSIM_SetPDPCurrent(i, right, val);
          float rightWidth = ImGui::GetItemRectSize().x;
          ImGui::PopID();
          ImGui::NextColumn();

          float width =
              (std::max)(leftWidth, rightWidth) * 2 + ImGui::GetFontSize() * 4;
          if (width > maxWidth) maxWidth = width;
        }
        ImGui::Columns(1);
        ImGui::Dummy(ImVec2(maxWidth, 0));
        ImGui::PopID();
      }
    }
  }
  if (!hasAny) ImGui::Text("No PDPs");
}

void PDPGui::Initialize() {
  gChannels.resize(HAL_GetNumPDPModules());
  gPDPSources.resize(HAL_GetNumPDPModules());
  HALSimGui::AddExecute(UpdatePDPSources);
  HALSimGui::AddWindow("PDP", DisplayPDP);
  // hide it by default
  HALSimGui::SetWindowVisibility("PDP", HALSimGui::kHide);
  HALSimGui::SetDefaultWindowPos("PDP", 245, 155);
}
