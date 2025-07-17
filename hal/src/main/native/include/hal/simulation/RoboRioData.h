// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstddef>

#include <wpi/string.h>

#include "hal/Types.h"
#include "hal/simulation/NotifyListener.h"

typedef void (*HAL_RoboRioStringCallback)(const char* name, void* param,
                                          const char* str, size_t size);

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_ResetRoboRioData(void);
int32_t HALSIM_RegisterRoboRioFPGAButtonCallback(HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify);
void HALSIM_CancelRoboRioFPGAButtonCallback(int32_t uid);
HAL_Bool HALSIM_GetRoboRioFPGAButton(void);
void HALSIM_SetRoboRioFPGAButton(HAL_Bool fPGAButton);

int32_t HALSIM_RegisterRoboRioVInVoltageCallback(HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify);
void HALSIM_CancelRoboRioVInVoltageCallback(int32_t uid);
double HALSIM_GetRoboRioVInVoltage(void);
void HALSIM_SetRoboRioVInVoltage(double vInVoltage);

int32_t HALSIM_RegisterRoboRioUserVoltage3V3Callback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelRoboRioUserVoltage3V3Callback(int32_t uid);
double HALSIM_GetRoboRioUserVoltage3V3(void);
void HALSIM_SetRoboRioUserVoltage3V3(double userVoltage3V3);

int32_t HALSIM_RegisterRoboRioUserCurrent3V3Callback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelRoboRioUserCurrent3V3Callback(int32_t uid);
double HALSIM_GetRoboRioUserCurrent3V3(void);
void HALSIM_SetRoboRioUserCurrent3V3(double userCurrent3V3);

int32_t HALSIM_RegisterRoboRioUserActive3V3Callback(HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify);
void HALSIM_CancelRoboRioUserActive3V3Callback(int32_t uid);
HAL_Bool HALSIM_GetRoboRioUserActive3V3(void);
void HALSIM_SetRoboRioUserActive3V3(HAL_Bool userActive3V3);

int32_t HALSIM_RegisterRoboRioUserFaults3V3Callback(HAL_NotifyCallback callback,
                                                    void* param,
                                                    HAL_Bool initialNotify);
void HALSIM_CancelRoboRioUserFaults3V3Callback(int32_t uid);
int32_t HALSIM_GetRoboRioUserFaults3V3(void);
void HALSIM_SetRoboRioUserFaults3V3(int32_t userFaults3V3);

int32_t HALSIM_RegisterRoboRioBrownoutVoltageCallback(
    HAL_NotifyCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelRoboRioBrownoutVoltageCallback(int32_t uid);
double HALSIM_GetRoboRioBrownoutVoltage(void);
void HALSIM_SetRoboRioBrownoutVoltage(double brownoutVoltage);

int32_t HALSIM_RegisterRoboRioTeamNumberCallback(HAL_NotifyCallback callback,
                                                 void* param,
                                                 HAL_Bool initialNotify);
void HALSIM_CancelRoboRioTeamNumberCallback(int32_t uid);
int32_t HALSIM_GetRoboRioTeamNumber(void);
void HALSIM_SetRoboRioTeamNumber(int32_t teamNumber);

int32_t HALSIM_RegisterRoboRioSerialNumberCallback(
    HAL_RoboRioStringCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelRoboRioSerialNumberCallback(int32_t uid);
void HALSIM_GetRoboRioSerialNumber(struct WPI_String* serialNumber);
void HALSIM_SetRoboRioSerialNumber(const struct WPI_String* serialNumber);

int32_t HALSIM_RegisterRoboRioCommentsCallback(
    HAL_RoboRioStringCallback callback, void* param, HAL_Bool initialNotify);
void HALSIM_CancelRoboRioCommentsCallback(int32_t uid);
void HALSIM_GetRoboRioComments(struct WPI_String* comments);
void HALSIM_SetRoboRioComments(const struct WPI_String* comments);

int32_t HALSIM_RegisterRoboRioCPUTempCallback(HAL_NotifyCallback callback,
                                              void* param,
                                              HAL_Bool initialNotify);
void HALSIM_CancelRoboRioCPUTempCallback(int32_t uid);
double HALSIM_GetRoboRioCPUTemp(void);
void HALSIM_SetRoboRioCPUTemp(double cpuTemp);

void HALSIM_RegisterRoboRioAllCallbacks(HAL_NotifyCallback callback,
                                        void* param, HAL_Bool initialNotify);
#ifdef __cplusplus
}  // extern "C"
#endif
