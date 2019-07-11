/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#ifndef __FRC_ROBORIO__

#include "NotifyListener.h"
#include "hal/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_ResetRoboRioData(int32_t index);
int32_t HALSIM_RegisterRoboRioFPGAButtonCallback(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify);
void HALSIM_CancelRoboRioFPGAButtonCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetRoboRioFPGAButton(int32_t index);
void HALSIM_SetRoboRioFPGAButton(int32_t index, HAL_Bool fPGAButton);

int32_t HALSIM_RegisterRoboRioVInVoltageCallback(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify);
void HALSIM_CancelRoboRioVInVoltageCallback(int32_t index, int32_t uid);
double HALSIM_GetRoboRioVInVoltage(int32_t index);
void HALSIM_SetRoboRioVInVoltage(int32_t index, double vInVoltage);

int32_t HALSIM_RegisterRoboRioVInCurrentCallback(int32_t index,
                                                 HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify);
void HALSIM_CancelRoboRioVInCurrentCallback(int32_t index, int32_t uid);
double HALSIM_GetRoboRioVInCurrent(int32_t index);
void HALSIM_SetRoboRioVInCurrent(int32_t index, double vInCurrent);

int32_t HALSIM_RegisterRoboRioUserVoltage6VCallback(int32_t index,
                                                    HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify);
void HALSIM_CancelRoboRioUserVoltage6VCallback(int32_t index, int32_t uid);
double HALSIM_GetRoboRioUserVoltage6V(int32_t index);
void HALSIM_SetRoboRioUserVoltage6V(int32_t index, double userVoltage6V);

int32_t HALSIM_RegisterRoboRioUserCurrent6VCallback(int32_t index,
                                                    HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify);
void HALSIM_CancelRoboRioUserCurrent6VCallback(int32_t index, int32_t uid);
double HALSIM_GetRoboRioUserCurrent6V(int32_t index);
void HALSIM_SetRoboRioUserCurrent6V(int32_t index, double userCurrent6V);

int32_t HALSIM_RegisterRoboRioUserActive6VCallback(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify);
void HALSIM_CancelRoboRioUserActive6VCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetRoboRioUserActive6V(int32_t index);
void HALSIM_SetRoboRioUserActive6V(int32_t index, HAL_Bool userActive6V);

int32_t HALSIM_RegisterRoboRioUserVoltage5VCallback(int32_t index,
                                                    HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify);
void HALSIM_CancelRoboRioUserVoltage5VCallback(int32_t index, int32_t uid);
double HALSIM_GetRoboRioUserVoltage5V(int32_t index);
void HALSIM_SetRoboRioUserVoltage5V(int32_t index, double userVoltage5V);

int32_t HALSIM_RegisterRoboRioUserCurrent5VCallback(int32_t index,
                                                    HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify);
void HALSIM_CancelRoboRioUserCurrent5VCallback(int32_t index, int32_t uid);
double HALSIM_GetRoboRioUserCurrent5V(int32_t index);
void HALSIM_SetRoboRioUserCurrent5V(int32_t index, double userCurrent5V);

int32_t HALSIM_RegisterRoboRioUserActive5VCallback(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify);
void HALSIM_CancelRoboRioUserActive5VCallback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetRoboRioUserActive5V(int32_t index);
void HALSIM_SetRoboRioUserActive5V(int32_t index, HAL_Bool userActive5V);

int32_t HALSIM_RegisterRoboRioUserVoltage3V3Callback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelRoboRioUserVoltage3V3Callback(int32_t index, int32_t uid);
double HALSIM_GetRoboRioUserVoltage3V3(int32_t index);
void HALSIM_SetRoboRioUserVoltage3V3(int32_t index, double userVoltage3V3);

int32_t HALSIM_RegisterRoboRioUserCurrent3V3Callback(
    int32_t index, HAL_NotifyCallback callback, void* param,
    HAL_Bool initialNotify);
void HALSIM_CancelRoboRioUserCurrent3V3Callback(int32_t index, int32_t uid);
double HALSIM_GetRoboRioUserCurrent3V3(int32_t index);
void HALSIM_SetRoboRioUserCurrent3V3(int32_t index, double userCurrent3V3);

int32_t HALSIM_RegisterRoboRioUserActive3V3Callback(int32_t index,
                                                    HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify);
void HALSIM_CancelRoboRioUserActive3V3Callback(int32_t index, int32_t uid);
HAL_Bool HALSIM_GetRoboRioUserActive3V3(int32_t index);
void HALSIM_SetRoboRioUserActive3V3(int32_t index, HAL_Bool userActive3V3);

int32_t HALSIM_RegisterRoboRioUserFaults6VCallback(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify);
void HALSIM_CancelRoboRioUserFaults6VCallback(int32_t index, int32_t uid);
int32_t HALSIM_GetRoboRioUserFaults6V(int32_t index);
void HALSIM_SetRoboRioUserFaults6V(int32_t index, int32_t userFaults6V);

int32_t HALSIM_RegisterRoboRioUserFaults5VCallback(int32_t index,
                                                   HAL_NotifyCallback callback,
                                                   void* param,
                                                   HAL_Bool initialNotify);
void HALSIM_CancelRoboRioUserFaults5VCallback(int32_t index, int32_t uid);
int32_t HALSIM_GetRoboRioUserFaults5V(int32_t index);
void HALSIM_SetRoboRioUserFaults5V(int32_t index, int32_t userFaults5V);

int32_t HALSIM_RegisterRoboRioUserFaults3V3Callback(int32_t index,
                                                    HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify);
void HALSIM_CancelRoboRioUserFaults3V3Callback(int32_t index, int32_t uid);
int32_t HALSIM_GetRoboRioUserFaults3V3(int32_t index);
void HALSIM_SetRoboRioUserFaults3V3(int32_t index, int32_t userFaults3V3);

void HALSIM_RegisterRoboRioAllCallbacks(int32_t index,
                                        HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
