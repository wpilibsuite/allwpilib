// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "hal/DriverStationTypes.h"
#include "hal/Types.h"

/**
 * @defgroup hal_driverstation Driver Station Functions
 * @ingroup hal_capi
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Sends an error to the driver station.
 *
 * @param isError   true for error, false for warning
 * @param errorCode the error code
 * @param isLVCode  true for a LV error code, false for a standard error code
 * @param details   the details of the error
 * @param location  the file location of the errror
 * @param callstack the callstack of the error
 * @param printMsg  true to print the error message to stdout as well as to the
 * DS
 */
int32_t HAL_SendError(HAL_Bool isError, int32_t errorCode, HAL_Bool isLVCode,
                      const char* details, const char* location,
                      const char* callStack, HAL_Bool printMsg);
/**
 * Sends a line to the driver station console.
 *
 * @param line the line to send (null terminated)
 */
int32_t HAL_SendConsoleLine(const char* line);

/**
 * Gets the current control word of the driver station.
 *
 * The control work contains the robot state.
 *
 * @param controlWord the control word (out)
 * @return            the error code, or 0 for success
 */
int32_t HAL_GetControlWord(HAL_ControlWord* controlWord);

/**
 * Gets the current alliance station ID.
 *
 * @param status the error code, or 0 for success
 * @return       the alliance station ID
 */
HAL_AllianceStationID HAL_GetAllianceStation(int32_t* status);

/**
 * Gets the axes of a specific joystick.
 *
 * @param joystickNum the joystick number
 * @param axes        the axes values (output)
 * @return            the error code, or 0 for success
 */
int32_t HAL_GetJoystickAxes(int32_t joystickNum, HAL_JoystickAxes* axes);

/**
 * Gets the POVs of a specific joystick.
 *
 * @param joystickNum the joystick number
 * @param povs        the POV values (output)
 * @return            the error code, or 0 for success
 */
int32_t HAL_GetJoystickPOVs(int32_t joystickNum, HAL_JoystickPOVs* povs);

/**
 * Gets the buttons of a specific joystick.
 *
 * @param joystickNum the joystick number
 * @param buttons     the button values (output)
 * @return            the error code, or 0 for success
 */
int32_t HAL_GetJoystickButtons(int32_t joystickNum,
                               HAL_JoystickButtons* buttons);

/**
 * Retrieves the Joystick Descriptor for particular slot.
 *
 * @param desc [out] descriptor (data transfer object) to fill in.  desc is
 * filled in regardless of success. In other words, if descriptor is not
 * available, desc is filled in with default values matching the init-values in
 * Java and C++ Driverstation for when caller requests a too-large joystick
 * index.
 *
 * @return error code reported from Network Comm back-end.  Zero is good,
 * nonzero is bad.
 */
int32_t HAL_GetJoystickDescriptor(int32_t joystickNum,
                                  HAL_JoystickDescriptor* desc);

/**
 * Gets is a specific joystick is considered to be an XBox controller.
 *
 * @param joystickNum the joystick number
 * @return            true if xbox, false otherwise
 */
HAL_Bool HAL_GetJoystickIsXbox(int32_t joystickNum);

/**
 * Gets the type of joystick connected.
 *
 * This is device specific, and different depending on what system input type
 * the joystick uses.
 *
 * @param joystickNum the joystick number
 * @return            the enumerated joystick type
 */
int32_t HAL_GetJoystickType(int32_t joystickNum);

/**
 * Gets the name of a joystick.
 *
 * The returned array must be freed with HAL_FreeJoystickName.
 *
 * Will be null terminated.
 *
 * @param joystickNum the joystick number
 * @return            the joystick name
 */
char* HAL_GetJoystickName(int32_t joystickNum);

/**
 * Frees a joystick name received with HAL_GetJoystickName
 *
 * @param name the name storage
 */
void HAL_FreeJoystickName(char* name);

/**
 * Gets the type of a specific joystick axis.
 *
 * This is device specific, and different depending on what system input type
 * the joystick uses.
 *
 * @param joystickNum the joystick number
 * @param axis        the axis number
 * @return            the enumerated axis type
 */
