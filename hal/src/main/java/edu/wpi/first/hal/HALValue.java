// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/** Represents a HAL entry value. */
public final class HALValue {
  /** Unassigned type. */
  public static final int kUnassigned = 0;

  /** Boolean. */
  public static final int kBoolean = 0x01;

  /** Double. */
  public static final int kDouble = 0x02;

  /** Enum. */
  public static final int kEnum = 0x04;

  /** Int. */
  public static final int kInt = 0x08;

  /** Long. */
  public static final int kLong = 0x10;

  private int m_type;
  private long m_long;
  private double m_double;

  private HALValue(double value, int type) {
    m_type = type;
    m_double = value;
  }

  private HALValue(long value, int type) {
    m_type = type;
    m_long = value;
  }

  private HALValue() {}

  /**
   * Get the type of the value.
   *
   * @return Type (e.g. kBoolean).
   */
  public int getType() {
    return m_type;
  }

  /**
   * Get the value as a boolean. Does not perform type checking.
   *
   * @return value contents
   */
  public boolean getBoolean() {
    return m_long != 0;
  }

  /**
   * Get the value as a long. Does not perform type checking.
   *
   * @return value contents
   */
  public long getLong() {
    return m_long;
  }

  /**
   * Get the value as a double. Does not perform type checking.
   *
   * @return value contents
   */
  public double getDouble() {
    return m_double;
  }

  /**
   * Get the native long value. Does not perform type checking.
   *
   * @return value contents
   */
  public long getNativeLong() {
    return m_long;
  }

  /**
   * Get the native double value. Does not perform type checking.
   *
   * @return value contents
   */
  public double getNativeDouble() {
    return m_double;
  }

  /**
   * Build a HAL boolean value.
   *
   * @param value value
   * @return HAL value
   */
  public static HALValue makeBoolean(boolean value) {
    return new HALValue(value ? 1 : 0, kBoolean);
  }

  /**
   * Build a HAL enum value.
   *
   * @param value value
   * @return HAL value
   */
  public static HALValue makeEnum(int value) {
    return new HALValue(value, kEnum);
  }

  /**
   * Build a HAL integer value.
   *
   * @param value value
   * @return HAL value
   */
  public static HALValue makeInt(int value) {
    return new HALValue(value, kInt);
  }

  /**
   * Build a HAL long value.
   *
   * @param value value
   * @return HAL value
   */
  public static HALValue makeLong(long value) {
    return new HALValue(value, kLong);
  }

  /**
   * Build a HAL double value.
   *
   * @param value value
   * @return HAL value
   */
  public static HALValue makeDouble(double value) {
    return new HALValue(value, kDouble);
  }

  /**
   * Build a HAL unassigned value.
   *
   * @return HAL value
   */
  public static HALValue makeUnassigned() {
    return new HALValue();
  }

  /**
   * Build a HAL value from its native components.
   *
   * @param type type
   * @param value1 long value (all except double)
   * @param value2 double value (for double only)
   * @return HAL value
   */
  public static HALValue fromNative(int type, long value1, double value2) {
    return switch (type) {
      case kBoolean -> makeBoolean(value1 != 0);
      case kDouble -> makeDouble(value2);
      case kEnum -> makeEnum((int) value1);
      case kInt -> makeInt((int) value1);
      case kLong -> makeLong(value1);
      default -> makeUnassigned();
    };
  }
}
