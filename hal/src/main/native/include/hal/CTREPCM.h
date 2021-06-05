// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_ctre_pcm CTRE PCM Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

HAL_CTREPCMHandle HAL_InitializeCTREPCM(int32_t module,
                                        const char* allocationLocation,
                                        int32_t* status);
void HAL_FreeCTREPCM(HAL_CTREPCMHandle handle);

HAL_Bool HAL_CheckCTREPCMSolenoidChannel(int32_t channel);

HAL_Bool HAL_GetCTREPCMCompressor(HAL_CTREPCMHandle handle, int32_t* status);
void HAL_SetCTREPCMClosedLoopControl(HAL_CTREPCMHandle handle, HAL_Bool enabled,
                                     int32_t* status);
HAL_Bool HAL_GetCTREPCMClosedLoopControl(HAL_CTREPCMHandle handle,
                                         int32_t* status);
HAL_Bool HAL_GetCTREPCMPressureSwitch(HAL_CTREPCMHandle handle,
                                      int32_t* status);
double HAL_GetCTREPCMCompressorCurrent(HAL_CTREPCMHandle handle,
                                       int32_t* status);

HAL_Bool HAL_GetCTREPCMCompressorCurrentTooHighFault(HAL_CTREPCMHandle handle,
                                                     int32_t* status);
HAL_Bool HAL_GetCTREPCMCompressorCurrentTooHighStickyFault(
    HAL_CTREPCMHandle handle, int32_t* status);
HAL_Bool HAL_GetCTREPCMCompressorShortedStickyFault(HAL_CTREPCMHandle handle,
                                                    int32_t* status);
HAL_Bool HAL_GetCTREPCMCompressorShortedFault(HAL_CTREPCMHandle handle,
                                              int32_t* status);
HAL_Bool HAL_GetCTREPCMCompressorNotConnectedStickyFault(
    HAL_CTREPCMHandle handle, int32_t* status);
HAL_Bool HAL_GetCTREPCMCompressorNotConnectedFault(HAL_CTREPCMHandle handle,
                                                   int32_t* status);

int32_t HAL_GetCTREPCMSolenoids(HAL_CTREPCMHandle handle, int32_t* status);
void HAL_SetCTREPCMSolenoids(HAL_CTREPCMHandle handle, int32_t mask,
                             int32_t values, int32_t* status);

int32_t HAL_GetCTREPCMSolenoidDisabledList(HAL_CTREPCMHandle handle,
                                           int32_t* status);
HAL_Bool HAL_GetCTREPCMSolenoidVoltageStickyFault(HAL_CTREPCMHandle handle,
                                                  int32_t* status);
HAL_Bool HAL_GetCTREPCMSolenoidVoltageFault(HAL_CTREPCMHandle handle,
                                            int32_t* status);

void HAL_ClearAllCTREPCMStickyFaults(HAL_CTREPCMHandle handle, int32_t* status);

void HAL_FireCTREPCMOneShot(HAL_CTREPCMHandle handle, int32_t index,
                            int32_t* status);
void HAL_SetCTREPCMOneShotDuration(HAL_CTREPCMHandle handle, int32_t index,
                                   int32_t durMs, int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
