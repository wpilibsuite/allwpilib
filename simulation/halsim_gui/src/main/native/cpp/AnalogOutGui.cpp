/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogOutGui.h"

#include <cstdio>
#include <cstring>
#include <memory>

#include <hal/Ports.h>
#include <imgui.h>
#include <mockdata/AnalogOutData.h>

#include "SimDeviceGui.h"

using namespace halsimgui;

static void DisplayAnalogOutputs() {
  static const int numAnalog = HAL_GetNumAnalogOutputs();
  static auto init = std::make_unique<bool[]>(numAnalog);

  int count = 0;
  for (int i = 0; i < numAnalog; ++i) {
    init[i] = HALSIM_GetAnalogOutInitialized(i);
    if (init[i]) ++count;
  }

  if (count == 0) return;

  if (SimDeviceGui::StartDevice("Analog Outputs")) {
    for (int i = 0; i < numAnalog; ++i) {
      if (!init[i]) continue;
      char name[32];
      std::snprintf(name, sizeof(name), "Out[%d]", i);
      HAL_Value value = HAL_MakeDouble(HALSIM_GetAnalogOutVoltage(i));
      SimDeviceGui::DisplayValue(name, true, &value);
    }

    SimDeviceGui::FinishDevice();
  }
}

void AnalogOutGui::Initialize() { SimDeviceGui::Add(DisplayAnalogOutputs); }
