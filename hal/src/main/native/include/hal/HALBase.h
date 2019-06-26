/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
 * The base HAL initialize function. Useful if you need to ensure the DS and
 * base HAL functions (the ones above this declaration in HAL.h) are properly
 * initialized. For normal programs and executables, please use HAL_Initialize.
 *
 * This is mainly expected to be use from libraries that are expected to be used
 * from LabVIEW, as it handles its own initialization for objects.
 */
void HAL_BaseInitialize(int32_t* status);

#ifndef HAL_USE_LABVIEW

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

// ifdef's definition is to allow for default parameters in C++.
#ifdef __cplusplus
/**
 * Reports a hardware usage to the HAL.
 *
 * @param resource       the used resource
 * @param instanceNumber the instance of the resource
 * @param context        a user specified context index
 * @param feature        a user specified feature string
 * @return               the index of the added value in NetComm
 */
int64_t HAL_Report(int32_t resource, int32_t instanceNumber,
                   int32_t context = 0, const char* feature = nullptr);
#else

/**
 * Reports a hardware usage to the HAL.
 *
 * @param resource       the used resource
 * @param instanceNumber the instance of the resource
 * @param context        a user specified context index
 * @param feature        a user specified feature string
 * @return               the index of the added value in NetComm
 */
int64_t HAL_Report(int32_t resource, int32_t instanceNumber, int32_t context,
                   const char* feature);
#endif

#endif  // HAL_USE_LABVIEW
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
