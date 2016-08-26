/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cameraserver;

public class VideoProperty {
  public enum Type {
    kNone(0), kBoolean(1), kDouble(2), kString(3), kEnum(4);
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

  public boolean isDouble() {
    return m_type == Type.kDouble;
  }

  public boolean isString() {
    return m_type == Type.kString;
  }

  public boolean isEnum() {
    return m_type == Type.kEnum;
  }

  // Boolean-specific functions
  public boolean getBoolean() {
    return CameraServerJNI.getBooleanProperty(m_handle);
  }

  public void setBoolean(boolean value) {
    CameraServerJNI.setBooleanProperty(m_handle, value);
  }

  // Double-specific functions
  public double getDouble() {
    return CameraServerJNI.getDoubleProperty(m_handle);
  }

  public void setDouble(double value) {
    CameraServerJNI.setDoubleProperty(m_handle, value);
  }

  public double getMin() {
    return CameraServerJNI.getDoublePropertyMin(m_handle);
  }

  public double getMax() {
    return CameraServerJNI.getDoublePropertyMax(m_handle);
  }

  // String-specific functions
  public String getString() {
    return CameraServerJNI.getStringProperty(m_handle);
  }

  public void setString(String value) {
    CameraServerJNI.setStringProperty(m_handle, value);
  }

  // Enum-specific functions
  public int getEnum() {
    return CameraServerJNI.getEnumProperty(m_handle);
  }

  public void setEnum(int value) {
    CameraServerJNI.setEnumProperty(m_handle, value);
  }

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
