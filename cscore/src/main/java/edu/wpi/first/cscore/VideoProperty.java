// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

/** A source or sink property. */
public class VideoProperty {
  /** VideoProperty property types. */
  public enum Kind {
    /** No specific property. */
    kNone(0),
    /** Boolean property. */
    kBoolean(1),
    /** Integer property. */
    kInteger(2),
    /** String property. */
    kString(4),
    /** Enum property. */
    kEnum(8);

    private final int value;

    Kind(int value) {
      this.value = value;
    }

    /**
     * Returns the Kind value.
     *
     * @return The Kind value.
     */
    public int getValue() {
      return value;
    }
  }

  /**
   * Convert from the numerical representation of kind to an enum type.
   *
   * @param kind The numerical representation of kind
   * @return The kind
   */
  public static Kind getKindFromInt(int kind) {
    return switch (kind) {
      case 1 -> Kind.kBoolean;
      case 2 -> Kind.kInteger;
      case 4 -> Kind.kString;
      case 8 -> Kind.kEnum;
      default -> Kind.kNone;
    };
  }

  /**
   * Returns property name.
   *
   * @return Property name.
   */
  public String getName() {
    return CameraServerJNI.getPropertyName(m_handle);
  }

  /**
   * Returns property kind.
   *
   * @return Property kind.
   */
  public Kind getKind() {
    return m_kind;
  }

  /**
   * Returns true if property is valid.
   *
   * @return True if property is valid.
   */
  public boolean isValid() {
    return m_kind != Kind.kNone;
  }

  /**
   * Returns true if property is a boolean.
   *
   * @return True if property is a boolean.
   */
  public boolean isBoolean() {
    return m_kind == Kind.kBoolean;
  }

  /**
   * Returns true if property is an integer.
   *
   * @return True if property is an integer.
   */
  public boolean isInteger() {
    return m_kind == Kind.kInteger;
  }

  /**
   * Returns true if property is a string.
   *
   * @return True if property is a string.
   */
  public boolean isString() {
    return m_kind == Kind.kString;
  }

  /**
   * Returns true if property is an enum.
   *
   * @return True if property is an enum.
   */
  public boolean isEnum() {
    return m_kind == Kind.kEnum;
  }

  /**
   * Returns property value.
   *
   * @return Property value.
   */
  public int get() {
    return CameraServerJNI.getProperty(m_handle);
  }

  /**
   * Sets property value.
   *
   * @param value Property value.
   */
  public void set(int value) {
    CameraServerJNI.setProperty(m_handle, value);
  }

  /**
   * Returns property minimum value.
   *
   * @return Property minimum value.
   */
  public int getMin() {
    return CameraServerJNI.getPropertyMin(m_handle);
  }

  /**
   * Returns property maximum value.
   *
   * @return Property maximum value.
   */
  public int getMax() {
    return CameraServerJNI.getPropertyMax(m_handle);
  }

  /**
   * Returns property step size.
   *
   * @return Property step size.
   */
  public int getStep() {
    return CameraServerJNI.getPropertyStep(m_handle);
  }

  /**
   * Returns property default value.
   *
   * @return Property default value.
   */
  public int getDefault() {
    return CameraServerJNI.getPropertyDefault(m_handle);
  }

  /**
   * Returns the string property value.
   *
   * <p>This function is string-specific.
   *
   * @return The string property value.
   */
  public String getString() {
    return CameraServerJNI.getStringProperty(m_handle);
  }

  /**
   * Sets the string property value.
   *
   * <p>This function is string-specific.
   *
   * @param value String property value.
   */
  public void setString(String value) {
    CameraServerJNI.setStringProperty(m_handle, value);
  }

  /**
   * Returns the possible values for the enum property value.
   *
   * <p>This function is enum-specific.
   *
   * @return The possible values for the enum property value.
   */
  public String[] getChoices() {
    return CameraServerJNI.getEnumPropertyChoices(m_handle);
  }

  VideoProperty(int handle) {
    m_handle = handle;
    m_kind = getKindFromInt(CameraServerJNI.getPropertyKind(handle));
  }

  VideoProperty(int handle, Kind kind) {
    m_handle = handle;
    m_kind = kind;
  }

  int m_handle;
  private final Kind m_kind;
}
