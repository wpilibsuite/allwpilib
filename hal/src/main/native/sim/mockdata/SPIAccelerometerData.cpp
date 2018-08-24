/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "SPIAccelerometerDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializeSPIAccelerometerData() {
  static SPIAccelerometerData ssad[5];
  ::hal::SimSPIAccelerometerData = ssad;
}
}  // namespace init
}  // namespace hal

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

void HALSIM_RegisterSPIAccelerometerAllCallbcaks(int32_t index,
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
