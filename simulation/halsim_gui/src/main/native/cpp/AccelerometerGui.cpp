/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AccelerometerGui.h"

#include <cstdio>
#include <memory>

#include <hal/Value.h>
#include <hal/simulation/AccelerometerData.h>
#include <imgui.h>

#include "GuiDataSource.h"
#include "HALSimGui.h"
#include "SimDeviceGui.h"

using namespace halsimgui;

namespace {
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(AccelerometerX, "X Accel");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(AccelerometerY, "Y Accel");
HALSIMGUI_DATASOURCE_DOUBLE_INDEXED(AccelerometerZ, "Z Accel");
}  // namespace

static std::unique_ptr<AccelerometerXSource> gAccelXSource;
static std::unique_ptr<AccelerometerYSource> gAccelYSource;
static std::unique_ptr<AccelerometerZSource> gAccelZSource;

static void UpdateAccelSources() {
  if (!HALSIM_GetAccelerometerActive(0)) return;
  if (!gAccelXSource) gAccelXSource = std::make_unique<AccelerometerXSource>(0);
  if (!gAccelYSource) gAccelYSource = std::make_unique<AccelerometerYSource>(0);
  if (!gAccelZSource) gAccelZSource = std::make_unique<AccelerometerZSource>(0);
}

static void DisplayAccelerometers() {
  if (!HALSIM_GetAccelerometerActive(0)) return;
  if (SimDeviceGui::StartDevice("BuiltInAccel",
                                HALSIM_GetAccelerometerDisplayName(0))) {
    HAL_Value value;

    // Range
    value = HAL_MakeEnum(HALSIM_GetAccelerometerRange(0));
    static const char* rangeOptions[] = {"2G", "4G", "8G"};
    SimDeviceGui::DisplayValue("Range", true, &value, rangeOptions, 3);

    // X Accel
    value = HAL_MakeDouble(gAccelXSource->GetValue());
    if (SimDeviceGui::DisplayValueSource("X Accel", false, &value,
                                         gAccelXSource.get()))
      HALSIM_SetAccelerometerX(0, value.data.v_double);

    // Y Accel
    value = HAL_MakeDouble(gAccelYSource->GetValue());
    if (SimDeviceGui::DisplayValueSource("Y Accel", false, &value,
                                         gAccelYSource.get()))
      HALSIM_SetAccelerometerY(0, value.data.v_double);

    // Z Accel
    value = HAL_MakeDouble(gAccelZSource->GetValue());
    if (SimDeviceGui::DisplayValueSource("Z Accel", false, &value,
                                         gAccelZSource.get()))
      HALSIM_SetAccelerometerZ(0, value.data.v_double);

    SimDeviceGui::FinishDevice();
  }
}

void AccelerometerGui::Initialize() {
  HALSimGui::AddExecute(UpdateAccelSources);
  SimDeviceGui::Add(DisplayAccelerometers);
}
