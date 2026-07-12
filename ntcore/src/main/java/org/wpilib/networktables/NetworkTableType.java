// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.networktables;

/** Network table data types. */
public enum NetworkTableType {
  /** Unassigned data type. */
  UNASSIGNED(0, ""),
  /** Boolean data type. */
  BOOLEAN(0x01, "boolean"),
  /** Double precision floating-point data type. */
  DOUBLE(0x02, "double"),
  /** String data type. */
  STRING(0x04, "string"),
  /** Raw data type. */
  RAW(0x08, "raw"),
  /** Boolean array data type. */
  BOOLEAN_ARRAY(0x10, "boolean[]"),
  /** Double precision floating-point array data type. */
  DOUBLE_ARRAY(0x20, "double[]"),
  /** String array data type. */
  STRING_ARRAY(0x40, "string[]"),
  /** Integer data type. */
  INTEGER(0x100, "int"),
  /** Single precision floating-point data type. */
  FLOAT(0x200, "float"),
  /** Integer array data type. */
  INTEGER_ARRAY(0x400, "int[]"),
  /** Single precision floating-point array data type. */
  FLOAT_ARRAY(0x800, "float[]");

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
      case 0x01 -> BOOLEAN;
      case 0x02 -> DOUBLE;
      case 0x04 -> STRING;
      case 0x08 -> RAW;
      case 0x10 -> BOOLEAN_ARRAY;
      case 0x20 -> DOUBLE_ARRAY;
      case 0x40 -> STRING_ARRAY;
      case 0x100 -> INTEGER;
      case 0x200 -> FLOAT;
      case 0x400 -> INTEGER_ARRAY;
      case 0x800 -> FLOAT_ARRAY;
      default -> UNASSIGNED;
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
      case "boolean" -> BOOLEAN;
      case "double" -> DOUBLE;
      case "float" -> FLOAT;
      case "int" -> INTEGER;
      case "string", "json" -> STRING;
      case "boolean[]" -> BOOLEAN_ARRAY;
      case "double[]" -> DOUBLE_ARRAY;
      case "float[]" -> FLOAT_ARRAY;
      case "int[]" -> INTEGER_ARRAY;
      case "string[]" -> STRING_ARRAY;
      case "" -> UNASSIGNED;
      default -> RAW;
    };
  }

  /**
   * Gets string from generic data value.
   *
   * @param data the data to check
   * @return type string of the data, or empty string if no match
   */
  public static String getStringFromObject(Object data) {
    return switch (data) {
      case Boolean _ -> "boolean";
      case Float _ -> "float";
      // Checking Long because NT supports 64-bit integers
      case Long _ -> "int";
      // If typeof Number class, return "double" as the type. Functions as a "catch-all".
      case Number _ -> "double";
      case String _ -> "string";
      case byte[] _, Byte[] _ -> "raw";
      case boolean[] _, Boolean[] _ -> "boolean[]";
      case float[] _, Float[] _ -> "float[]";
      case long[] _, Long[] _ -> "int[]";
      // If typeof Number class, return "double[]" as the type. Functions as a "catch-all".
      case double[] _, Double[] _, Number[] _ -> "double[]";
      case String[] _ -> "string[]";
      case null, default -> "";
    };
  }
}
