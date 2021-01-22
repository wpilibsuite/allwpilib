// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/Types.h"

/**
 * @defgroup hal_extensions Simulator Extensions
 * @ingroup hal_capi
 * HAL Simulator Extensions.  These are libraries that provide additional
 * simulator functionality, such as a Gazebo interface, or a more light weight
 * simulation.
 *
 * An extension must expose the HALSIM_InitExtension entry point which is
 * invoked after the library is loaded.
 *
 * The entry point is expected to return < 0 for errors that should stop
 * the HAL completely, 0 for success, and > 0 for a non fatal error.
 * @{
 */
typedef int halsim_extension_init_func_t(void);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Loads a single extension from a direct path.
 *
 * Expected to be called internally, not by users.
 *
 * @param library the library path
 * @return        the succes state of the initialization
 */
int HAL_LoadOneExtension(const char* library);

/**
 * Loads any extra halsim libraries provided in the HALSIM_EXTENSIONS
 * environment variable.
 *
 * @return        the succes state of the initialization
 */
int HAL_LoadExtensions(void);

/**
 * Registers an extension such that other extensions can discover it.
 *
 * The passed data pointer is retained and the extension must keep this
 * pointer valid.
 *
 * @param name extension name (may embed version number)
 * @param data data pointer
 */
void HAL_RegisterExtension(const char* name, void* data);

/**
 * Registers an extension registration listener function. The function will
 * be called immediately with any currently registered extensions, and will
 * be called later when any additional extensions are registered.
 *
 * @param param parameter data to pass to callback function
 * @param func callback function to be called for each registered extension;
 *             parameters are the parameter data, extension name, and extension
 *             data pointer passed to HAL_RegisterExtension()
 */
void HAL_RegisterExtensionListener(void* param,
                                   void (*func)(void*, const char* name,
                                                void* data));

/**
 * Enables or disables the message saying no HAL extensions were found.
 *
 * Some apps don't care, and the message create clutter. For general team code,
 * we want it.
 *
 * This must be called before HAL_Initialize is called.
 *
 * This defaults to true.
 *
 * @param showMessage true to show message, false to not.
 */
void HAL_SetShowExtensionsNotFoundMessages(HAL_Bool showMessage);

/**
 * Registers a function to be called from HAL_Shutdown(). This is intended
 * for use only by simulation extensions.
 *
 * @param param parameter data to pass to callback function
 * @param func callback function
 */
void HAL_OnShutdown(void* param, void (*func)(void*));

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
