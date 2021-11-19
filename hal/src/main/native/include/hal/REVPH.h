// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_rev_ph REV PH Functions
 * @ingroup hal_capi
 * @{
 */

/**
 * The compressor configuration type
 */
HAL_ENUM(HAL_REVPHCompressorConfigType){
    HAL_REVPHCompressorConfigType_kDisabled = 0,
    HAL_REVPHCompressorConfigType_kDigital = 1,
    HAL_REVPHCompressorConfigType_kAnalog = 2,
    HAL_REVPHCompressorConfigType_kHybrid = 3,
};

/**
 * Storage for compressor config
 */
struct HAL_REVPHCompressorConfig {
    double minAnalogVoltage;
    double maxAnalogVoltage;
    HAL_Bool forceDisable;
    HAL_Bool useDigital;
};

#ifdef __cplusplus
extern "C" {
#endif

HAL_REVPHHandle HAL_InitializeREVPH(int32_t module,
                                    const char* allocationLocation,
                                    int32_t* status);

void HAL_FreeREVPH(HAL_REVPHHandle handle);

HAL_Bool HAL_CheckREVPHSolenoidChannel(int32_t channel);
HAL_Bool HAL_CheckREVPHModuleNumber(int32_t module);

HAL_Bool HAL_GetREVPHCompressor(HAL_REVPHHandle handle, int32_t* status);
void HAL_SetREVPHClosedLoopControl(HAL_REVPHHandle handle, HAL_Bool enabled,
                                   int32_t* status);
HAL_Bool HAL_GetREVPHClosedLoopControl(HAL_REVPHHandle handle, int32_t* status);
void HAL_SetREVPHCompressorConfig(HAL_REVPHHandle handle,
                                  HAL_REVPHCompressorConfig config,
                                  int32_t* status);
void HAL_SetREVPHClosedLoopControlDisabled(HAL_REVPHHandle handle,
                                           int32_t* status);
void HAL_SetREVPHClosedLoopControlDigital(HAL_REVPHHandle handle,
                                          int32_t* status);
void HAL_SetREVPHClosedLoopControlAnalog(HAL_REVPHHandle handle,
                                         double minAnalogVoltage,
                                         double maxAnalogVoltage,
                                         int32_t* status);
void HAL_SetREVPHClosedLoopControlHybrid(HAL_REVPHHandle handle,
                                         double minAnalogVoltage,
                                         double maxAnalogVoltage,
                                         int32_t* status);
HAL_REVPHCompressorConfigType HAL_GetREVPHCompressorConfig(
    HAL_REVPHHandle handle, int32_t* status);
HAL_Bool HAL_GetREVPHPressureSwitch(HAL_REVPHHandle handle, int32_t* status);
double HAL_GetREVPHCompressorCurrent(HAL_REVPHHandle handle, int32_t* status);
double HAL_GetREVPHAnalogPressure(HAL_REVPHHandle handle, int32_t channel,
                                  int32_t* status);

int32_t HAL_GetREVPHSolenoids(HAL_REVPHHandle handle, int32_t* status);
void HAL_SetREVPHSolenoids(HAL_REVPHHandle handle, int32_t mask, int32_t values,
                           int32_t* status);

void HAL_FireREVPHOneShot(HAL_REVPHHandle handle, int32_t index, int32_t durMs,
                          int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
