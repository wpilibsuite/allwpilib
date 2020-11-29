/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CompressorGui.h"

#include <cstdio>
#include <memory>
#include <vector>

#include <hal/Ports.h>
#include <hal/Value.h>
#include <hal/simulation/PCMData.h>
#include <imgui.h>

#include "GuiDataSource.h"
#include "HALSimGui.h"
#include "SimDeviceGui.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED(PCMCompressorOn, "Compressor On");
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED(PCMClosedLoopEnabled, "Closed Loop");
HALSIMGUI_DATASOURCE_BOOLEAN_INDEXED(PCMPressureSwitch, "Pressure Switch");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(PCMCompressorCurrent, "Comp Current");
struct CompressorSource {
  explicit CompressorSource(int32_t index)
      : running{index}, enabled{index}, pressureSwitch{index}, current{index} {}
  PCMCompressorOnSource running;
  PCMClosedLoopEnabledSource enabled;
  PCMPressureSwitchSource pressureSwitch;
  PCMCompressorCurrentSource current;
};
}  // namespace

static std::vector<std::unique_ptr<CompressorSource>> gCompressorSources;

static void UpdateCompressorSources() {
  for (int i = 0, iend = gCompressorSources.size(); i < iend; ++i) {
    auto& source = gCompressorSources[i];
    if (HALSIM_GetPCMCompressorInitialized(i)) {
      if (!source) {
        source = std::make_unique<CompressorSource>(i);
      }
    } else {
      source.reset();
    }
  }
}

static void DisplayCompressors() {
  for (int i = 0, iend = gCompressorSources.size(); i < iend; ++i) {
    if (auto source = gCompressorSources[i].get()) {
      char name[32];
      std::snprintf(name, sizeof(name), "Compressor[%d]", i);
      if (SimDeviceGui::StartDevice(name, name)) {
        HAL_Value value;

        // enabled
        if (HALSimGui::AreOutputsDisabled())
          value = HAL_MakeBoolean(false);
        else
          value = HAL_MakeBoolean(source->running.GetValue());
        if (SimDeviceGui::DisplayValueSource("Running", false, &value,
                                             &source->running))
          HALSIM_SetPCMCompressorOn(i, value.data.v_boolean);

        // closed loop
        value = HAL_MakeEnum(source->enabled.GetValue() ? 1 : 0);
        static const char* enabledOptions[] = {"disabled", "enabled"};
        if (SimDeviceGui::DisplayValueSource("Closed Loop", true, &value,
                                             &source->enabled, enabledOptions,
                                             2))
          HALSIM_SetPCMClosedLoopEnabled(i, value.data.v_enum);

        // pressure switch
        value = HAL_MakeEnum(source->pressureSwitch.GetValue() ? 1 : 0);
        static const char* switchOptions[] = {"full", "low"};
        if (SimDeviceGui::DisplayValueSource("Pressure", false, &value,
                                             &source->pressureSwitch,
                                             switchOptions, 2))
          HALSIM_SetPCMPressureSwitch(i, value.data.v_enum);

        // compressor current
        value = HAL_MakeDouble(source->current.GetValue());
        if (SimDeviceGui::DisplayValueSource("Current (A)", false, &value,
                                             &source->current))
          HALSIM_SetPCMCompressorCurrent(i, value.data.v_double);

        SimDeviceGui::FinishDevice();
      }
    }
  }
}

void CompressorGui::Initialize() {
  gCompressorSources.resize(HAL_GetNumPCMModules());
  HALSimGui::AddExecute(UpdateCompressorSources);
  SimDeviceGui::Add(DisplayCompressors);
}