int32_t HAL_GetJoystickAxisType(int32_t joystickNum, int32_t axis);

/**
 * Set joystick outputs.
 *
 * @param joystickNum the joystick numer
 * @param outputs     bitmask of outputs, 1 for on 0 for off
 * @param leftRumble  the left rumble value (0-FFFF)
 * @param rightRumble the right rumble value (0-FFFF)
 * @return            the error code, or 0 for success
 */
int32_t HAL_SetJoystickOutputs(int32_t joystickNum, int64_t outputs,
                               int32_t leftRumble, int32_t rightRumble);

/**
 * Returns the approximate match time.
 *
 * The FMS does not send an official match time to the robots, but does send
 * an approximate match time. The value will count down the time remaining in
 * the current period (auto or teleop).
 *
 * Warning: This is not an official time (so it cannot be used to dispute ref
 * calls or guarantee that a function will trigger before the match ends).
 *
 * The Practice Match function of the DS approximates the behavior seen on
 * the field.
 *
 * @param status the error code, or 0 for success
 * @return time remaining in current match period (auto or teleop)
 */
double HAL_GetMatchTime(int32_t* status);

/**
 * Gets info about a specific match.
 *
 * @param info the match info (output)
 * @return     the error code, or 0 for success
 */
int32_t HAL_GetMatchInfo(HAL_MatchInfo* info);

/**
 * Releases the DS Mutex to allow proper shutdown of any threads that are
 * waiting on it.
 */
void HAL_ReleaseDSMutex(void);

/**
 * Has a new control packet from the driver station arrived since the last
 * time this function was called?
 *
 * @return true if the control data has been updated since the last call
 */
HAL_Bool HAL_IsNewControlData(void);

/**
 * Waits for the newest DS packet to arrive. Note that this is a blocking call.
 * Checks if new control data has arrived since the last HAL_WaitForDSData or
 * HAL_IsNewControlData call. If new data has not arrived, waits for new data
 * to arrive. Otherwise, returns immediately.
 */
void HAL_WaitForDSData(void);

/**
 * Waits for the newest DS packet to arrive. If timeout is <= 0, this will wait
 * forever. Otherwise, it will wait until either a new packet, or the timeout
 * time has passed.
 *
 * @param timeout timeout in seconds
 * @return        true for new data, false for timeout
 */
HAL_Bool HAL_WaitForDSDataTimeout(double timeout);

/**
 * Initializes the driver station communication. This will properly
 * handle multiple calls. However note that this CANNOT be called from a library
 * that interfaces with LabVIEW.
 */
void HAL_InitializeDriverStation(void);

/**
 * Sets the program starting flag in the DS.
 *
 * This is what changes the DS to showing robot code ready.
 */
void HAL_ObserveUserProgramStarting(void);

/**
 * Sets the disabled flag in the DS.
 *
 * This is used for the DS to ensure the robot is properly responding to its
 * state request. Ensure this get called about every 50ms, or the robot will be
 * disabled by the DS.
 */
void HAL_ObserveUserProgramDisabled(void);

/**
 * Sets the autonomous enabled flag in the DS.
 *
 * This is used for the DS to ensure the robot is properly responding to its
 * state request. Ensure this get called about every 50ms, or the robot will be
 * disabled by the DS.
 */
void HAL_ObserveUserProgramAutonomous(void);

/**
 * Sets the teleoperated enabled flag in the DS.
 *
 * This is used for the DS to ensure the robot is properly responding to its
 * state request. Ensure this get called about every 50ms, or the robot will be
 * disabled by the DS.
 */
void HAL_ObserveUserProgramTeleop(void);

/**
 * Sets the test mode flag in the DS.
 *
 * This is used for the DS to ensure the robot is properly responding to its
 * state request. Ensure this get called about every 50ms, or the robot will be
 * disabled by the DS.
 */
void HAL_ObserveUserProgramTest(void);

#ifdef __cplusplus
}  // extern "C"
#endif
/** @} */
