// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/Types.h"
#include "wpi/util/string.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Information about an alert. */
struct HALSIM_AlertInfo {
  HAL_AlertHandle handle;
  struct WPI_String group;
  struct WPI_String text;
  int64_t activeStartTime;  // 0 if not active
  int32_t level;            // HAL_AlertLevel
};

/**
 * Gets the number of alerts. Note: this is not guaranteed to be consistent
 * with the number of alerts returned by HALSIM_GetAlerts, so the latter's
 * return value should be used to determine how many alerts were actually filled
 * in.
 *
 * @return the number of alerts
 */
int32_t HALSIM_GetNumAlerts(void);

/**
 * Gets detailed information about each alert.
 *
 * @param arr array of information to be filled
 * @param length length of arr
 * @return Number of alerts; note: may be larger or smaller than passed-in
 * length
 */
int32_t HALSIM_GetAlerts(struct HALSIM_AlertInfo* arr, int32_t length);

/**
 * Frees an array of alert information returned by HALSIM_GetAlerts.
 *
 * @param arr array to free
 * @param length number of alerts in arr (as returned by HALSIM_GetAlerts)
 */
void HALSIM_FreeAlerts(struct HALSIM_AlertInfo* arr, int32_t length);

/**
 * Resets all alert simulation data.
 */
void HALSIM_ResetAlertData(void);

#ifdef __cplusplus
}  // extern "C"
#endif
