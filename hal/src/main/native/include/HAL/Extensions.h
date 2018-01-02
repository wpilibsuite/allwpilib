/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

/**
 * HAL Simulator Extensions are libraries that provide additional simulator
 * functionality, such as a Gazebo interface, or a more light weight simulation.
 *
 * An extension must expose the HALSIM_InitExtension entry point which is
 * invoked after the library is loaded.
 *
 * The entry point is expected to return < 0 for errors that should stop
 * the HAL completely, 0 for success, and > 0 for a non fatal error.
 */
typedef int halsim_extension_init_func_t(void);

extern "C" {
int HAL_LoadOneExtension(const char* library);
int HAL_LoadExtensions(void);
}  // extern "C"
