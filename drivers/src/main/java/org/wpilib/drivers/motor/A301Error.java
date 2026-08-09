// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.drivers.motor;

import org.wpilib.hardware.hal.HALUtil;

/** REVLib-compatible error categories returned by high-level A301 commands. */
public enum A301Error {
  kOk(0),
  kError(1),
  kTimeout(2),
  kNotImplemented(3),
  kHALError(4),
  kCantFindFirmware(5),
  kFirmwareTooOld(6),
  kFirmwareTooNew(7),
  kParamInvalidID(8),
  kParamMismatchType(9),
  kParamAccessMode(10),
  kParamInvalid(11),
  kParamNotImplementedDeprecated(12),
  kFollowConfigMismatch(13),
  kInvalid(14),
  kSetpointOutOfRange(15),
  kUnknown(16),
  kCANDisconnected(17),
  kDuplicateCANId(18),
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
