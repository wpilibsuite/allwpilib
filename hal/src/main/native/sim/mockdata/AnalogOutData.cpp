// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../PortsInternal.h"
#include "AnalogOutDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeAnalogOutData() {
  static AnalogOutData siod[kNumAnalogOutputs];
  ::hal::SimAnalogOutData = siod;
}
}  // namespace hal::init

AnalogOutData* hal::SimAnalogOutData;
void AnalogOutData::ResetData() {
  voltage.Reset(0.0);
  initialized.Reset(0);
}

extern "C" {
void HALSIM_ResetAnalogOutData(int32_t index) {
  SimAnalogOutData[index].ResetData();
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                    \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, AnalogOut##CAPINAME, \
                               SimAnalogOutData, LOWERNAME)

DEFINE_CAPI(double, Voltage, voltage)
DEFINE_CAPI(HAL_Bool, Initialized, initialized)

#define REGISTER(NAME) \
  SimAnalogOutData[index].NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterAnalogOutAllCallbacks(int32_t index,
                                          HAL_NotifyCallback callback,
                                          void* param, HAL_Bool initialNotify) {
  REGISTER(voltage);
  REGISTER(initialized);
}
}  // extern "C"
