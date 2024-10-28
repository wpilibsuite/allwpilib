// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/** Network table data types. */
public enum NetworkTableType {
  /** Unassigned data type. */
  kUnassigned(0, ""),
  /** Boolean data type. */
  kBoolean(0x01, "boolean"),
  /** Double precision floating-point data type. */
  kDouble(0x02, "double"),
  /** String data type. */
  kString(0x04, "string"),
  /** Raw data type. */
  kRaw(0x08, "raw"),
  /** Boolean array data type. */
  kBooleanArray(0x10, "boolean[]"),
  /** Double precision floating-point array data type. */
  kDoubleArray(0x20, "double[]"),
  /** String array data type. */
  kStringArray(0x40, "string[]"),
  /** Integer data type. */
  kInteger(0x100, "int"),
  /** Single precision floating-point data type. */
  kFloat(0x200, "float"),
  /** Integer array data type. */
  kIntegerArray(0x400, "int[]"),
  /** Single precision floating-point array data type. */
  kFloatArray(0x800, "float[]");

  private final int m_value;
  private final String m_valueStr;

  NetworkTableType(int value, String valueStr) {
    m_value = value;
    m_valueStr = valueStr;
  }

  /**
   * Returns the NetworkTable type value.
   *
   * @return The NetworkTable type value.
   */
  public int getValue() {
    return m_value;
  }

  /**
   * Returns the NetworkTable type value as as string.
   *
   * @return The NetworkTable type value as a string.
   */
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
    return switch (value) {
      case 0x01 -> kBoolean;
      case 0x02 -> kDouble;
      case 0x04 -> kString;
      case 0x08 -> kRaw;
      case 0x10 -> kBooleanArray;
      case 0x20 -> kDoubleArray;
      case 0x40 -> kStringArray;
      case 0x100 -> kInteger;
      case 0x200 -> kFloat;
      case 0x400 -> kIntegerArray;
      case 0x800 -> kFloatArray;
      default -> kUnassigned;
    };
  }

  /**
   * Convert from a type string to an enum type.
   *
   * @param typeString type string
   * @return The kind
   */
  public static NetworkTableType getFromString(String typeString) {
    return switch (typeString) {
      case "boolean" -> kBoolean;
      case "double" -> kDouble;
      case "float" -> kFloat;
      case "int" -> kInteger;
      case "string", "json" -> kString;
      case "boolean[]" -> kBooleanArray;
      case "double[]" -> kDoubleArray;
      case "float[]" -> kFloatArray;
      case "int[]" -> kIntegerArray;
      case "string[]" -> kStringArray;
      case "" -> kUnassigned;
      default -> kRaw;
    };
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
      // Checking Long because NT supports 64-bit integers
      return "int";
    } else if (data instanceof Double || data instanceof Number) {
      // If typeof Number class, return "double" as the type. Functions as a "catch-all".
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
      // If typeof Number class, return "double[]" as the type. Functions as a "catch-all".
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
