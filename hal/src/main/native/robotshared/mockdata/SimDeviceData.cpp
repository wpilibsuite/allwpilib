// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/simulation/SimDeviceData.h"

#include "hal/simulation/SimDataValue.h"

extern "C" {

void HALSIM_SetSimDeviceEnabled(const char* prefix, HAL_Bool enabled) {}

HAL_Bool HALSIM_IsSimDeviceEnabled(const char* name) {
  return false;
}

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

HAL_SimDeviceHandle HALSIM_GetSimDeviceHandle(const char* name) {
  return 0;
}

const char* HALSIM_GetSimDeviceName(HAL_SimDeviceHandle handle) {
  return "";
}

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

int32_t HALSIM_RegisterSimValueResetCallback(HAL_SimValueHandle handle,
                                             void* param,
                                             HALSIM_SimValueCallback callback,
                                             HAL_Bool initialNotify) {
  return 0;
}

void HALSIM_CancelSimValueResetCallback(int32_t uid) {}

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

const double* HALSIM_GetSimValueEnumDoubleValues(HAL_SimValueHandle handle,
                                                 int32_t* numOptions) {
  *numOptions = 0;
  return nullptr;
}

void HALSIM_ResetSimDeviceData(void) {}

}  // extern "C"
