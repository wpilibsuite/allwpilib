// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../PortsInternal.h"
#include "AnalogInDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeAnalogInData() {
  static AnalogInData sind[NUM_ANALOG_INPUTS];
  ::hal::SimAnalogInData = sind;
}
}  // namespace hal::init

AnalogInData* hal::SimAnalogInData;
void AnalogInData::ResetData() {
  initialized.Reset(false);
  simDevice = 0;
  averageBits.Reset(7);
  oversampleBits.Reset(0);
  voltage.Reset(0.0);
}

extern "C" {
void HALSIM_ResetAnalogInData(int32_t index) {
  SimAnalogInData[index].ResetData();
}

HAL_SimDeviceHandle HALSIM_GetAnalogInSimDevice(int32_t index) {
  return SimAnalogInData[index].simDevice;
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                   \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, AnalogIn##CAPINAME, \
                               SimAnalogInData, LOWERNAME)

DEFINE_CAPI(HAL_Bool, Initialized, initialized)
DEFINE_CAPI(int32_t, AverageBits, averageBits)
DEFINE_CAPI(int32_t, OversampleBits, oversampleBits)
DEFINE_CAPI(double, Voltage, voltage)

#define REGISTER(NAME) \
  SimAnalogInData[index].NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterAnalogInAllCallbacks(int32_t index,
                                         HAL_NotifyCallback callback,
                                         void* param, HAL_Bool initialNotify) {
  REGISTER(initialized);
  REGISTER(averageBits);
  REGISTER(oversampleBits);
  REGISTER(voltage);
}
}  // extern "C"
