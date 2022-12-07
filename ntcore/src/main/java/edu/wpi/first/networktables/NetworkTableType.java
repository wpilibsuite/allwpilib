// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/** Network table data types. */
public enum NetworkTableType {
  kUnassigned(0, ""),
  kBoolean(0x01, "boolean"),
  kDouble(0x02, "double"),
  kString(0x04, "string"),
  kRaw(0x08, "raw"),
  kBooleanArray(0x10, "boolean[]"),
  kDoubleArray(0x20, "double[]"),
  kStringArray(0x40, "string[]"),
  kInteger(0x100, "int"),
  kFloat(0x200, "float"),
  kIntegerArray(0x400, "int[]"),
  kFloatArray(0x800, "float[]");

  private final int m_value;
  private final String m_valueStr;

  NetworkTableType(int value, String valueStr) {
    m_value = value;
    m_valueStr = valueStr;
  }

  public int getValue() {
    return m_value;
  }

  public String getValueStr() {
    return m_valueStr;
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
    switch (typeString) {
      case "boolean":
        return kBoolean;
      case "double":
        return kDouble;
      case "float":
        return kFloat;
      case "int":
        return kInteger;
      case "string":
      case "json":
        return kString;
      case "boolean[]":
        return kBooleanArray;
      case "double[]":
        return kDoubleArray;
      case "float[]":
        return kFloatArray;
      case "int[]":
        return kIntegerArray;
      case "string[]":
        return kStringArray;
      case "":
        return kUnassigned;
      default:
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
