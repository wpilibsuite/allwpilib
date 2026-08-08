// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include "wpi/hal/Types.h"

/**
 * @defgroup hal_a301 FIRST A301 Motor Controller Functions
 * @ingroup hal_capi
 * @{
 */

/** Minimum device ID accepted by an A301. */
#define HAL_A301_MIN_DEVICE_ID 0

/** Maximum device ID accepted by an A301. */
#define HAL_A301_MAX_DEVICE_ID 62

/** The factory-default A301 device ID. */
#define HAL_A301_DEFAULT_DEVICE_ID 3

/** A301 periodic status frames. */
HAL_ENUM(HAL_A301PeriodicFrame) {
  HAL_A301_STATUS_0 = 0,
  HAL_A301_STATUS_1 = 1,
  HAL_A301_STATUS_2 = 2,
  HAL_A301_STATUS_3 = 3,
};

/** A301 gearbox speed variants. */
HAL_ENUM(HAL_A301GearboxRPM) {
  HAL_A301_GEARBOX_RPM_UNKNOWN = 0,
  HAL_A301_GEARBOX_RPM_215 = 1,
  HAL_A301_GEARBOX_RPM_500 = 2,
};

/** A301 idle modes. */
HAL_ENUM(HAL_A301IdleMode) {
  HAL_A301_IDLE_MODE_COAST = 0,
  HAL_A301_IDLE_MODE_BRAKE = 1,
};

/** A301 closed-loop control types. */
HAL_ENUM(HAL_A301ControlType) {
  HAL_A301_CONTROL_TYPE_DUTY_CYCLE = 0,
  HAL_A301_CONTROL_TYPE_VELOCITY = 1,
  HAL_A301_CONTROL_TYPE_VOLTAGE = 2,
  HAL_A301_CONTROL_TYPE_RELATIVE_POSITION = 3,
  HAL_A301_CONTROL_TYPE_ABSOLUTE_POSITION = 4,
  HAL_A301_CONTROL_TYPE_CURRENT = 5,
};

/** A301 firmware version information. */
struct HAL_A301FirmwareVersion {
  uint8_t major;
  uint8_t minor;
  uint16_t patch;
  uint8_t prerelease;
  uint8_t hardwareRevision;
  uint32_t raw;
};

/** A301 periodic status frame 0. */
struct HAL_A301PeriodicStatus0 {
  double appliedOutput;
  double voltage;
  double current;
  int32_t motorTemperature;
  HAL_Bool inverted;
  HAL_Bool primaryHeartbeatLock;
  HAL_A301GearboxRPM gearboxRPM;
  uint64_t timestamp;
};

/** A301 periodic status frame 1. */
struct HAL_A301PeriodicStatus1 {
  HAL_Bool otherFault;
  HAL_Bool motorTypeFault;
  HAL_Bool sensorFault;
  HAL_Bool canFault;
  HAL_Bool temperatureFault;
  HAL_Bool drvFault;
  HAL_Bool escEepromFault;
  HAL_Bool firmwareFault;
  HAL_Bool motorStartupFault;

  HAL_Bool brownoutWarning;
  HAL_Bool overcurrentWarning;
  HAL_Bool escEepromWarning;
  HAL_Bool extEepromWarning;
  HAL_Bool sensorWarning;
  HAL_Bool stallWarning;
  HAL_Bool hasResetWarning;
  HAL_Bool otherWarning;
  HAL_Bool overvoltageWarning;
  HAL_Bool motorLoopSpeedWarning;

  HAL_Bool otherStickyFault;
  HAL_Bool motorTypeStickyFault;
  HAL_Bool sensorStickyFault;
  HAL_Bool canStickyFault;
  HAL_Bool temperatureStickyFault;
  HAL_Bool drvStickyFault;
  HAL_Bool escEepromStickyFault;
  HAL_Bool firmwareStickyFault;
  HAL_Bool motorStartupStickyFault;

  HAL_Bool brownoutStickyWarning;
  HAL_Bool overcurrentStickyWarning;
  HAL_Bool escEepromStickyWarning;
  HAL_Bool extEepromStickyWarning;
  HAL_Bool sensorStickyWarning;
  HAL_Bool stallStickyWarning;
  HAL_Bool hasResetStickyWarning;
  HAL_Bool otherStickyWarning;
  HAL_Bool overvoltageStickyWarning;
  HAL_Bool motorLoopSpeedStickyWarning;

  HAL_Bool isFollower;
  uint64_t timestamp;
};

/** A301 periodic status frame 2. */
struct HAL_A301PeriodicStatus2 {
  double encoderVelocity;
  double relativeEncoderPosition;
  uint64_t timestamp;
};

