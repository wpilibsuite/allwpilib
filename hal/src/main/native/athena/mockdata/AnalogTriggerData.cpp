/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/simulation/AnalogTriggerData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {

int32_t HALSIM_FindAnalogTriggerForChannel(int32_t channel) { return 0; }

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
