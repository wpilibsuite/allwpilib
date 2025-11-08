// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct;

import java.util.Map;

/** Raw struct dynamic field descriptor. */
public class StructFieldDescriptor {
  private static int toBitWidth(int size, int bitWidth) {
    if (bitWidth == 0) {
      return size * 8;
    } else {
      return bitWidth;
    }
  }

  private static long toBitMask(int size, int bitWidth) {
    if (size == 0) {
      return 0;
    } else {
      return -1L >>> (64 - toBitWidth(size, bitWidth));
    }
  }

  // does not fill in offset, shift
  StructFieldDescriptor(
      StructDescriptor parent,
      String name,
      StructFieldType type,
      int size,
      int arraySize,
      int bitWidth,
      Map<String, Long> enumValues,
      StructDescriptor structDesc) {
    m_parent = parent;
    m_name = name;
    m_size = size;
    m_arraySize = arraySize;
    m_enum = enumValues;
    m_struct = structDesc;
    m_bitMask = toBitMask(size, bitWidth);
    m_type = type;
    m_bitWidth = toBitWidth(size, bitWidth);
  }

  /**
   * Gets the dynamic struct this field is contained in.
   *
   * @return struct descriptor
   */
  public StructDescriptor getParent() {
    return m_parent;
  }

  /**
   * Gets the field name.
   *
   * @return field name
   */
  public String getName() {
    return m_name;
  }

  /**
   * Gets the field type.
   *
   * @return field type
   */
  public StructFieldType getType() {
    return m_type;
  }

  /**
   * Returns whether the field type is a signed integer.
   *
   * @return true if signed integer, false otherwise
   */
  public boolean isInt() {
    return m_type.isInt;
  }

  /**
   * Returns whether the field type is an unsigned integer.
   *
   * @return true if unsigned integer, false otherwise
   */
  public boolean isUint() {
    return m_type.isUint;
  }

  /**
   * Gets the underlying storage size of the field, in bytes.
   *
   * @return number of bytes
   */
  public int getSize() {
    return m_size;
  }

  /**
   * Gets the storage offset of the field, in bytes.
   *
   * @return number of bytes from the start of the struct
   */
  public int getOffset() {
    return m_offset;
  }

  /**
   * Gets the bit width of the field, in bits.
   *
   * @return number of bits
   */
  public int getBitWidth() {
    return m_bitWidth == 0 ? m_size * 8 : m_bitWidth;
  }

  /**
   * Gets the bit mask for the field. The mask is always the least significant bits (it is not
   * shifted).
   *
   * @return bit mask
   */
  public long getBitMask() {
    return m_bitMask;
  }

  /**
   * Gets the bit shift for the field (LSB=0).
   *
   * @return number of bits
   */
  public int getBitShift() {
    return m_bitShift;
  }

  /**
   * Returns whether the field is an array.
   *
   * @return true if array
   */
  public boolean isArray() {
    return m_arraySize > 1;
  }

  /**
   * Gets the array size. Returns 1 if non-array.
   *
   * @return number of elements
   */
  public int getArraySize() {
    return m_arraySize;
  }

  /**
   * Returns whether the field has enumerated values.
   *
   * @return true if there are enumerated values
   */
  public boolean hasEnum() {
    return m_enum != null;
  }

  /**
   * Gets the enumerated values.
   *
   * @return set of enumerated values
   */
  public Map<String, Long> getEnumValues() {
    return m_enum;
  }

  /**
   * Gets the struct descriptor for a struct data type.
   *
   * @return struct descriptor; returns null for non-struct
   */
  public StructDescriptor getStruct() {
    return m_struct;
  }

  /**
   * Gets the minimum unsigned integer value that can be stored in this field.
   *
   * @return minimum value
   */
  public long getUintMin() {
    return 0;
  }

  /**
   * Gets the maximum unsigned integer value that can be stored in this field. Note this is not the
   * actual maximum for uint64 (due to Java lacking support for 64-bit unsigned integers).
   *
   * @return maximum value
   */
  public long getUintMax() {
    return m_bitMask;
  }

  /**
   * Gets the minimum signed integer value that can be stored in this field.
   *
   * @return minimum value
   */
  public long getIntMin() {
    return -(m_bitMask >> 1) - 1;
  }

  /**
   * Gets the maximum signed integer value that can be stored in this field.
   *
   * @return maximum value
   */
  public long getIntMax() {
    return m_bitMask >> 1;
  }

  /**
   * Returns whether the field is a bitfield.
   *
   * @return true if bitfield
   */
  public boolean isBitField() {
    return (m_bitShift != 0 || m_bitWidth != (m_size * 8)) && m_struct == null;
  }

  private final StructDescriptor m_parent;
  private final String m_name;
  int m_size;
  int m_offset;
  final int m_arraySize; // 1 for non-arrays
  private final Map<String, Long> m_enum;
  private final StructDescriptor m_struct; // null for non-structs
  private final long m_bitMask;
  private final StructFieldType m_type;
  private final int m_bitWidth;
  int m_bitShift;
}
