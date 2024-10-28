// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

/** A base class for single image providing sources. */
public abstract class ImageSource extends VideoSource {
  /**
   * Constructs an ImageSource.
   *
   * @param handle The image source handle.
   */
  protected ImageSource(int handle) {
    super(handle);
  }

  /**
   * Signal sinks that an error has occurred. This should be called instead of NotifyFrame when an
   * error occurs.
   *
   * @param msg Error message.
   */
  public void notifyError(String msg) {
    CameraServerJNI.notifySourceError(m_handle, msg);
  }

  /**
   * Set source connection status. Defaults to true.
   *
   * @param connected True for connected, false for disconnected
   */
  public void setConnected(boolean connected) {
    CameraServerJNI.setSourceConnected(m_handle, connected);
  }

  /**
   * Set source description.
   *
   * @param description Description
   */
  public void setDescription(String description) {
    CameraServerJNI.setSourceDescription(m_handle, description);
  }

  /**
   * Create a property.
   *
   * @param name Property name
   * @param kind Property kind
   * @param minimum Minimum value
   * @param maximum Maximum value
   * @param step Step value
   * @param defaultValue Default value
   * @param value Current value
   * @return Property
   */
  public VideoProperty createProperty(
      String name,
      VideoProperty.Kind kind,
      int minimum,
      int maximum,
      int step,
      int defaultValue,
      int value) {
    return new VideoProperty(
        CameraServerJNI.createSourceProperty(
            m_handle, name, kind.getValue(), minimum, maximum, step, defaultValue, value));
  }

  /**
   * Create an integer property.
   *
   * @param name Property name
   * @param minimum Minimum value
   * @param maximum Maximum value
   * @param step Step value
   * @param defaultValue Default value
   * @param value Current value
   * @return Property
   */
  public VideoProperty createIntegerProperty(
      String name, int minimum, int maximum, int step, int defaultValue, int value) {
    return new VideoProperty(
        CameraServerJNI.createSourceProperty(
            m_handle,
            name,
            VideoProperty.Kind.kInteger.getValue(),
            minimum,
            maximum,
            step,
            defaultValue,
            value));
  }

  /**
   * Create a boolean property.
   *
   * @param name Property name
   * @param defaultValue Default value
   * @param value Current value
   * @return Property
   */
  public VideoProperty createBooleanProperty(String name, boolean defaultValue, boolean value) {
    return new VideoProperty(
        CameraServerJNI.createSourceProperty(
            m_handle,
            name,
            VideoProperty.Kind.kBoolean.getValue(),
            0,
            1,
            1,
            defaultValue ? 1 : 0,
            value ? 1 : 0));
  }

  /**
   * Create a string property.
   *
   * @param name Property name
   * @param value Current value
   * @return Property
   */
  public VideoProperty createStringProperty(String name, String value) {
    VideoProperty prop =
        new VideoProperty(
            CameraServerJNI.createSourceProperty(
                m_handle, name, VideoProperty.Kind.kString.getValue(), 0, 0, 0, 0, 0));
    prop.setString(value);
    return prop;
  }

  /**
   * Configure enum property choices.
   *
   * @param property Property
   * @param choices Choices
   */
  public void setEnumPropertyChoices(VideoProperty property, String[] choices) {
    CameraServerJNI.setSourceEnumPropertyChoices(m_handle, property.m_handle, choices);
  }
}
