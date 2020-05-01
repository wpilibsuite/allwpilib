/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogOutGui.h"

#include <hal/Ports.h>
#include <imgui.h>
#include <mockdata/AnalogOutData.h>

#include "IniSaver.h"
#include "IniSaverInfo.h"
#include "SimDeviceGui.h"

using namespace halsimgui;

static IniSaver<NameInfo> gAnalogOuts{"AnalogOut"};  // indexed by channel

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

      auto& info = gAnalogOuts[i];
      char name[128];
      info.GetName(name, sizeof(name), "Out", i);
      HAL_Value value = HAL_MakeDouble(HALSIM_GetAnalogOutVoltage(i));
      SimDeviceGui::DisplayValue(name, true, &value);
      info.PopupEditName(i);
    }

    SimDeviceGui::FinishDevice();
  }
}

void AnalogOutGui::Initialize() {
  gAnalogOuts.Initialize();
  SimDeviceGui::Add(DisplayAnalogOutputs);
}
