// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#ifdef __cplusplus
#include <cstddef>
#else

#include <stddef.h>  // NOLINT(build/include_order)

#endif

#include <wpi/string.h>

#include "hal/Types.h"

/**
 * @defgroup hal_capi WPILib HAL API
 * Hardware Abstraction Layer (HAL) to hardware or simulator
 * @{
 */

/**
 * Runtime type.
 */
HAL_ENUM(HAL_RuntimeType) {
  /** SystemCore runtime */
  HAL_Runtime_SystemCore,
  /** Simulation runtime */
  HAL_Runtime_Simulation,
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Gets the last error set on this thread, or the message for the status code.
 *
 * If passed HAL_USE_LAST_ERROR, the last error set on the thread will be
 * returned.
 *
 * @param[out] status the status code, set to the error status code if input is
 *               HAL_USE_LAST_ERROR
 * @return the error message for the code. This does not need to be freed,
 *               but can be overwritten by another hal call on the same thread.
 */
const char* HAL_GetLastError(int32_t* status);

/**
 * Gets the error message for a specific status code.
 *
 * @param code the status code
 * @return the error message for the code. This does not need to be freed.
 */
const char* HAL_GetErrorMessage(int32_t code);

/**
 * Returns the roboRIO serial number.
 *
 * @param[out] serialNumber The roboRIO serial number. Free with WPI_FreeString
 */
void HAL_GetSerialNumber(struct WPI_String* serialNumber);

/**
 * Returns the comments from the roboRIO web interface.
 *
 * @param[out] comments The comments string. Free with WPI_FreeString
 */
void HAL_GetComments(struct WPI_String* comments);

/**
 * Returns the team number configured for the robot controller.
 * @return team number, or 0 if not found.
 */
int32_t HAL_GetTeamNumber(void);

/**
 * Returns the runtime type of the HAL.
 *
 * @return HAL Runtime Type
 */
HAL_RuntimeType HAL_GetRuntimeType(void);

/**
 * Gets if the system outputs are currently active.
 *
 * @param[out] status the error code, or 0 for success
 * @return true if the system outputs are active, false if disabled
 */
HAL_Bool HAL_GetSystemActive(int32_t* status);

/**
 * Gets if the system is in a browned out state.
 *
 * @param[out] status the error code, or 0 for success
 * @return true if the system is in a low voltage brown out, false otherwise
 */
HAL_Bool HAL_GetBrownedOut(int32_t* status);

/**
 * Gets the number of times the system has been disabled due to communication
 * errors with the Driver Station.
 * @return number of disables due to communication errors.
 */
int32_t HAL_GetCommsDisableCount(int32_t* status);

/**
 * Reads the microsecond-resolution timer on the FPGA.
 *
 * @param[out] status the error code, or 0 for success
 * @return The current time in microseconds according to the FPGA (since FPGA
 * reset).
 */
uint64_t HAL_GetFPGATime(int32_t* status);

/**
 * Gets the current state of the Robot Signal Light (RSL).
 *
 * @param[out] status the error code, or 0 for success
 * @return The current state of the RSL- true if on, false if off
 */
HAL_Bool HAL_GetRSLState(int32_t* status);

/**
 * Gets if the system time is valid.
 *
 * @param[out] status the error code, or 0 for success
 * @return True if the system time is valid, false otherwise
 */
HAL_Bool HAL_GetSystemTimeValid(int32_t* status);

/**
 * Call this to start up HAL. This is required for robot programs.
 *
 * This must be called before any other HAL functions. Failure to do so will
 * result in undefined behavior, and likely segmentation faults. This means that
 * any statically initialized variables in a program MUST call this function in
 * their constructors if they want to use other HAL calls.
 *
 * The common parameters are 500 for timeout and 0 for mode.
 *
 * This function is safe to call from any thread, and as many times as you wish.
 * It internally guards from any reentrancy.
 *
 * The applicable modes are:
 *   0: Try to kill an existing HAL from another program, if not successful,
 * error.
 *   1: Force kill a HAL from another program.
 *   2: Just warn if another hal exists and cannot be killed. Will likely result
 * in undefined behavior.
 *
 * @param timeout the initialization timeout (ms)
 * @param mode    the initialization mode (see remarks)
 * @return true if initialization was successful, otherwise false.
 */
HAL_Bool HAL_Initialize(int32_t timeout, int32_t mode);

/**
 * Call this to shut down HAL.
 *
 * This must be called at termination of the robot program to avoid potential
 * segmentation faults with simulation extensions at exit.
 */
void HAL_Shutdown(void);

/**
 * Calls registered SimPeriodic "before" callbacks (only in simulation mode).
 * This should be called prior to user code periodic simulation functions.
 */
void HAL_SimPeriodicBefore(void);

/**
 * Calls registered SimPeriodic "after" callbacks (only in simulation mode).
 * This should be called after user code periodic simulation functions.
 */
void HAL_SimPeriodicAfter(void);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
