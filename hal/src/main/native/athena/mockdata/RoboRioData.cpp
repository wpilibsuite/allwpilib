/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/simulation/RoboRioData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
void HALSIM_ResetRoboRioData(void) {}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI_NOINDEX(TYPE, HALSIM, RoboRio##CAPINAME, RETURN)

DEFINE_CAPI(HAL_Bool, FPGAButton, false)
DEFINE_CAPI(double, VInVoltage, 0)
DEFINE_CAPI(double, VInCurrent, 0)
DEFINE_CAPI(double, UserVoltage6V, 0)
DEFINE_CAPI(double, UserCurrent6V, 0)
DEFINE_CAPI(HAL_Bool, UserActive6V, false)
DEFINE_CAPI(double, UserVoltage5V, 0)
DEFINE_CAPI(double, UserCurrent5V, 0)
DEFINE_CAPI(HAL_Bool, UserActive5V, false)
DEFINE_CAPI(double, UserVoltage3V3, 0)
DEFINE_CAPI(double, UserCurrent3V3, 0)
DEFINE_CAPI(HAL_Bool, UserActive3V3, false)
DEFINE_CAPI(int32_t, UserFaults6V, 0)
DEFINE_CAPI(int32_t, UserFaults5V, 0)
DEFINE_CAPI(int32_t, UserFaults3V3, 0)

void HALSIM_RegisterRoboRioAllCallbacks(HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify) {}
}  // extern "C"
