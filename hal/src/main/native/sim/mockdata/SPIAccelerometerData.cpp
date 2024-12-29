// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../PortsInternal.h"
#include "SPIAccelerometerDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeSPIAccelerometerData() {
  static SPIAccelerometerData ssad[kSPIAccelerometers];
  ::hal::SimSPIAccelerometerData = ssad;
}
}  // namespace hal::init

SPIAccelerometerData* hal::SimSPIAccelerometerData;
void SPIAccelerometerData::ResetData() {
  active.Reset(false);
  range.Reset(0);
  x.Reset(0.0);
  y.Reset(0.0);
  z.Reset(0.0);
}

extern "C" {
void HALSIM_ResetSPIAccelerometerData(int32_t index) {
  SimSPIAccelerometerData[index].ResetData();
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                           \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, SPIAccelerometer##CAPINAME, \
                               SimSPIAccelerometerData, LOWERNAME)

DEFINE_CAPI(HAL_Bool, Active, active)
DEFINE_CAPI(int32_t, Range, range)
DEFINE_CAPI(double, X, x)
DEFINE_CAPI(double, Y, y)
DEFINE_CAPI(double, Z, z)

#define REGISTER(NAME)                                                  \
  SimSPIAccelerometerData[index].NAME.RegisterCallback(callback, param, \
                                                       initialNotify)

void HALSIM_RegisterSPIAccelerometerAllCallbacks(int32_t index,
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
