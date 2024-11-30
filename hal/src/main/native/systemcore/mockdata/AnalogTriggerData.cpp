// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/simulation/AnalogTriggerData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {

int32_t HALSIM_FindAnalogTriggerForChannel(int32_t channel) {
  return 0;
}

void HALSIM_ResetAnalogTriggerData(int32_t index) {}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, AnalogTrigger##CAPINAME, RETURN)

DEFINE_CAPI(HAL_Bool, Initialized, false)
DEFINE_CAPI(double, TriggerLowerBound, 0)
DEFINE_CAPI(double, TriggerUpperBound, 0)
DEFINE_CAPI(HALSIM_AnalogTriggerMode, TriggerMode,
            HALSIM_AnalogTriggerUnassigned)

void HALSIM_RegisterAnalogTriggerAllCallbacks(int32_t index,
                                              HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {}
}  // extern "C"
