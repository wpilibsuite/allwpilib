/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/simulation/DriverStationData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {
void HALSIM_ResetDriverStationData(void) {}

#define DEFINE_CAPI(TYPE, CAPINAME, RETURN)                                 \
  HAL_SIMDATAVALUE_STUB_CAPI_NOINDEX(TYPE, HALSIM, DriverStation##CAPINAME, \
                                     RETURN)

DEFINE_CAPI(HAL_Bool, Enabled, false)
DEFINE_CAPI(HAL_Bool, Autonomous, false)
DEFINE_CAPI(HAL_Bool, Test, false)
DEFINE_CAPI(HAL_Bool, EStop, false)
DEFINE_CAPI(HAL_Bool, FmsAttached, false)
DEFINE_CAPI(HAL_Bool, DsAttached, false)
DEFINE_CAPI(HAL_AllianceStationID, AllianceStationId,
            HAL_AllianceStationID_kRed1)
DEFINE_CAPI(double, MatchTime, 0)

void HALSIM_SetJoystickAxes(int32_t joystickNum, const HAL_JoystickAxes* axes) {
}

void HALSIM_SetJoystickPOVs(int32_t joystickNum, const HAL_JoystickPOVs* povs) {
}

void HALSIM_SetJoystickButtons(int32_t joystickNum,
                               const HAL_JoystickButtons* buttons) {}

void HALSIM_SetJoystickDescriptor(int32_t joystickNum,
                                  const HAL_JoystickDescriptor* descriptor) {}

void HALSIM_GetJoystickOutputs(int32_t joystickNum, int64_t* outputs,
                               int32_t* leftRumble, int32_t* rightRumble) {}

void HALSIM_SetMatchInfo(const HAL_MatchInfo* info) {}

void HALSIM_NotifyDriverStationNewData(void) {}

void HALSIM_SetJoystickButton(int32_t stick, int32_t button, HAL_Bool state) {}

void HALSIM_SetJoystickAxis(int32_t stick, int32_t axis, double value) {}

void HALSIM_SetJoystickPOV(int32_t stick, int32_t pov, int32_t value) {}

void HALSIM_SetJoystickButtonsValue(int32_t stick, uint32_t buttons) {}

void HALSIM_SetJoystickAxisCount(int32_t stick, int32_t count) {}

void HALSIM_SetJoystickPOVCount(int32_t stick, int32_t count) {}

void HALSIM_SetJoystickButtonCount(int32_t stick, int32_t count) {}

void HALSIM_SetJoystickIsXbox(int32_t stick, HAL_Bool isXbox) {}

void HALSIM_SetJoystickType(int32_t stick, int32_t type) {}

void HALSIM_SetJoystickName(int32_t stick, const char* name) {}

void HALSIM_SetJoystickAxisType(int32_t stick, int32_t axis, int32_t type) {}

void HALSIM_SetGameSpecificMessage(const char* message) {}

void HALSIM_SetEventName(const char* name) {}

void HALSIM_SetMatchType(HAL_MatchType type) {}

void HALSIM_SetMatchNumber(int32_t matchNumber) {}

void HALSIM_SetReplayNumber(int32_t replayNumber) {}

void HALSIM_RegisterDriverStationAllCallbacks(HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify) {}
}  // extern "C"
