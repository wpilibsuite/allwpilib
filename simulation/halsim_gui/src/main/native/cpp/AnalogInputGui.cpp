/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogInputGui.h"

#include <memory>
#include <vector>

#include <hal/Ports.h>
#include <hal/simulation/AnalogGyroData.h>
#include <hal/simulation/AnalogInData.h>
#include <hal/simulation/SimDeviceData.h>
#include <imgui.h>

#include "GuiDataSource.h"
#include "HALSimGui.h"
#include "IniSaver.h"
#include "IniSaverInfo.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(AnalogInVoltage, "AIn");

class AnalogInNameAccessor : public NameInfo {
 public:
  bool GetDisplayName(char* buf, size_t size, const char* defaultName,
                      int index) const {
    const char* displayName = HALSIM_GetAnalogInDisplayName(index);
    if (displayName[0] != '\0') {
      std::snprintf(buf, size, "%s", displayName);
      return true;
    } else {
      GetLabel(buf, size, defaultName, index);
      return false;
    }
  }
};

}  // namespace

// indexed by channel
static IniSaver<AnalogInNameAccessor> gAnalogInputs{"AnalogInput"};
static std::vector<std::unique_ptr<AnalogInVoltageSource>> gAnalogInputSources;

static void UpdateAnalogInputSources() {
  for (int i = 0, iend = gAnalogInputSources.size(); i < iend; ++i) {
    auto& source = gAnalogInputSources[i];
    if (HALSIM_GetAnalogInInitialized(i)) {
      if (!source) {
        source = std::make_unique<AnalogInVoltageSource>(i);
        source->SetName(gAnalogInputs[i].GetName());
      }
    } else {
      source.reset();
    }
  }
}

static void DisplayAnalogInputs() {
  ImGui::Text("(Use Ctrl+Click to edit value)");
  bool hasInputs = false;
  static const int numAccum = HAL_GetNumAccumulators();
  bool first = true;
  for (int i = 0, iend = gAnalogInputSources.size(); i < iend; ++i) {
    if (auto source = gAnalogInputSources[i].get()) {
      ImGui::PushID(i);
      hasInputs = true;

      if (!first) {
        ImGui::Spacing();
        ImGui::Spacing();
      } else {
        first = false;
      }

      auto& info = gAnalogInputs[i];
      // build label
      char label[128];
      bool hasDisplayName = info.GetDisplayName(label, sizeof(label), "In", i);

      if (i < numAccum && HALSIM_GetAnalogGyroInitialized(i)) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
        ImGui::LabelText(label, "AnalogGyro[%d]", i);
        ImGui::PopStyleColor();
      } else if (auto simDevice = HALSIM_GetAnalogInSimDevice(i)) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(96, 96, 96, 255));
        ImGui::LabelText(label, "%s", HALSIM_GetSimDeviceName(simDevice));
        ImGui::PopStyleColor();
      } else {
        float val = source->GetValue();
        if (source->SliderFloat(label, &val, 0.0, 5.0))
          HALSIM_SetAnalogInVoltage(i, val);
      }

      // context menu to change name
      if (!hasDisplayName) {
        if (info.PopupEditName(i)) {
          source->SetName(info.GetName());
        }
      }
      ImGui::PopID();
    }
  }
  if (!hasInputs) ImGui::Text("No analog inputs");
}

void AnalogInputGui::Initialize() {
  gAnalogInputs.Initialize();
  gAnalogInputSources.resize(HAL_GetNumAnalogInputs());

  HALSimGui::AddExecute(UpdateAnalogInputSources);
  HALSimGui::AddWindow("Analog Inputs", DisplayAnalogInputs,
                       ImGuiWindowFlags_AlwaysAutoResize);
  HALSimGui::SetDefaultWindowPos("Analog Inputs", 640, 20);
}
