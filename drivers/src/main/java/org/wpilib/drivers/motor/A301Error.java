// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.drivers.motor;

import org.wpilib.hardware.hal.HALUtil;

/** REVLib-compatible error categories returned by high-level A301 commands. */
public enum A301Error {
  /** No error occurred. */
  kOk(0),
  /** A general controller error occurred. */
  kError(1),
  /** The operation timed out. */
  kTimeout(2),
  /** The requested operation is not implemented. */
  kNotImplemented(3),
  /** The HAL returned an error. */
  kHALError(4),
  /** The controller firmware could not be read. */
  kCantFindFirmware(5),
  /** The controller firmware is too old. */
  kFirmwareTooOld(6),
  /** The controller firmware is newer than this driver supports. */
  kFirmwareTooNew(7),
  /** The parameter ID is invalid. */
  kParamInvalidID(8),
  /** The parameter type does not match the expected type. */
  kParamMismatchType(9),
  /** The parameter does not support the requested access mode. */
  kParamAccessMode(10),
  /** The parameter value is invalid. */
  kParamInvalid(11),
  /** The parameter is deprecated or not implemented. */
  kParamNotImplementedDeprecated(12),
  /** The follower configuration does not match its leader. */
  kFollowConfigMismatch(13),
  /** The returned error value is invalid. */
  kInvalid(14),
  /** The requested setpoint is outside the accepted range. */
  kSetpointOutOfRange(15),
  /** An unknown controller error occurred. */
  kUnknown(16),
  /** The controller is disconnected from the CAN bus. */
  kCANDisconnected(17),
  /** Another device is using the requested CAN ID. */
  kDuplicateCANId(18),
  /** The requested CAN ID is invalid. */
  kInvalidCANId(19);

  /** REVLib-compatible numeric value. */
  @SuppressWarnings("MemberName")
  public final int value;

  A301Error(int value) {
    this.value = value;
  }

  /**
   * Returns the error corresponding to a REVLib-compatible numeric value.
   *
   * @param value numeric error value
   * @return corresponding error, or {@link #kInvalid} when unknown
   */
  public static A301Error fromInt(int value) {
    for (A301Error error : values()) {
      if (error.value == value) {
        return error;
      }
    }
    return kInvalid;
  }

  static A301Error fromHalStatus(int status) {
    return switch (status) {
      case 0 -> kOk;
      case -1154, -44087 -> kTimeout;
      case HALUtil.PARAMETER_OUT_OF_RANGE -> kParamInvalid;
      case -1029 -> kDuplicateCANId;
      case -1030 -> kInvalidCANId;
      case HALUtil.INCOMPATIBLE_STATE -> kError;
      case -35007 -> kCANDisconnected;
      default -> kHALError;
    };
  }
}
