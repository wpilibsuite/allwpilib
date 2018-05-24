/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

/**
 * Network table data types.
 */
public enum NetworkTableType {
  kUnassigned(0),
  kBoolean(0x01),
  kDouble(0x02),
  kString(0x04),
  kRaw(0x08),
  kBooleanArray(0x10),
  kDoubleArray(0x20),
  kStringArray(0x40),
  kRpc(0x80);

  @SuppressWarnings("MemberName")
  private final int value;

  NetworkTableType(int value) {
    this.value = value;
  }

  public int getValue() {
    return value;
  }

  /**
   * Convert from the numerical representation of type to an enum type.
   *
   * @param value The numerical representation of kind
   * @return The kind
   */
  public static NetworkTableType getFromInt(int value) {
    switch (value) {
      case 0x01: return kBoolean;
      case 0x02: return kDouble;
      case 0x04: return kString;
      case 0x08: return kRaw;
      case 0x10: return kBooleanArray;
      case 0x20: return kDoubleArray;
      case 0x40: return kStringArray;
      case 0x80: return kRpc;
      default: return kUnassigned;
    }
  }
}
