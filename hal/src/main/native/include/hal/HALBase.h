// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_capi WPILib HAL API
 * Hardware Abstraction Layer to hardware or simulator
 * @{
 */

// clang-format off
HAL_ENUM(HAL_RuntimeType) { HAL_Athena, HAL_Mock };
// clang-format on

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Gets the error message for a specific status code.
 *
 * @param code the status code
 * @return     the error message for the code. This does not need to be freed.
 */
const char* HAL_GetErrorMessage(int32_t code);

/**
 * Returns the FPGA Version number.
 *
 * For now, expect this to be competition year.
 *
 * @return FPGA Version number.
 */
int32_t HAL_GetFPGAVersion(int32_t* status);

/**
 * Returns the FPGA Revision number.
 *
 * The format of the revision is 3 numbers.
 * The 12 most significant bits are the Major Revision.
 * the next 8 bits are the Minor Revision.
 * The 12 least significant bits are the Build Number.
 *
 * @return FPGA Revision number.
 */
int64_t HAL_GetFPGARevision(int32_t* status);

HAL_RuntimeType HAL_GetRuntimeType(void);

/**
 * Gets the state of the "USER" button on the roboRIO.
 *
 * @return true if the button is currently pressed down
 */
HAL_Bool HAL_GetFPGAButton(int32_t* status);

/**
 * Gets if the system outputs are currently active
 *
 * @return true if the system outputs are active, false if disabled
 */
HAL_Bool HAL_GetSystemActive(int32_t* status);

/**
 * Gets if the system is in a browned out state.
 *
 * @return true if the system is in a low voltage brown out, false otherwise
 */
HAL_Bool HAL_GetBrownedOut(int32_t* status);

/**
 * Gets a port handle for a specific channel.
 *
 * The created handle does not need to be freed.
 *
 * @param channel the channel number
 * @return        the created port
 */
HAL_PortHandle HAL_GetPort(int32_t channel);

/**
 * Gets a port handle for a specific channel and module.
 *
 * This is expected to be used for PCMs, as the roboRIO does not work with
 * modules anymore.
 *
 * The created handle does not need to be freed.
 *
 * @param module  the module number
 * @param channel the channel number
 * @return        the created port
 */
HAL_PortHandle HAL_GetPortWithModule(int32_t module, int32_t channel);

/**
 * Reads the microsecond-resolution timer on the FPGA.
 *
 * @return The current time in microseconds according to the FPGA (since FPGA
 * reset).
 */
uint64_t HAL_GetFPGATime(int32_t* status);

/**
 * Given an 32 bit FPGA time, expand it to the nearest likely 64 bit FPGA time.
 *
 * Note: This is making the assumption that the timestamp being converted is
 * always in the past.  If you call this with a future timestamp, it probably
 * will make it in the past.  If you wait over 70 minutes between capturing the
 * bottom 32 bits of the timestamp and expanding it, you will be off by
 * multiples of 1<<32 microseconds.
 *
 * @return The current time in microseconds according to the FPGA (since FPGA
 * reset) as a 64 bit number.
 */
uint64_t HAL_ExpandFPGATime(uint32_t unexpanded_lower, int32_t* status);

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
 * @return        true if initialization was successful, otherwise false.
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
