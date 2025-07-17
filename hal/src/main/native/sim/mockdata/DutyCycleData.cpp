// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../PortsInternal.h"
#include "DutyCycleDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeDutyCycleData() {
  static DutyCycleData sed[kNumDutyCycles];
  ::hal::SimDutyCycleData = sed;
}
}  // namespace hal::init

DutyCycleData* hal::SimDutyCycleData;

void DutyCycleData::ResetData() {
  initialized.Reset(false);
  simDevice = 0;
  frequency.Reset(0);
  output.Reset(0);
}

extern "C" {

void HALSIM_ResetDutyCycleData(int32_t index) {
  SimDutyCycleData[index].ResetData();
}

HAL_SimDeviceHandle HALSIM_GetDutyCycleSimDevice(int32_t index) {
  return SimDutyCycleData[index].simDevice;
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                    \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, DutyCycle##CAPINAME, \
                               SimDutyCycleData, LOWERNAME)

DEFINE_CAPI(HAL_Bool, Initialized, initialized)
DEFINE_CAPI(double, Frequency, frequency)
DEFINE_CAPI(double, Output, output)

#define REGISTER(NAME) \
  SimDutyCycleData[index].NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterDutyCycleAllCallbacks(int32_t index,
                                          HAL_NotifyCallback callback,
                                          void* param, HAL_Bool initialNotify) {
  REGISTER(initialized);
  REGISTER(frequency);
  REGISTER(output);
}

}  // extern "C"
