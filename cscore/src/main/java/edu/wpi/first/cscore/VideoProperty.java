// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

/** A source or sink property. */
public class VideoProperty {
  public enum Kind {
    kNone(0),
    kBoolean(1),
    kInteger(2),
    kString(4),
    kEnum(8);

    private final int value;

    Kind(int value) {
      this.value = value;
    }

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
    switch (kind) {
      case 1:
        return Kind.kBoolean;
      case 2:
        return Kind.kInteger;
      case 4:
        return Kind.kString;
      case 8:
        return Kind.kEnum;
      default:
        return Kind.kNone;
    }
  }

  public String getName() {
    return CameraServerJNI.getPropertyName(m_handle);
  }

  public Kind getKind() {
    return m_kind;
  }

  public boolean isValid() {
    return m_kind != Kind.kNone;
  }

  // Kind checkers
  public boolean isBoolean() {
    return m_kind == Kind.kBoolean;
  }

  public boolean isInteger() {
    return m_kind == Kind.kInteger;
  }

  public boolean isString() {
    return m_kind == Kind.kString;
  }

  public boolean isEnum() {
    return m_kind == Kind.kEnum;
  }

  public int get() {
    return CameraServerJNI.getProperty(m_handle);
  }

  public void set(int value) {
    CameraServerJNI.setProperty(m_handle, value);
  }

  public int getMin() {
    return CameraServerJNI.getPropertyMin(m_handle);
  }

  public int getMax() {
    return CameraServerJNI.getPropertyMax(m_handle);
  }

  public int getStep() {
    return CameraServerJNI.getPropertyStep(m_handle);
  }

  public int getDefault() {
    return CameraServerJNI.getPropertyDefault(m_handle);
  }

  // String-specific functions
  public String getString() {
    return CameraServerJNI.getStringProperty(m_handle);
  }

  public void setString(String value) {
    CameraServerJNI.setStringProperty(m_handle, value);
  }

  // Enum-specific functions
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
  private Kind m_kind;
}
