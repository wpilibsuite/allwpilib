// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/** Network table data types. */
public enum NetworkTableType {
  kUnassigned(0),
  kBoolean(0x01),
  kDouble(0x02),
  kString(0x04),
  kRaw(0x08),
  kBooleanArray(0x10),
  kDoubleArray(0x20),
  kStringArray(0x40),
  kInteger(0x100),
  kFloat(0x200),
  kIntegerArray(0x400),
  kFloatArray(0x800);

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
      case 0x01:
        return kBoolean;
      case 0x02:
        return kDouble;
      case 0x04:
        return kString;
      case 0x08:
        return kRaw;
      case 0x10:
        return kBooleanArray;
      case 0x20:
        return kDoubleArray;
      case 0x40:
        return kStringArray;
      case 0x100:
        return kInteger;
      case 0x200:
        return kFloat;
      case 0x400:
        return kIntegerArray;
      case 0x800:
        return kFloatArray;
      default:
        return kUnassigned;
    }
  }

  /**
   * Convert from a type string to an enum type.
   *
   * @param typeString type string
   * @return The kind
   */
  public static NetworkTableType getFromString(String typeString) {
    if ("boolean".equals(typeString)) {
      return kBoolean;
    } else if ("double".equals(typeString)) {
      return kDouble;
    } else if ("float".equals(typeString)) {
      return kFloat;
    } else if ("int".equals(typeString)) {
      return kInteger;
    } else if ("string".equals(typeString) || "json".equals(typeString)) {
      return kString;
    } else if ("boolean[]".equals(typeString)) {
      return kBooleanArray;
    } else if ("double[]".equals(typeString)) {
      return kDoubleArray;
    } else if ("float[]".equals(typeString)) {
      return kFloatArray;
    } else if ("int[]".equals(typeString)) {
      return kIntegerArray;
    } else if ("string[]".equals(typeString)) {
      return kStringArray;
    } else if ("".equals(typeString)) {
      return kUnassigned;
    } else {
      return kRaw;
    }
  }

  /**
   * Gets string from generic data value.
   *
   * @param data the data to check
   * @return type string of the data, or empty string if no match
   */
  public static String getStringFromObject(Object data) {
    if (data instanceof Boolean) {
      return "boolean";
    } else if (data instanceof Float) {
      return "float";
    } else if (data instanceof Long) {
      return "int";
    } else if (data instanceof Double || data instanceof Number) {
      return "double";
    } else if (data instanceof String) {
      return "string";
    } else if (data instanceof boolean[] || data instanceof Boolean[]) {
      return "boolean[]";
    } else if (data instanceof float[] || data instanceof Float[]) {
      return "float[]";
    } else if (data instanceof long[] || data instanceof Long[]) {
      return "int[]";
    } else if (data instanceof double[] || data instanceof Double[] || data instanceof Number[]) {
      return "double[]";
    } else if (data instanceof String[]) {
      return "string[]";
    } else if (data instanceof byte[] || data instanceof Byte[]) {
      return "raw";
    } else {
      return "";
    }
  }
}