/** A301 periodic status frame 3. */
struct HAL_A301PeriodicStatus3 {
  double absoluteEncoderPosition;
  uint64_t timestamp;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Detects the device ID of an A301 attached to a CAN bus.
 *
 * @param busId the CAN bus ID
 * @param status error status variable; 0 on success
 * @return the detected device ID, or -1 on failure
 */
int32_t HAL_DetectA301DeviceId(int32_t busId, int32_t* status);

/**
 * Initializes an A301.
 *
 * On Motioncore CAN buses (IDs 5 and above), device ID detection is attempted
 * and the detected ID is used when available.
 *
 * @param busId the CAN bus ID
 * @param deviceId the requested device ID
 * @param allocationLocation allocation stack trace
 * @param status error status variable; 0 on success
 * @return the created A301 handle
 */
HAL_A301Handle HAL_InitializeA301(int32_t busId, int32_t deviceId,
                                  const char* allocationLocation,
                                  int32_t* status);

/** Frees an A301 handle. */
void HAL_FreeA301(HAL_A301Handle handle);

/** Returns whether an A301 device ID is valid. */
HAL_Bool HAL_CheckA301DeviceId(int32_t deviceId);

/** Gets the CAN bus ID associated with a handle. */
int32_t HAL_GetA301BusId(HAL_A301Handle handle, int32_t* status);

/** Gets the resolved device ID associated with a handle. */
int32_t HAL_GetA301DeviceId(HAL_A301Handle handle, int32_t* status);

/** Gets the A301 firmware version. */
void HAL_GetA301FirmwareVersion(HAL_A301Handle handle,
                                struct HAL_A301FirmwareVersion* version,
                                int32_t* status);

/** Gets periodic status frame 0. */
void HAL_GetA301PeriodicStatus0(HAL_A301Handle handle,
                                struct HAL_A301PeriodicStatus0* frame,
                                int32_t* status);

/** Gets periodic status frame 1. */
void HAL_GetA301PeriodicStatus1(HAL_A301Handle handle,
                                struct HAL_A301PeriodicStatus1* frame,
                                int32_t* status);

/** Gets periodic status frame 2. */
void HAL_GetA301PeriodicStatus2(HAL_A301Handle handle,
                                struct HAL_A301PeriodicStatus2* frame,
                                int32_t* status);

/** Gets periodic status frame 3. */
void HAL_GetA301PeriodicStatus3(HAL_A301Handle handle,
                                struct HAL_A301PeriodicStatus3* frame,
                                int32_t* status);

/** Sets the relative encoder position in rotations. */
void HAL_SetA301RelativeEncoderPosition(HAL_A301Handle handle, double position,
                                        int32_t* status);

/** Sets the absolute encoder position in rotations. */
void HAL_SetA301AbsoluteEncoderPosition(HAL_A301Handle handle, double position,
                                        int32_t* status);

/** Sets an A301 control setpoint. */
void HAL_SetA301Setpoint(HAL_A301Handle handle, double value,
                         HAL_A301ControlType controlType, double positionSpeed,
                         int32_t* status);

/** Sets the A301 idle mode. */
void HAL_SetA301IdleMode(HAL_A301Handle handle, HAL_A301IdleMode idleMode,
                         int32_t* status);

/** Gets the A301 idle mode. */
HAL_A301IdleMode HAL_GetA301IdleMode(HAL_A301Handle handle, int32_t* status);

/** Enables or disables continuous input for absolute position control. */
void HAL_SetA301AbsolutePositionContinuousInput(HAL_A301Handle handle,
                                                HAL_Bool enabled,
                                                int32_t* status);

/** Gets whether absolute position continuous input is enabled. */
HAL_Bool HAL_GetA301AbsolutePositionContinuousInput(HAL_A301Handle handle,
                                                    int32_t* status);

/** Sets the absolute encoder range offset in rotations. */
void HAL_SetA301AbsoluteEncoderRangeOffset(HAL_A301Handle handle, double offset,
                                           int32_t* status);

/** Gets the absolute encoder range offset in rotations. */
double HAL_GetA301AbsoluteEncoderRangeOffset(HAL_A301Handle handle,
                                             int32_t* status);

/** Sets whether the A301 output is inverted. */
void HAL_SetA301Inverted(HAL_A301Handle handle, HAL_Bool inverted,
                         int32_t* status);

/** Gets whether the A301 output is inverted. */
HAL_Bool HAL_GetA301Inverted(HAL_A301Handle handle, int32_t* status);

/** Clears active and sticky faults. */
void HAL_ClearA301Faults(HAL_A301Handle handle, int32_t* status);

/** Sets an A301 periodic status frame period in milliseconds. */
void HAL_SetA301StatusFramePeriod(HAL_A301Handle handle,
                                  HAL_A301PeriodicFrame frame, int32_t periodMs,
                                  int32_t* status);

/** Gets an A301 periodic status frame period in milliseconds. */
int32_t HAL_GetA301StatusFramePeriod(HAL_A301Handle handle,
                                     HAL_A301PeriodicFrame frame,
                                     int32_t* status);

#ifdef __cplusplus
}  // extern "C"
#endif

/** @} */
