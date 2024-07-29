// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "../PortsInternal.h"
#include "AnalogGyroDataInternal.h"

using namespace hal;

namespace hal::init {
void InitializeAnalogGyroData() {
  static AnalogGyroData agd[kNumAccumulators];
  ::hal::SimAnalogGyroData = agd;
}
}  // namespace hal::init

AnalogGyroData* hal::SimAnalogGyroData;
void AnalogGyroData::ResetData() {
  angle.Reset(0.0);
  rate.Reset(0.0);
  initialized.Reset(false);
}

extern "C" {
void HALSIM_ResetAnalogGyroData(int32_t index) {
  SimAnalogGyroData[index].ResetData();
}

#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                     \
  HAL_SIMDATAVALUE_DEFINE_CAPI(TYPE, HALSIM, AnalogGyro##CAPINAME, \
                               SimAnalogGyroData, LOWERNAME)

DEFINE_CAPI(double, Angle, angle)
DEFINE_CAPI(double, Rate, rate)
DEFINE_CAPI(HAL_Bool, Initialized, initialized)

#define REGISTER(NAME) \
  SimAnalogGyroData[index].NAME.RegisterCallback(callback, param, initialNotify)

void HALSIM_RegisterAnalogGyroAllCallbacks(int32_t index,
                                           HAL_NotifyCallback callback,
                                           void* param,
                                           HAL_Bool initialNotify) {
  REGISTER(angle);
  REGISTER(rate);
  REGISTER(initialized);
}
}  // extern "C"
