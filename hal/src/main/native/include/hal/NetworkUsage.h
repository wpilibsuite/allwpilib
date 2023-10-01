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

void HAL_GetNetworkStatus(int32_t* rxBytes, int32_t* txBytes, int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
