/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "hal/Types.h"

/**
 * @defgroup hal_relay Main loop functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Sets up the system to run the provided main loop in the main thread (e.g.
 * the thread in which main() starts execution) and run the robot code in a
 * separate thread.
 *
 * Normally the robot code runs in the main thread, but some GUI systems
 * require the GUI be run in the main thread.
 *
 * To be effective, this function must be called before the robot code starts
 * the main loop (e.g. by frc::StartRobot()).
 *
 * @param param parameter data to pass to mainFunc and exitFunc
 * @param mainFunc the function to be run when HAL_RunMain() is called.
 * @param exitFunc the function to be run when HAL_ExitMain() is called.
 */
void HAL_SetMain(void* param, void (*mainFunc)(void*), void (*exitFunc)(void*));

/**
 * Returns true if HAL_SetMain() has been called.
 *
 * @return True if HAL_SetMain() has been called, false otherwise.
 */
HAL_Bool HAL_HasMain(void);

/**
 * Runs the main function provided to HAL_SetMain().
 *
 * If HAL_SetMain() has not been called, simply sleeps until HAL_ExitMain()
 * is called.
 */
void HAL_RunMain(void);

/**
 * Causes HAL_RunMain() to exit.
 *
 * If HAL_SetMain() has been called, this calls the exit function provided
 * to that function.
 */
void HAL_ExitMain(void);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
