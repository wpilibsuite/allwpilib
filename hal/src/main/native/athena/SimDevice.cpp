/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/SimDevice.h"

extern "C" {

HAL_SimDeviceHandle HAL_CreateSimDevice(const char* name) { return 0; }

void HAL_FreeSimDevice(HAL_SimDeviceHandle handle) {}

HAL_SimValueHandle HAL_CreateSimValue(HAL_SimDeviceHandle device,
                                      const char* name, int32_t direction,
                                      const struct HAL_Value* initialValue) {
  return 0;
}

HAL_SimValueHandle HAL_CreateSimValueEnum(HAL_SimDeviceHandle device,
                                          const char* name, int32_t direction,
                                          int32_t numOptions,
                                          const char** options,
                                          int32_t initialValue) {
  return 0;
}

HAL_SimValueHandle HAL_CreateSimValueEnumDouble(
    HAL_SimDeviceHandle device, const char* name, int32_t direction,
    int32_t numOptions, const char** options, const double* optionValues,
    int32_t initialValue) {
  return 0;
}

void HAL_GetSimValue(HAL_SimValueHandle handle, struct HAL_Value* value) {
  value->type = HAL_UNASSIGNED;
}

void HAL_SetSimValue(HAL_SimValueHandle handle, const struct HAL_Value* value) {
}

hal::SimDevice::SimDevice(const char* name, int index) {}

hal::SimDevice::SimDevice(const char* name, int index, int channel) {}

}  // extern "C"
