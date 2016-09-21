/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cameraserver;

public class VideoProperty {
  public enum Type {
    kNone(0), kBoolean(1), kInteger(2), kString(4), kEnum(8);
    private int value;

    private Type(int value) {
      this.value = value;
    }

    public int getValue() {
      return value;
    }
  };
  private static final Type[] m_typeValues = Type.values();

  public String getName() {
    return CameraServerJNI.getPropertyName(m_handle);
  }

  public Type type() {
    return m_type;
  }

  public boolean isValid() {
    return m_type != Type.kNone;
  }

  // Type checkers
  public boolean isBoolean() {
    return m_type == Type.kBoolean;
  }

  public boolean isInteger() {
    return m_type == Type.kInteger;
  }

  public boolean isString() {
    return m_type == Type.kString;
  }

  public boolean isEnum() {
    return m_type == Type.kEnum;
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
    m_type = m_typeValues[CameraServerJNI.getPropertyType(handle)];
  }

  int m_handle;
  private Type m_type;
}
