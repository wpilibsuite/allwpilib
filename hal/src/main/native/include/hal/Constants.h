/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

/**
 * @defgroup hal_constants Constants Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Gets the number of FPGA system clock ticks per microsecond.
 *
 * @return the number of clock ticks per microsecond
 */
int32_t HAL_GetSystemClockTicksPerMicrosecond(void);
#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
