// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/util/Handle.h"
#include "wpi/util/string.h"

typedef WPI_Handle WPI_AlertHandle;

#ifdef __cplusplus
extern "C" {
#endif

/** Represents an alert's level of urgency. */
enum WPI_AlertLevel {
  /**
   * High priority alert - displayed first with a red "X" symbol. Use this type
   * for problems which will seriously affect the robot's functionality and thus
   * require immediate attention.
   */
  WPI_ALERT_HIGH = 0,

  /**
   * Medium priority alert - displayed second with a yellow "!" symbol. Use this
   * type for problems which could affect the robot's functionality but do not
   * necessarily require immediate attention.
   */
  WPI_ALERT_MEDIUM = 1,

  /**
   * Low priority alert - displayed last with a green "i" symbol. Use this type
   * for problems which are unlikely to affect the robot's functionality, or any
   * other alerts which do not fall under the other categories.
   */
  WPI_ALERT_LOW = 2,
};

/** Information about an alert. */
struct WPI_AlertInfo {
  struct WPI_String group;
  struct WPI_String id;
  struct WPI_String text;
  int64_t activeStartTime;  // 0 if not active
  int32_t level;            // WPI_AlertLevel
};

struct WPI_AlertBackend {
  int32_t (*createAlert)(const struct WPI_String* group,
                         const struct WPI_String* id,
                         const struct WPI_String* text, int32_t level,
                         WPI_AlertHandle* handle);
  void (*destroyAlert)(WPI_AlertHandle alertHandle);
  int32_t (*setAlertActive)(WPI_AlertHandle alertHandle, int32_t active);
  int32_t (*isAlertActive)(WPI_AlertHandle alertHandle, int32_t* active);
  int32_t (*setAlertText)(WPI_AlertHandle alertHandle,
                          const struct WPI_String* text);
  int32_t (*getAlertText)(WPI_AlertHandle alertHandle, struct WPI_String* text);
  int32_t (*getAlertLevel)(WPI_AlertHandle alertHandle, int32_t* level);
  int32_t (*getNumAlerts)(void);
  int32_t (*getAlerts)(struct WPI_AlertInfo* arr, int32_t length);
  void (*freeAlerts)(struct WPI_AlertInfo* arr, int32_t length);
  void (*resetAlertData)(void);
};

/**
 * Creates an alert.
 *
 * @param group Group identifier
 * @param id Alert identifier. This must be unique within the group and level.
 * @param text Text to be displayed when the alert is active
 * @param level Alert urgency level (WPI_AlertLevel)
 * @param[out] handle the new alert. If an alert with the same group and id
 * already exists, this function will return an error status.
 * @return Error status. 0 on success.
 */
int32_t WPI_CreateAlert(const struct WPI_String* group,
                        const struct WPI_String* id,
                        const struct WPI_String* text, int32_t level,
                        WPI_AlertHandle* handle);

/**
 * Destroys an alert.
 *
 * @param alertHandle the alert handle
 */
void WPI_DestroyAlert(WPI_AlertHandle alertHandle);

/**
 * Sets whether the alert should be displayed. This method can be
 * safely be called periodically.
 *
 * @param alertHandle the alert handle
 * @param active true to display the alert, false to hide it
 * @return Error status. 0 on success.
 */
int32_t WPI_SetAlertActive(WPI_AlertHandle alertHandle, int32_t active);

/**
 * Checks if an alert is active.
 *
 * @param alertHandle the alert handle
 * @param[out] active true if the alert is active, false otherwise
 * @return Error status. 0 on success.
 */
int32_t WPI_IsAlertActive(WPI_AlertHandle alertHandle, int32_t* active);

/**
 * Updates the text of an alert. Use this method to dynamically change the
 * displayed alert, such as including more details about the detected problem.
 *
 * @param alertHandle the alert handle
 * @param text new text to be displayed when the alert is active
 * @return Error status. 0 on success.
 */
int32_t WPI_SetAlertText(WPI_AlertHandle alertHandle,
                         const struct WPI_String* text);

/**
 * Gets the text of an alert.
 *
 * @param alertHandle the alert handle
 * @param text pointer to a WPI_String to be filled with the current text
 * @return Error status. 0 on success.
 */
int32_t WPI_GetAlertText(WPI_AlertHandle alertHandle, struct WPI_String* text);

/**
 * Gets the level of an alert.
 *
 * @param alertHandle the alert handle
 * @param[out] level pointer to an int32_t to be filled with the alert's urgency
 * level (WPI_AlertLevel)
 * @return Error status. 0 on success.
 */
int32_t WPI_GetAlertLevel(WPI_AlertHandle alertHandle, int32_t* level);

/**
 * Gets the number of alerts. Note: this is not guaranteed to be consistent
 * with the number of alerts returned by WPI_GetAlerts, so the latter's
 * return value should be used to determine how many alerts were actually filled
 * in.
 *
 * @return the number of alerts
 */
int32_t WPI_GetNumAlerts(void);

/**
 * Gets detailed information about all alerts.
 *
 * @param arr array of information to be filled
 * @param length length of arr
 * @return Number of alerts; note: may be larger or smaller than passed-in
 * length
 */
int32_t WPI_GetAlerts(struct WPI_AlertInfo* arr, int32_t length);

/**
 * Frees an array of alert information returned by WPI_GetAlerts.
 *
 * @param arr array to free
 * @param length number of alerts in arr (as returned by WPI_GetAlerts)
 */
void WPI_FreeAlerts(struct WPI_AlertInfo* arr, int32_t length);

/**
 * Resets all alerts in the alert backend.
 */
void WPI_ResetAlertData(void);

/**
 * Sets the alert backend.
 *
 * @param backend pointer to the alert backend
 */
void WPI_SetAlertBackend(const struct WPI_AlertBackend* backend);

/**
 * Gets the alert backend.
 *
 * @return pointer to the alert backend
 */
const struct WPI_AlertBackend* WPI_GetAlertBackend(void);

#ifdef __cplusplus
}  // extern "C"
#endif
