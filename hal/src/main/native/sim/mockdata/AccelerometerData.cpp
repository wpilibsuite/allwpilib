/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "AccelerometerDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeAccelerometerData() {
  static AccelerometerData sad[1];
  ::hal::SimAccelerometerData = sad;
}
}  // namespace init
}  // namespace hal

AccelerometerData* hal::SimAccelerometerData;
void AccelerometerData::ResetData() {
  active.Reset(false);
  range.Reset(static_cast<HAL_AccelerometerRange>(0));
  x.Reset(0.0);
  y.Reset(0.0);
  z.Reset(0.0);
  displayName.Reset();
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

const char* HALSIM_GetAccelerometerDisplayName(int32_t index) {
  return SimAccelerometerData[index].displayName.Get();
}
void HALSIM_SetAccelerometerDisplayName(int32_t index,
                                        const char* displayName) {
  SimAccelerometerData[index].displayName.Set(displayName);
}

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
