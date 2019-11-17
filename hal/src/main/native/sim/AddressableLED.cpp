/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/AddressableLED.h"

extern "C" {
HAL_AddressableLEDHandle HAL_InitializeAddressableLED(
    HAL_DigitalHandle outputPort, int32_t* status) {
  return HAL_kInvalidHandle;
}

void HAL_FreeAddressableLED(HAL_AddressableLEDHandle handle) {}

void HAL_SetAddressableLEDOutputPort(HAL_AddressableLEDHandle handle,
                                     HAL_DigitalHandle outputPort,
                                     int32_t* status) {}

void HAL_SetAddressableLEDLength(HAL_AddressableLEDHandle handle,
                                 int32_t length, int32_t* status) {}

void HAL_WriteAddressableLEDData(HAL_AddressableLEDHandle handle,
                                 const struct HAL_AddressableLEDData* data,
                                 int32_t length, int32_t* status) {}

void HAL_SetAddressableLEDBitTiming(HAL_AddressableLEDHandle handle,
                                    int32_t lowTime0NanoSeconds,
                                    int32_t highTime0NanoSeconds,
                                    int32_t lowTime1NanoSeconds,
                                    int32_t highTime1NanoSeconds,
                                    int32_t* status) {}

void HAL_SetAddressableLEDSyncTime(HAL_AddressableLEDHandle handle,
                                   int32_t syncTimeMicroSeconds,
                                   int32_t* status) {}

void HAL_StartAddressableLEDOutput(HAL_AddressableLEDHandle handle,
                                   int32_t* status) {}

void HAL_StopAddressableLEDOutput(HAL_AddressableLEDHandle handle,
                                  int32_t* status) {}
}  // extern "C"
