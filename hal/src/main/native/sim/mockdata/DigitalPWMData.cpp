// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../PortsInternal.h"
#include "DigitalPWMDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeDigitalPWMData() {
  static DigitalPWMData sdpd[kNumDigitalPWMOutputs];
  ::hal::SimDigitalPWMData = sdpd;
}
}  // namespace hal::init

DigitalPWMData* hal::SimDigitalPWMData;
void DigitalPWMData::ResetData() {
  initialized.Reset(false);
  dutyCycle.Reset(0.0);
  pin.Reset(0);
}

extern "C" {
int32_t HALSIM_FindDigitalPWMForChannel(int32_t channel) {
  for (int i = 0; i < kNumDigitalPWMOutputs; ++i) {
    if (SimDigitalPWMData[i].initialized &&
        SimDigitalPWMData[i].pin == channel) {
      return i;
    }
  }
  return -1;
}

void HALSIM_ResetDigitalPWMData(int32_t index) {
  SimDigitalPWMData[index].ResetData();
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                     \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, DigitalPWM##CAPINAME, \
                               SimDigitalPWMData, LOWERNAME)

DEFINE_CAPI(HAL_Bool, Initialized, initialized)
DEFINE_CAPI(double, DutyCycle, dutyCycle)
DEFINE_CAPI(int32_t, Pin, pin)

#define REGISTER(NAME) \
  SimDigitalPWMData[index].NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterDigitalPWMAllCallbacks(int32_t index,
                                           HAL_NotifyCallback callback,
                                           void* param,
                                           HAL_Bool initialNotify) {
  REGISTER(initialized);
  REGISTER(dutyCycle);
  REGISTER(pin);
}
}  // extern "C"
