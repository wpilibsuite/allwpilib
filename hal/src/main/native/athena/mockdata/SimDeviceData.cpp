/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/simulation/SimDeviceData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {

void HALSIM_SetSimDeviceEnabled(const char* prefix, HAL_Bool enabled) {}

HAL_Bool HALSIM_IsSimDeviceEnabled(const char* name) { return false; }

int32_t HALSIM_RegisterSimDeviceCreatedCallback(
    const char* prefix, void* param, HALSIM_SimDeviceCallback callback,
    HAL_Bool initialNotify) {
  return 0;
}

void HALSIM_CancelSimDeviceCreatedCallback(int32_t uid) {}

int32_t HALSIM_RegisterSimDeviceFreedCallback(const char* prefix, void* param,
                                              HALSIM_SimDeviceCallback callback,
                                              HAL_Bool initialNotify) {
  return 0;
}

void HALSIM_CancelSimDeviceFreedCallback(int32_t uid) {}

HAL_SimDeviceHandle HALSIM_GetSimDeviceHandle(const char* name) { return 0; }

const char* HALSIM_GetSimDeviceName(HAL_SimDeviceHandle handle) { return ""; }

const char* HALSIM_GetSimDeviceDisplayName(HAL_SimDeviceHandle handle) {
  return "";
}
void HALSIM_SetSimDeviceDisplayName(HAL_SimDeviceHandle handle,
                                    const char* displayName) {}

HAL_SimDeviceHandle HALSIM_GetSimValueDeviceHandle(HAL_SimValueHandle handle) {
  return 0;
}

void HALSIM_EnumerateSimDevices(const char* prefix, void* param,
                                HALSIM_SimDeviceCallback callback) {}

int32_t HALSIM_RegisterSimValueCreatedCallback(HAL_SimDeviceHandle device,
                                               void* param,
                                               HALSIM_SimValueCallback callback,
                                               HAL_Bool initialNotify) {
  return 0;
}

void HALSIM_CancelSimValueCreatedCallback(int32_t uid) {}

int32_t HALSIM_RegisterSimValueChangedCallback(HAL_SimValueHandle handle,
                                               void* param,
                                               HALSIM_SimValueCallback callback,
                                               HAL_Bool initialNotify) {
  return 0;
}

void HALSIM_CancelSimValueChangedCallback(int32_t uid) {}

HAL_SimValueHandle HALSIM_GetSimValueHandle(HAL_SimDeviceHandle device,
                                            const char* name) {
  return 0;
}

void HALSIM_EnumerateSimValues(HAL_SimDeviceHandle device, void* param,
                               HALSIM_SimValueCallback callback) {}

const char** HALSIM_GetSimValueEnumOptions(HAL_SimValueHandle handle,
                                           int32_t* numOptions) {
  *numOptions = 0;
  return nullptr;
}

void HALSIM_ResetSimDeviceData(void) {}

}  // extern "C"
