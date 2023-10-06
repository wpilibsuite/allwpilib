// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

/**
 * @defgroup hal_net Network Usage Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get the current status of the RIO network interface.
 *
 * @param[out] rxBytes Amount of bytes received over the RIO network interface
 * @param[out] txBytes Amount of bytes transmitted over the RIO network interface
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_GetNetworkStatus(int32_t* rxBytes, int32_t* txBytes, int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
