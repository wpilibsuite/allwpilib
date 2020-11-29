/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogOutGui.h"

#include <memory>
#include <vector>

#include <hal/Ports.h>
#include <hal/simulation/AnalogOutData.h>
#include <imgui.h>

#include "GuiDataSource.h"
#include "HALSimGui.h"
#include "IniSaver.h"
#include "IniSaverInfo.h"
#include "SimDeviceGui.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(AnalogOutVoltage, "AOut");

class AnalogOutNameAccessor {
 public:
  void GetLabel(char* buf, size_t size, const char* defaultName,
                int index) const {
    const char* displayName = HALSIM_GetAnalogOutDisplayName(index);
    if (displayName[0] != '\0') {
      std::snprintf(buf, size, "%s", displayName);
    } else {
      std::snprintf(buf, size, "%s[%d]###Name%d", defaultName, index, index);
    }
  }
};
}  // namespace

static std::vector<AnalogOutNameAccessor> gAnalogOuts;  // indexed by channel
static std::vector<std::unique_ptr<AnalogOutVoltageSource>> gAnalogOutSources;

static void UpdateAnalogOutSources() {
  for (int i = 0, iend = gAnalogOutSources.size(); i < iend; ++i) {
    auto& source = gAnalogOutSources[i];
    if (HALSIM_GetAnalogOutInitialized(i)) {
      if (!source) {
        source = std::make_unique<AnalogOutVoltageSource>(i);
      }
    } else {
      source.reset();
    }
  }
}

static void DisplayAnalogOutputs() {
  int count = 0;
  for (auto&& source : gAnalogOutSources) {
    if (source) ++count;
  }

  if (count == 0) return;

  if (SimDeviceGui::StartDevice("Analog Outputs", "Analog Outputs")) {
    for (int i = 0, iend = gAnalogOutSources.size(); i < iend; ++i) {
      if (auto source = gAnalogOutSources[i].get()) {
        ImGui::PushID(i);

        auto& info = gAnalogOuts[i];
        char label[128];
        info.GetLabel(label, sizeof(label), "Out", i);
        HAL_Value value = HAL_MakeDouble(source->GetValue());
        SimDeviceGui::DisplayValueSource(label, true, &value, source);

        ImGui::PopID();
      }
    }

    SimDeviceGui::FinishDevice();
  }
}

void AnalogOutGui::Initialize() {
  gAnalogOuts.resize(HAL_GetNumAnalogOutputs());
  gAnalogOutSources.resize(HAL_GetNumAnalogOutputs());
  HALSimGui::AddExecute(UpdateAnalogOutSources);
  SimDeviceGui::Add(DisplayAnalogOutputs);
}
