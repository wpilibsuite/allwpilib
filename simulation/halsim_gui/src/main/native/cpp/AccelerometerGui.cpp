/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AccelerometerGui.h"

#include <cstdio>

#include <hal/Value.h>
#include <imgui.h>
#include <mockdata/AccelerometerData.h>

#include "SimDeviceGui.h"

using namespace halsimgui;

static void DisplayAccelerometers() {
  if (!HALSIM_GetAccelerometerActive(0)) return;
  if (SimDeviceGui::StartDevice("BuiltInAccel")) {
    HAL_Value value;

    // Range
    value = HAL_MakeEnum(HALSIM_GetAccelerometerRange(0));
    static const char* rangeOptions[] = {"2G", "4G", "8G"};
    SimDeviceGui::DisplayValue("Range", true, &value, rangeOptions, 3);

    // X Accel
    value = HAL_MakeDouble(HALSIM_GetAccelerometerX(0));
    if (SimDeviceGui::DisplayValue("X Accel", false, &value))
      HALSIM_SetAccelerometerX(0, value.data.v_double);

    // Y Accel
    value = HAL_MakeDouble(HALSIM_GetAccelerometerY(0));
    if (SimDeviceGui::DisplayValue("Y Accel", false, &value))
      HALSIM_SetAccelerometerY(0, value.data.v_double);

    // Z Accel
    value = HAL_MakeDouble(HALSIM_GetAccelerometerZ(0));
    if (SimDeviceGui::DisplayValue("Z Accel", false, &value))
      HALSIM_SetAccelerometerZ(0, value.data.v_double);

    SimDeviceGui::FinishDevice();
  }
}

void AccelerometerGui::Initialize() {
  SimDeviceGui::Add(DisplayAccelerometers);
}
