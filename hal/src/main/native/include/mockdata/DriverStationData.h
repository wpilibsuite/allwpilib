/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "NotifyListener.h"
#include "hal/DriverStationTypes.h"
#include "hal/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_ResetDriverStationData(void);
int32_t HALSIM_RegisterDriverStationEnabledCallback(HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify);
void HALSIM_CancelDriverStationEnabledCallback(int32_t uid);
HAL_Bool HALSIM_GetDriverStationEnabled(void);
void HALSIM_SetDriverStationEnabled(HAL_Bool enabled);

int32_t HALSIM_RegisterDriverStationAutonomousCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelDriverStationAutonomousCallback(int32_t uid);
HAL_Bool HALSIM_GetDriverStationAutonomous(void);
void HALSIM_SetDriverStationAutonomous(HAL_Bool autonomous);

int32_t HALSIM_RegisterDriverStationTestCallback(HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify);
void HALSIM_CancelDriverStationTestCallback(int32_t uid);
HAL_Bool HALSIM_GetDriverStationTest(void);
void HALSIM_SetDriverStationTest(HAL_Bool test);

int32_t HALSIM_RegisterDriverStationEStopCallback(HAL_NotifyCallback callback,
                                                  void* param,
                                                  HAL_Bool initialNotify);
void HALSIM_CancelDriverStationEStopCallback(int32_t uid);
HAL_Bool HALSIM_GetDriverStationEStop(void);
void HALSIM_SetDriverStationEStop(HAL_Bool eStop);

int32_t HALSIM_RegisterDriverStationFmsAttachedCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelDriverStationFmsAttachedCallback(int32_t uid);
HAL_Bool HALSIM_GetDriverStationFmsAttached(void);
void HALSIM_SetDriverStationFmsAttached(HAL_Bool fmsAttached);

int32_t HALSIM_RegisterDriverStationDsAttachedCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelDriverStationDsAttachedCallback(int32_t uid);
HAL_Bool HALSIM_GetDriverStationDsAttached(void);
void HALSIM_SetDriverStationDsAttached(HAL_Bool dsAttached);

int32_t HALSIM_RegisterDriverStationAllianceStationIdCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelDriverStationAllianceStationIdCallback(int32_t uid);
HAL_AllianceStationID HALSIM_GetDriverStationAllianceStationId(void);
void HALSIM_SetDriverStationAllianceStationId(
    HAL_AllianceStationID allianceStationId);

int32_t HALSIM_RegisterDriverStationMatchTimeCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelDriverStationMatchTimeCallback(int32_t uid);
double HALSIM_GetDriverStationMatchTime(void);
void HALSIM_SetDriverStationMatchTime(double matchTime);

void HALSIM_SetJoystickAxes(int32_t joystickNum, const HAL_JoystickAxes* axes);
void HALSIM_SetJoystickPOVs(int32_t joystickNum, const HAL_JoystickPOVs* povs);
void HALSIM_SetJoystickButtons(int32_t joystickNum,
                               const HAL_JoystickButtons* buttons);
void HALSIM_SetJoystickDescriptor(int32_t joystickNum,
                                  const HAL_JoystickDescriptor* descriptor);

void HALSIM_GetJoystickOutputs(int32_t joystickNum, int64_t* outputs,
                               int32_t* leftRumble, int32_t* rightRumble);

void HALSIM_SetMatchInfo(const HAL_MatchInfo* info);

void HALSIM_RegisterDriverStationAllCallbacks(HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify);

void HALSIM_NotifyDriverStationNewData(void);

#ifdef __cplusplus
}  // extern "C"
#endif
