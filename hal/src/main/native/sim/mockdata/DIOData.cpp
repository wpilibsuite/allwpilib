// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../PortsInternal.h"
#include "DIODataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeDIOData() {
  static DIOData sdd[kNumDigitalChannels];
  ::hal::SimDIOData = sdd;
}
}  // namespace hal::init

DIOData* hal::SimDIOData;
void DIOData::ResetData() {
  initialized.Reset(false);
  simDevice = 0;
  value.Reset(true);
  pulseLength.Reset(0.0);
  isInput.Reset(true);
  filterIndex.Reset(-1);
}

extern "C" {
void HALSIM_ResetDIOData(int32_t index) {
  SimDIOData[index].ResetData();
}

HAL_SimDeviceHandle HALSIM_GetDIOSimDevice(int32_t index) {
  return SimDIOData[index].simDevice;
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                          \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, DIO##CAPINAME, SimDIOData, \
                               LOWERNAME)

DEFINE_CAPI(HAL_Bool, Initialized, initialized)
DEFINE_CAPI(HAL_Bool, Value, value)
DEFINE_CAPI(double, PulseLength, pulseLength)
DEFINE_CAPI(HAL_Bool, IsInput, isInput)
DEFINE_CAPI(int32_t, FilterIndex, filterIndex)

#define REGISTER(NAME) \
  SimDIOData[index].NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterDIOAllCallbacks(int32_t index, HAL_NotifyCallback callback,
                                    void* param, HAL_Bool initialNotify) {
  REGISTER(initialized);
  REGISTER(value);
  REGISTER(pulseLength);
  REGISTER(isInput);
  REGISTER(filterIndex);
}
}  // extern "C"
