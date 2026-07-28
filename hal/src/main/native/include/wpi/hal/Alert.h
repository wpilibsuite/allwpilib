// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/Types.h"
#include "wpi/util/string.h"

/**
 * @defgroup hal_alert Alert Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** Represents an alert's level of urgency. */
HAL_ENUM(HAL_AlertLevel) {
  /**
   * High priority alert - displayed first with a red "X" symbol. Use this type
   * for problems which will seriously affect the robot's functionality and thus
   * require immediate attention.
   */
  HAL_ALERT_HIGH = 0,

  /**
   * Medium priority alert - displayed second with a yellow "!" symbol. Use this
   * type for problems which could affect the robot's functionality but do not
   * necessarily require immediate attention.
   */
  HAL_ALERT_MEDIUM = 1,

  /**
   * Low priority alert - displayed last with a green "i" symbol. Use this type
   * for problems which are unlikely to affect the robot's functionality, or any
   * other alerts which do not fall under the other categories.
   */
  HAL_ALERT_LOW = 2,
};

/**
 * Creates an alert.
 *
 * @param group Group identifier
 * @param text Text to be displayed when the alert is active
 * @param level Alert urgency level (HAL_AlertLevel)
 * @param[out] status Error status variable. 0 on success.
 * @return the created alert
 */
HAL_AlertHandle HAL_CreateAlert(const struct WPI_String* group,
                                const struct WPI_String* text, int32_t level,
                                int32_t* status);

/**
 * Destroys an alert.
 *
 * @param alertHandle the alert handle
 */
void HAL_DestroyAlert(HAL_AlertHandle alertHandle);

/**
 * Sets whether the alert should be displayed. This method can be
 * safely be called periodically.
 *
 * @param alertHandle the alert handle
 * @param active true to display the alert, false to hide it
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_SetAlertActive(HAL_AlertHandle alertHandle, HAL_Bool active,
                        int32_t* status);

/**
 * Checks if an alert is active.
 *
 * @param alertHandle the alert handle
 * @param[out] status Error status variable. 0 on success.
 * @return true if the alert is active
 */
HAL_Bool HAL_IsAlertActive(HAL_AlertHandle alertHandle, int32_t* status);

/**
 * Updates the text of an alert. Use this method to dynamically change the
 * displayed alert, such as including more details about the detected problem.
 *
 * @param alertHandle the alert handle
 * @param text new text to be displayed when the alert is active
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_SetAlertText(HAL_AlertHandle alertHandle,
                      const struct WPI_String* text, int32_t* status);

/**
 * Gets the text of an alert.
 *
 * @param alertHandle the alert handle
 * @param text pointer to a WPI_String to be filled with the current text
 * @param[out] status Error status variable. 0 on success.
 */
void HAL_GetAlertText(HAL_AlertHandle alertHandle, struct WPI_String* text,
                      int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
