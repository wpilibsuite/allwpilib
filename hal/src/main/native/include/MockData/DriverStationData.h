/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "HAL/HAL.h"
#include "NotifyListener.h"

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_ResetDriverStationData(void);
int32_t HALSIM_RegisterDriverStationEnabledCallback(HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify);
void HALSIM_CancelDriverStationEnabledCallback(int32_t uid);
HAL_Bool HALSIM_GetDriverStationEnabled();
void HALSIM_SetDriverStationEnabled(HAL_Bool enabled);

int32_t HALSIM_RegisterDriverStationAutonomousCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelDriverStationAutonomousCallback(int32_t uid);
HAL_Bool HALSIM_GetDriverStationAutonomous();
void HALSIM_SetDriverStationAutonomous(HAL_Bool autonomous);

int32_t HALSIM_RegisterDriverStationTestCallback(HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify);
void HALSIM_CancelDriverStationTestCallback(int32_t uid);
HAL_Bool HALSIM_GetDriverStationTest();
void HALSIM_SetDriverStationTest(HAL_Bool test);

int32_t HALSIM_RegisterDriverStationEStopCallback(HAL_NotifyCallback callback,
                                                  void* param,
                                                  HAL_Bool initialNotify);
void HALSIM_CancelDriverStationEStopCallback(int32_t uid);
HAL_Bool HALSIM_GetDriverStationEStop();
void HALSIM_SetDriverStationEStop(HAL_Bool eStop);

int32_t HALSIM_RegisterDriverStationFmsAttachedCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelDriverStationFmsAttachedCallback(int32_t uid);
HAL_Bool HALSIM_GetDriverStationFmsAttached();
void HALSIM_SetDriverStationFmsAttached(HAL_Bool fmsAttached);

int32_t HALSIM_RegisterDriverStationDsAttachedCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelDriverStationDsAttachedCallback(int32_t uid);
HAL_Bool HALSIM_GetDriverStationDsAttached();
void HALSIM_SetDriverStationDsAttached(HAL_Bool dsAttached);

int32_t HALSIM_RegisterDriverStationAllianceStationIdCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelDriverStationAllianceStationIdCallback(int32_t uid);
HAL_AllianceStationID HALSIM_GetDriverStationAllianceStationId();
void HALSIM_SetDriverStationAllianceStationId(
    HAL_AllianceStationID allianceStationId);

int32_t HALSIM_RegisterDriverStationMatchTimeCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelDriverStationMatchTimeCallback(int32_t uid);
double HALSIM_GetDriverStationMatchTime();
void HALSIM_SetDriverStationMatchTime(double matchTime);

void HALSIM_NotifyDriverStationNewData(void);

void HALSIM_RegisterDriverStationAllCallbacks(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify);

#ifdef __cplusplus
}
#endif
