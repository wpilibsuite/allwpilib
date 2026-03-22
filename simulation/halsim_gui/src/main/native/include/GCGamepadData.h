// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#ifdef __APPLE__
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize GCController monitoring. Call once at startup.
 */
void GCGamepad_Init(void);

/**
 * Refresh the internal controller list from the current GCController snapshot.
 * Call once per update cycle from the main GUI thread.
 */
void GCGamepad_Refresh(void);

/**
 * Get the high-water mark of connected GCController gamepads.
 * This value only grows so that joystick slot indices remain stable.
 */
int GCGamepad_GetCount(void);
/**
 * Check if a GCController gamepad at the given index is connected.
 * @param index Index into the GCController array (0-based).
 * @return true if connected.
 */
bool GCGamepad_IsConnected(int index);
/**
 * Get the display name of a connected GCController gamepad.
 * @param index Index into the GCController array.
 * @return Controller name string, or nullptr if not connected.
 */
const char* GCGamepad_GetName(int index);
/**
 * Get a stable GUID string for the controller (for joystick matching).
 * @param index Index into the GCController array.
 * @return GUID string, or nullptr if not connected.
 */
const char* GCGamepad_GetGUID(int index);
/**
 * Read the current gamepad state mapped to FRC standard layout.
 *
 * Axes are mapped as:
 *   [0] Left X, [1] Left Y, [2] Left Trigger (0-1),
 *   [3] Right Trigger (0-1), [4] Right X, [5] Right Y
 *
 * Buttons are mapped in GLFW gamepad order:
 *   [0] A, [1] B, [2] X, [3] Y, [4] LB, [5] RB,
 *   [6] Back, [7] Start, [8] Guide, [9] L3, [10] R3
 *
 * @param index Index into the GCController array.
 * @param[out] axes Float array (at least 6 elements).
 * @param[out] axisCount Number of axes written.
 * @param[out] buttons Unsigned char array (at least 11 elements).
 * @param[out] buttonCount Number of buttons written.
 * @param[out] hatAngle D-pad angle (0/45/90/135/180/225/270/315) or -1.
 * @return true if data was successfully read.
 */
bool GCGamepad_GetState(int index, float* axes, int* axisCount,
                        unsigned char* buttons, int* buttonCount,
                        int* hatAngle);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // __APPLE__
