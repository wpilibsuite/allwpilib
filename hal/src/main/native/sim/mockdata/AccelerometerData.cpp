// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../PortsInternal.h"
#include "AccelerometerDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeAccelerometerData() {
  static AccelerometerData sad[kAccelerometers];
  ::hal::SimAccelerometerData = sad;
}
}  // namespace hal::init

AccelerometerData* hal::SimAccelerometerData;
void AccelerometerData::ResetData() {
  active.Reset(false);
  range.Reset(static_cast<HAL_AccelerometerRange>(0));
  x.Reset(0.0);
  y.Reset(0.0);
  z.Reset(0.0);
}

extern "C" {
void HALSIM_ResetAccelerometerData(int32_t index) {
  SimAccelerometerData[index].ResetData();
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                        \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, Accelerometer##CAPINAME, \
                               SimAccelerometerData, LOWERNAME)

DEFINE_CAPI(HAL_Bool, Active, active)
DEFINE_CAPI(HAL_AccelerometerRange, Range, range)
DEFINE_CAPI(double, X, x)
DEFINE_CAPI(double, Y, y)
DEFINE_CAPI(double, Z, z)

#define REGISTER(NAME)                                               \
  SimAccelerometerData[index].NAME.RegisterCallback(callback, param, \
                                                    initialNotify)

void HALSIM_RegisterAccelerometerAllCallbacks(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {
  REGISTER(active);
  REGISTER(range);
  REGISTER(x);
  REGISTER(y);
  REGISTER(z);
}
}  // extern "C"
