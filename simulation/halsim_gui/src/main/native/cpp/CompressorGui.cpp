/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CompressorGui.h"

#include <cstdio>

#include <hal/Ports.h>
#include <hal/Value.h>
#include <imgui.h>
#include <mockdata/PCMData.h>

#include "HALSimGui.h"
#include "SimDeviceGui.h"

using namespace halsimgui;

static void DisplayCompressors() {
  static int numPcm = HAL_GetNumPCMModules();
  for (int i = 0; i < numPcm; ++i) {
    if (!HALSIM_GetPCMCompressorInitialized(i)) continue;
    char name[32];
    std::snprintf(name, sizeof(name), "Compressor[%d]", i);
    if (SimDeviceGui::StartDevice(name)) {
      HAL_Value value;

      // enabled
      if (HALSimGui::AreOutputsDisabled())
        value = HAL_MakeBoolean(false);
      else
        value = HAL_MakeBoolean(HALSIM_GetPCMCompressorOn(i));
      if (SimDeviceGui::DisplayValue("Running", false, &value))
        HALSIM_SetPCMCompressorOn(i, value.data.v_boolean);

      // closed loop
      value = HAL_MakeEnum(HALSIM_GetPCMClosedLoopEnabled(i) ? 1 : 0);
      static const char* enabledOptions[] = {"disabled", "enabled"};
      if (SimDeviceGui::DisplayValue("Closed Loop", true, &value,
                                     enabledOptions, 2))
        HALSIM_SetPCMClosedLoopEnabled(i, value.data.v_enum);

      // pressure switch
      value = HAL_MakeEnum(HALSIM_GetPCMPressureSwitch(i) ? 1 : 0);
      static const char* switchOptions[] = {"full", "low"};
      if (SimDeviceGui::DisplayValue("Pressure", false, &value, switchOptions,
                                     2))
        HALSIM_SetPCMPressureSwitch(i, value.data.v_enum);

      // compressor current
      value = HAL_MakeDouble(HALSIM_GetPCMCompressorCurrent(i));
      if (SimDeviceGui::DisplayValue("Current (A)", false, &value))
        HALSIM_SetPCMCompressorCurrent(i, value.data.v_double);

      SimDeviceGui::FinishDevice();
    }
  }
}

void CompressorGui::Initialize() { SimDeviceGui::Add(DisplayCompressors); }
