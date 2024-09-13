// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_ctre_pcm CTRE Pneumatic Control Module (PCM) Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a PCM.
 *
 * @param[in] module             the CAN ID to initialize
 * @param[in] allocationLocation the location where the allocation is occurring
 *                               (can be null)
 * @param[out] status            Error status variable. 0 on success.
 * @return the created PH handle
 */
HAL_CTREPCMHandle HAL_InitializeCTREPCM(int32_t module,
                                        const char* allocationLocation,
                                        int32_t* status);

/**
 * Frees a PCM handle.
 *
 * @param[in] handle the PCMhandle
 */
void HAL_FreeCTREPCM(HAL_CTREPCMHandle handle);

/**
 * Checks if a solenoid channel number is valid.
 *
 * @param[in] channel the channel to check
 * @return true if the channel is valid, otherwise false
 */
HAL_Bool HAL_CheckCTREPCMSolenoidChannel(int32_t channel);

/**
 * Get whether compressor is turned on.
 *
 * @param[in] handle  the PCM handle
 * @param[out] status Error status variable. 0 on success.
 * @return true if the compressor is turned on
 */
HAL_Bool HAL_GetCTREPCMCompressor(HAL_CTREPCMHandle handle, int32_t* status);

/**
 * Enables the compressor closed loop control using the digital pressure switch.
 * The compressor will turn on when the pressure switch indicates that the
 * system is not full, and will turn off when the pressure switch indicates that
 * the system is full.
 *
 * @param[in] handle  the PCM handle
 * @param[in] enabled true to enable closed loop control
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_SetCTREPCMClosedLoopControl(HAL_CTREPCMHandle handle, HAL_Bool enabled,
                                     int32_t* status);

/**
 * Get whether the PCM closed loop control is enabled.
 *
 * @param[in] handle  the PCM handle
 * @param[out] status Error status variable. 0 on success.
 * @return True if closed loop control is enabled, otherwise false.
 */
HAL_Bool HAL_GetCTREPCMClosedLoopControl(HAL_CTREPCMHandle handle,
                                         int32_t* status);

/**
 * Returns the state of the pressure switch.
 *
 * @param[in] handle  the PCM handle
 * @param[out] status Error status variable. 0 on success.
 * @return True if pressure switch indicates that the system is full,
 * otherwise false.
 */
HAL_Bool HAL_GetCTREPCMPressureSwitch(HAL_CTREPCMHandle handle,
                                      int32_t* status);

/**
 * Returns the current drawn by the compressor.
 *
 * @param[in] handle  the PCM handle
 * @param[out] status Error status variable. 0 on success.
 * @return The current drawn by the compressor in amps.
 */
double HAL_GetCTREPCMCompressorCurrent(HAL_CTREPCMHandle handle,
                                       int32_t* status);

/**
 * Return whether the compressor current is currently too high.
 *
 * @param[in] handle  the PCM handle
 * @param[out] status Error status variable. 0 on success.
 * @return True if the compressor current is too high, otherwise false.
 * @see HAL_GetCTREPCMCompressorShortedStickyFault
 */
HAL_Bool HAL_GetCTREPCMCompressorCurrentTooHighFault(HAL_CTREPCMHandle handle,
                                                     int32_t* status);

/**
 * Returns whether the compressor current has been too high since sticky
 * faults were last cleared. This fault is persistent and can be cleared by
 * HAL_ClearAllCTREPCMStickyFaults()
 *
 * @param[in] handle  the PCM handle
 * @param[out] status Error status variable. 0 on success.
 * @return True if the compressor current has been too high since sticky
 * faults were last cleared.
 * @see HAL_GetCTREPCMCompressorCurrentTooHighFault()
 */
HAL_Bool HAL_GetCTREPCMCompressorCurrentTooHighStickyFault(
    HAL_CTREPCMHandle handle, int32_t* status);

/**
 * Returns whether the compressor has been shorted since sticky faults were
 * last cleared. This fault is persistent and can be cleared by
 * HAL_ClearAllCTREPCMStickyFaults()
 *
 * @param[in] handle  the PCM handle
 * @param[out] status Error status variable. 0 on success.
 * @return True if the compressor has been shorted since sticky faults were
 * last cleared, otherwise false.
 * @see HAL_GetCTREPCMCompressorShortedFault()
 */
HAL_Bool HAL_GetCTREPCMCompressorShortedStickyFault(HAL_CTREPCMHandle handle,
                                                    int32_t* status);

