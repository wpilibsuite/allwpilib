/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/simulation/AnalogGyroData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
void HALSIM_ResetAnalogGyroData(int32_t index) {}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, AnalogGyro##CAPINAME, RETURN)

const char* HALSIM_GetAnalogGyroDisplayName(int32_t index) { return ""; }
void HALSIM_SetAnalogGyroDisplayName(int32_t index, const char* displayName) {}
DEFINE_CAPI(double, Angle, 0)
DEFINE_CAPI(double, Rate, 0)
DEFINE_CAPI(HAL_Bool, Initialized, false)

void HALSIM_RegisterAnalogGyroAllCallbacks(int32_t index,
                                           HAL_NotifyCallback callback,
                                           void* param,
                                           HAL_Bool initialNotify) {}
}  // extern "C"
