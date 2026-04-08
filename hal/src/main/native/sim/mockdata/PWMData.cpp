// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../PortsInternal.hpp"
#include "PWMDataInternal.hpp"

using namespace wpi::hal;

namespace wpi::hal::init {
void InitializePWMData() {
  static PWMData spd[kNumPWMChannels];
  ::wpi::hal::SimPWMData = spd;
}
}  // namespace wpi::hal::init

PWMData* wpi::hal::SimPWMData;
void PWMData::ResetData() {
  initialized.Reset(false);
  simDevice = 0;
  pulseMicrosecond.Reset(0);
  outputPeriod.Reset(0);
}

extern "C" {
void HALSIM_ResetPWMData(int32_t index) {
  SimPWMData[index].ResetData();
}

HAL_SimDeviceHandle HALSIM_GetPWMSimDevice(int32_t index) {
  return SimPWMData[index].simDevice;
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                          \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, PWM##CAPINAME, SimPWMData, \
                               LOWERNAME)

DEFINE_CAPI(HAL_Bool, Initialized, initialized)
DEFINE_CAPI(int32_t, PulseMicrosecond, pulseMicrosecond)
DEFINE_CAPI(int32_t, OutputPeriod, outputPeriod)

#define REGISTER(NAME) \
  SimPWMData[index].NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterPWMAllCallbacks(int32_t index, HAL_NotifyCallback callback,
                                    void* param, HAL_Bool initialNotify) {
  REGISTER(initialized);
  REGISTER(pulseMicrosecond);
  REGISTER(outputPeriod);
}
}  // extern "C"
