// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/simulation/EncoderData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
int32_t HALSIM_FindEncoderForChannel(int32_t channel) {
  return 0;
}

void HALSIM_ResetEncoderData(int32_t index) {}

int32_t HALSIM_GetEncoderDigitalChannelA(int32_t index) {
  return 0;
}

int32_t HALSIM_GetEncoderDigitalChannelB(int32_t index) {
  return 0;
}

HAL_SimDeviceHandle HALSIM_GetEncoderSimDevice(int32_t index) {
  return 0;
}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, Encoder##CAPINAME, RETURN)

DEFINE_CAPI(HAL_Bool, Initialized, false)
DEFINE_CAPI(int32_t, Count, 0)
DEFINE_CAPI(double, Period, 0)
DEFINE_CAPI(HAL_Bool, Reset, false)
DEFINE_CAPI(double, MaxPeriod, 0)
DEFINE_CAPI(HAL_Bool, Direction, false)
DEFINE_CAPI(HAL_Bool, ReverseDirection, false)
DEFINE_CAPI(int32_t, SamplesToAverage, 0)
DEFINE_CAPI(double, DistancePerPulse, 0)

void HALSIM_SetEncoderDistance(int32_t index, double distance) {}

double HALSIM_GetEncoderDistance(int32_t index) {
  return 0;
}

void HALSIM_SetEncoderRate(int32_t index, double rate) {}

double HALSIM_GetEncoderRate(int32_t index) {
  return 0;
}

void HALSIM_RegisterEncoderAllCallbacks(int32_t index,
                                        HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify) {}
}  // extern "C"
