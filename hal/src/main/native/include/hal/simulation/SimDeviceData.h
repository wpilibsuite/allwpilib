/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "hal/Types.h"
#include "hal/Value.h"

typedef void (*HALSIM_SimDeviceCallback)(const char* name, void* param,
                                         HAL_SimDeviceHandle handle);

typedef void (*HALSIM_SimValueCallback)(const char* name, void* param,
                                        HAL_SimValueHandle handle,
                                        HAL_Bool readonly,
                                        const struct HAL_Value* value);

#ifdef __cplusplus
extern "C" {
#endif

void HALSIM_SetSimDeviceEnabled(const char* prefix, HAL_Bool enabled);
HAL_Bool HALSIM_IsSimDeviceEnabled(const char* name);

int32_t HALSIM_RegisterSimDeviceCreatedCallback(
    const char* prefix, void* param, HALSIM_SimDeviceCallback callback,
    HAL_Bool initialNotify);

void HALSIM_CancelSimDeviceCreatedCallback(int32_t uid);

int32_t HALSIM_RegisterSimDeviceFreedCallback(const char* prefix, void* param,
                                              HALSIM_SimDeviceCallback callback,
                                              HAL_Bool initialNotify);

void HALSIM_CancelSimDeviceFreedCallback(int32_t uid);

HAL_SimDeviceHandle HALSIM_GetSimDeviceHandle(const char* name);

const char* HALSIM_GetSimDeviceDisplayName(HAL_SimDeviceHandle handle);
void HALSIM_SetSimDeviceDisplayName(HAL_SimDeviceHandle handle,
                                    const char* displayName);
const char* HALSIM_GetSimDeviceName(HAL_SimDeviceHandle handle);

HAL_SimDeviceHandle HALSIM_GetSimValueDeviceHandle(HAL_SimValueHandle handle);

void HALSIM_EnumerateSimDevices(const char* prefix, void* param,
                                HALSIM_SimDeviceCallback callback);

int32_t HALSIM_RegisterSimValueCreatedCallback(HAL_SimDeviceHandle device,
                                               void* param,
                                               HALSIM_SimValueCallback callback,
                                               HAL_Bool initialNotify);

void HALSIM_CancelSimValueCreatedCallback(int32_t uid);

int32_t HALSIM_RegisterSimValueChangedCallback(HAL_SimValueHandle handle,
                                               void* param,
                                               HALSIM_SimValueCallback callback,
                                               HAL_Bool initialNotify);

void HALSIM_CancelSimValueChangedCallback(int32_t uid);

HAL_SimValueHandle HALSIM_GetSimValueHandle(HAL_SimDeviceHandle device,
                                            const char* name);

void HALSIM_EnumerateSimValues(HAL_SimDeviceHandle device, void* param,
                               HALSIM_SimValueCallback callback);

const char** HALSIM_GetSimValueEnumOptions(HAL_SimValueHandle handle,
                                           int32_t* numOptions);

void HALSIM_ResetSimDeviceData(void);

#ifdef __cplusplus
}  // extern "C"
#endif