/**
 * Returns whether the compressor is currently shorted.
 *
 * @param[in] handle  the PCM handle
 * @param[out] status Error status variable. 0 on success.
 * @return True if the compressor is currently shorted, otherwise false.
 * @see HAL_GetCTREPCMCompressorShortedStickyFault
 */
HAL_Bool HAL_GetCTREPCMCompressorShortedFault(HAL_CTREPCMHandle handle,
                                              int32_t* status);

/**
 * Returns whether the compressor has been disconnected since sticky faults
 * were last cleared. This fault is persistent and can be cleared by
 * HAL_ClearAllCTREPCMStickyFaults()
 *
 * @param[in] handle  the PCM handle
 * @param[out] status Error status variable. 0 on success.
 * @return True if the compressor has been disconnected since sticky faults
 * were last cleared, otherwise false.
 * @see HAL_GetCTREPCMCompressorShortedFault()
 */
HAL_Bool HAL_GetCTREPCMCompressorNotConnectedStickyFault(
    HAL_CTREPCMHandle handle, int32_t* status);

/**
 * Returns whether the compressor is currently disconnected.
 *
 * @param[in] handle  the PCM handle
 * @param[out] status Error status variable. 0 on success.
 * @return True if compressor is currently disconnected, otherwise false.
 * @see HAL_GetCTREPCMCompressorNotConnectedStickyFault()
 */
HAL_Bool HAL_GetCTREPCMCompressorNotConnectedFault(HAL_CTREPCMHandle handle,
                                                   int32_t* status);

/**
 * Gets a bitmask of solenoid values.
 *
 * @param[in] handle  the PCM handle
 * @param[out] status Error status variable. 0 on success.
 * @return solenoid values
 */
int32_t HAL_GetCTREPCMSolenoids(HAL_CTREPCMHandle handle, int32_t* status);

/**
 * Sets solenoids on a pneumatics module.
 *
 * @param[in] handle  the PCM handle
 * @param[in] mask bitmask to set
 * @param[in] values solenoid values
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_SetCTREPCMSolenoids(HAL_CTREPCMHandle handle, int32_t mask,
                             int32_t values, int32_t* status);

/**
 * Get a bitmask of disabled solenoids.
 *
 * @param[in] handle  the PCM handle
 * @param[out] status Error status variable. 0 on success.
 * @return bitmask of disabled solenoids
 */
int32_t HAL_GetCTREPCMSolenoidDisabledList(HAL_CTREPCMHandle handle,
                                           int32_t* status);

/**
 * Returns whether the solenoid has reported a voltage fault since sticky faults
 * were last cleared. This fault is persistent and can be cleared by
 * HAL_ClearAllCTREPCMStickyFaults()
 *
 * @param[in] handle  the PCM handle
 * @param[out] status Error status variable. 0 on success.
 * @return True if solenoid is reporting a fault, otherwise false.
 * @see HAL_GetCTREPCMSolenoidVoltageFault()
 */
HAL_Bool HAL_GetCTREPCMSolenoidVoltageStickyFault(HAL_CTREPCMHandle handle,
                                                  int32_t* status);
/**
 * Returns whether the solenoid is currently reporting a voltage fault.
 *
 * @param[in] handle  the PCM handle
 * @param[out] status Error status variable. 0 on success.
 * @return True if solenoid is reporting a fault, otherwise false.
 * @see HAL_GetCTREPCMSolenoidVoltageStickyFault()
 */
HAL_Bool HAL_GetCTREPCMSolenoidVoltageFault(HAL_CTREPCMHandle handle,
                                            int32_t* status);

/**
 * Clears all sticky faults on this device.
 *
 * @param[in] handle  the PCM handle
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_ClearAllCTREPCMStickyFaults(HAL_CTREPCMHandle handle, int32_t* status);

/**
 * Fire a single solenoid shot.
 *
 * @param[in] handle  the PCM handle
 * @param[in] index solenoid index
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_FireCTREPCMOneShot(HAL_CTREPCMHandle handle, int32_t index,
                            int32_t* status);

/**
 * Set the duration for a single solenoid shot.
 *
 * @param[in] handle  the PCM handle
 * @param[in] index solenoid index
 * @param[in] durMs shot duration in ms
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_SetCTREPCMOneShotDuration(HAL_CTREPCMHandle handle, int32_t index,
                                   int32_t durMs, int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
