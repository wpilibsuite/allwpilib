/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore;

import org.opencv.core.Mat;

/**
 * A source that represents a video camera.
 */
public class CvSource extends VideoSource {
  /**
   * Create an OpenCV source.
   * @param name Source name (arbitrary unique identifier)
   * @param mode Video mode being generated
   */
  public CvSource(String name, VideoMode mode) {
    super(CameraServerJNI.createCvSource(name, mode.pixelFormat.getValue(), mode.width, mode.height, mode.fps));
  }

  /**
   * Create an OpenCV source.
   * @param name Source name (arbitrary unique identifier)
   * @param pixelFormat Pixel format
   * @param width width
   * @param height height
   * @param fps fps
   */
  public CvSource(String name, VideoMode.PixelFormat pixelFormat, int width, int height, int fps) {
    super(CameraServerJNI.createCvSource(name, pixelFormat.getValue(), width, height, fps));
  }

  /**
   * Put an OpenCV image and notify sinks.
   * Only 8-bit single-channel or 3-channel (with BGR channel order) images
   * are supported. If the format, depth or channel order is different, use
   * Mat.convertTo() and/or cvtColor() to convert it first.
   * @param image OpenCV image
   */
  public void putFrame(Mat image) {
    CameraServerJNI.putSourceFrame(m_handle, image.nativeObj);
  }

  /**
   * Signal sinks that an error has occurred.  This should be called instead
   * of NotifyFrame when an error occurs.
   */
  public void notifyError(String msg) {
    CameraServerJNI.notifySourceError(m_handle, msg);
  }

  /**
   * Set source connection status.  Defaults to true.
   * @param connected True for connected, false for disconnected
   */
  public void setConnected(boolean connected) {
    CameraServerJNI.setSourceConnected(m_handle, connected);
  }

  /**
   * Set source description.
   * @param description Description
   */
  public void setDescription(String description) {
    CameraServerJNI.setSourceDescription(m_handle, description);
  }

  /**
   * Create a property.
   * @param name Property name
   * @param kind Property kind
   * @param minimum Minimum value
   * @param maximum Maximum value
   * @param step Step value
   * @param defaultValue Default value
   * @param value Current value
   * @return Property
   */
  public VideoProperty createProperty(String name, VideoProperty.Kind kind, int minimum, int maximum, int step, int defaultValue, int value) {
    return new VideoProperty(
        CameraServerJNI.createSourceProperty(m_handle, name, kind.getValue(), minimum, maximum, step, defaultValue, value));
  }

  /// Create an integer property.
  /// @param name Property name
  /// @param minimum Minimum value
  /// @param maximum Maximum value
  /// @param step Step value
  /// @param defaultValue Default value
  /// @param value Current value
  /// @return Property
  public VideoProperty createIntegerProperty(String name, int minimum, int maximum, int step, int defaultValue, int value) {
    return new VideoProperty(
        CameraServerJNI.createSourceProperty(m_handle, name, VideoProperty.Kind.kInteger.getValue(), minimum, maximum, step, defaultValue, value));
  }

  /// Create a boolean property.
  /// @param name Property name
  /// @param defaultValue Default value
  /// @param value Current value
  /// @return Property
  public VideoProperty createBooleanProperty(String name, boolean defaultValue, boolean value) {
    return new VideoProperty(
        CameraServerJNI.createSourceProperty(m_handle, name, VideoProperty.Kind.kBoolean.getValue(), 0, 1, 1, defaultValue ? 1 : 0, value ? 1 : 0));
  }

  /// Create a string property.
  /// @param name Property name
  /// @param value Current value
  /// @return Property
  public VideoProperty createStringProperty(String name, String value) {
    VideoProperty prop = new VideoProperty(
        CameraServerJNI.createSourceProperty(m_handle, name, VideoProperty.Kind.kString.getValue(), 0, 0, 0, 0, 0));
    prop.setString(value);
    return prop;
  }

  /// Create a property with a change callback.
  /// @param name Property name
  /// @param kind Property kind
  /// @param minimum Minimum value
  /// @param maximum Maximum value
  /// @param step Step value
  /// @param defaultValue Default value
  /// @param value Current value
  /// @param onChange Callback to call when the property value changes
  /// @return Property
  //public VideoProperty createProperty(
  //    String name, VideoProperty.Kind kind, int minimum, int maximum, int step, int defaultValue, int value,
  //    std::function<void(VideoProperty property)>
  //        onChange);

  /**
   * Configure enum property choices.
   * @param property Property
   * @param choices Choices
   */
  public void SetEnumPropertyChoices(VideoProperty property, String[] choices) {
    CameraServerJNI.setSourceEnumPropertyChoices(m_handle, property.m_handle, choices);
  }
}
