// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../PortsInternal.h"
#include "PWMDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializePWMData() {
  static PWMData spd[kNumPWMChannels];
  ::hal::SimPWMData = spd;
}
}  // namespace hal::init

PWMData* hal::SimPWMData;
void PWMData::ResetData() {
  initialized.Reset(false);
  rawValue.Reset(0);
  speed.Reset(0);
  position.Reset(0);
  periodScale.Reset(0);
  zeroLatch.Reset(false);
}

extern "C" {
void HALSIM_ResetPWMData(int32_t index) {
  SimPWMData[index].ResetData();
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                          \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, PWM##CAPINAME, SimPWMData, \
                               LOWERNAME)

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
