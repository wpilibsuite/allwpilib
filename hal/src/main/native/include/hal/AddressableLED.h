/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "hal/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

HAL_AddressableLEDHandle HAL_InitializeAddressableLED(HAL_DigitalHandle outputPort, int32_t* status);
void HAL_FreeAddressableLED(HAL_AddressableLEDHandle handle);

void HAL_WriteAddressableLEDStringLength(HAL_AddressableLEDHandle handle, int32_t value, int32_t* status);

void HAL_SetAddressableLEDTiming(HAL_AddressableLEDHandle handle, int32_t highTime0, int32_t lowTime0, int32_t highTime1, int32_t lowTime1, int32_t reset, int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
