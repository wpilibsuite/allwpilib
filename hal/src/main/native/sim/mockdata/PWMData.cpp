/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "../PortsInternal.h"
#include "PWMDataInternal.h"

using namespace hal;

namespace hal {
namespace init {
void InitializePWMData() {
  static PWMData spd[kNumPWMChannels];
  ::hal::SimPWMData = spd;
}
}  // namespace init
}  // namespace hal

PWMData* hal::SimPWMData;
void PWMData::ResetData() {
  initialized.Reset(false);
  rawValue.Reset(0);
  speed.Reset(0);
  position.Reset(0);
  periodScale.Reset(0);
  zeroLatch.Reset(false);
  displayName.Reset();
}

extern "C" {
void HALSIM_ResetPWMData(int32_t index) { SimPWMData[index].ResetData(); }

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                          \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, PWM##CAPINAME, SimPWMData, \
                               LOWERNAME)

const char* HALSIM_GetPWMDisplayName(int32_t index) {
  return SimPWMData[index].displayName.Get();
}
void HALSIM_SetPWMDisplayName(int32_t index, const char* displayName) {
  SimPWMData[index].displayName.Set(displayName);
}

DEFINE_CAPI(HAL_Bool, Initialized, initialized)
DEFINE_CAPI(int32_t, RawValue, rawValue)
DEFINE_CAPI(double, Speed, speed)
DEFINE_CAPI(double, Position, position)
DEFINE_CAPI(int32_t, PeriodScale, periodScale)
DEFINE_CAPI(HAL_Bool, ZeroLatch, zeroLatch)

#define REGISTER(NAME) \
  SimPWMData[index].NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterPWMAllCallbacks(int32_t index, HAL_NotifyCallback callback,
                                    void* param, HAL_Bool initialNotify) {
  REGISTER(initialized);
  REGISTER(rawValue);
  REGISTER(speed);
  REGISTER(position);
  REGISTER(periodScale);
  REGISTER(zeroLatch);
}
}  // extern "C"
