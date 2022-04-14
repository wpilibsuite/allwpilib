// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
