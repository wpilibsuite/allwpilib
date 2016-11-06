/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore;

/// Video event
public class VideoEvent {
  public enum Type {
    kSourceCreated(0x0001),
    kSourceDestroyed(0x0002),
    kSourceConnected(0x0004),
    kSourceDisconnected(0x0008),
    kSourceVideoModesUpdated(0x0010),
    kSourceVideoModeChanged(0x0020),
    kSinkCreated(0x0100),
    kSinkDestroyed(0x0200),
    kSinkEnabled(0x0400),
    kSinkDisabled(0x0800),
    kSourcePropertyCreated(0x1000),
    kSourcePropertyValueUpdated(0x2000),
    kSourcePropertyChoicesUpdated(0x4000);

    private int value;

    private Type(int value) {
      this.value = value;
    }

    public int getValue() {
      return value;
    }
  }
  private static final Type[] m_typeValues = Type.values();

  VideoEvent(int type, int source, int sink, String name, int pixelFormat,
             int width, int height, int fps, int property, int propertyType,
             int value, String valueStr) {
    this.type = m_typeValues[type];
    this.sourceHandle = source;
    this.sinkHandle = sink;
    this.name = name;
    this.mode = new VideoMode(pixelFormat, width, height, fps);
    this.propertyHandle = property;
    this.propertyType = VideoProperty.m_typeValues[propertyType];
    this.value = value;
    this.valueStr = valueStr;
  }

  public Type type;

  // Valid for kSource* and kSink* respectively
  private int sourceHandle;
  private int sinkHandle;

  // Source/sink name
  public String name;

  // Fields for kSourceVideoModeChanged event
  public VideoMode mode;

  // Fields for kSourceProperty* events
  private int propertyHandle;
  public VideoProperty.Type propertyType;
  public int value;
  public String valueStr;

  public VideoSource getSource() {
    return new VideoSource(CameraServerJNI.copySource(sourceHandle));
  }

  public VideoSink getSink() {
    return new VideoSink(CameraServerJNI.copySink(sinkHandle));
  }

  public VideoProperty getProperty() {
    return new VideoProperty(propertyHandle, propertyType);
  }

}
