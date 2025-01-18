// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
DEFINE_CAPI(double, BrownoutVoltage, 6.75)
DEFINE_CAPI(double, CPUTemp, 45.0)
DEFINE_CAPI(int32_t, TeamNumber, 0)
DEFINE_CAPI(HAL_RadioLEDState, RadioLEDState, HAL_RadioLED_kOff);

int32_t HALSIM_RegisterRoboRioSerialNumberCallback(
    HAL_RoboRioStringCallback callback, void* param, HAL_Bool initialNotify) {
  return 0;
}
void HALSIM_CancelRoboRioSerialNumberCallback(int32_t uid) {}
void HALSIM_GetRoboRioSerialNumber(struct WPI_String* serialNumber) {
  WPI_AllocateString(serialNumber, 0);
}
void HALSIM_SetRoboRioSerialNumber(const struct WPI_String* serialNumber) {}

int32_t HALSIM_RegisterRoboRioCommentsCallback(
    HAL_RoboRioStringCallback callback, void* param, HAL_Bool initialNotify) {
  return 0;
}
void HALSIM_CancelRoboRioCommentsCallback(int32_t uid) {}
void HALSIM_GetRoboRioComments(struct WPI_String* comments) {
  WPI_AllocateString(comments, 0);
}
void HALSIM_SetRoboRioComments(const struct WPI_String* comments) {}

void HALSIM_RegisterRoboRioAllCallbacks(HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify) {}
}  // extern "C"
