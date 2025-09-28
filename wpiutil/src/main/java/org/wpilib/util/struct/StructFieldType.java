// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util.struct;

/** Known data types for raw struct dynamic fields (see StructFieldDescriptor). */
public enum StructFieldType {
  /** bool. */
  kBool("bool", false, false, 1),
  /** char. */
  kChar("char", false, false, 1),
  /** int8. */
  kInt8("int8", true, false, 1),
  /** int16. */
  kInt16("int16", true, false, 2),
  /** int32. */
  kInt32("int32", true, false, 4),
  /** int64. */
  kInt64("int64", true, false, 8),
  /** uint8. */
  kUint8("uint8", false, true, 1),
  /** uint16. */
  kUint16("uint16", false, true, 2),
  /** uint32. */
  kUint32("uint32", false, true, 4),
  /** uint64. */
  kUint64("uint64", false, true, 8),
  /** float. */
  kFloat("float", false, false, 4),
  /** double. */
  kDouble("double", false, false, 8),
  /** struct. */
  kStruct("struct", false, false, 0);

  /** The name of the data type. */
  @SuppressWarnings("MemberName")
  public final String name;

  /** Indicates if the data type is a signed integer. */
  @SuppressWarnings("MemberName")
  public final boolean isInt;

  /** Indicates if the data type is an unsigned integer. */
  @SuppressWarnings("MemberName")
  public final boolean isUint;

  /** The size (in bytes) of the data type. */
  @SuppressWarnings("MemberName")
  public final int size;

  StructFieldType(String name, boolean isInt, boolean isUint, int size) {
    this.name = name;
    this.isInt = isInt;
    this.isUint = isUint;
    this.size = size;
  }

  @Override
  public String toString() {
    return name;
  }

  /**
   * Get field type from string.
   *
   * @param str string
   * @return field type
   */
  public static StructFieldType fromString(String str) {
    for (StructFieldType type : StructFieldType.values()) {
      if (type.name.equals(str)) {
        return type;
      }
    }
    if ("float32".equals(str)) {
      return kFloat;
    } else if ("float64".equals(str)) {
      return kDouble;
    } else {
      return kStruct;
    }
  }
}
