/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore;

/**
 * Video event.
 */
public class VideoEvent {
  public enum Kind {
    kUnknown(0x0000),
    kSourceCreated(0x0001),
    kSourceDestroyed(0x0002),
    kSourceConnected(0x0004),
    kSourceDisconnected(0x0008),
    kSourceVideoModesUpdated(0x0010),
    kSourceVideoModeChanged(0x0020),
    kSourcePropertyCreated(0x0040),
    kSourcePropertyValueUpdated(0x0080),
    kSourcePropertyChoicesUpdated(0x0100),
    kSinkSourceChanged(0x0200),
    kSinkCreated(0x0400),
    kSinkDestroyed(0x0800),
    kSinkEnabled(0x1000),
    kSinkDisabled(0x2000),
    kNetworkInterfacesChanged(0x4000),
    kTelemetryUpdated(0x8000),
    kSinkPropertyCreated(0x10000),
    kSinkPropertyValueUpdated(0x20000),
    kSinkPropertyChoicesUpdated(0x40000);

    @SuppressWarnings("MemberName")
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
  @SuppressWarnings("PMD.CyclomaticComplexity")
  public static Kind getKindFromInt(int kind) {
    switch (kind) {
      case 0x0001: return Kind.kSourceCreated;
      case 0x0002: return Kind.kSourceDestroyed;
      case 0x0004: return Kind.kSourceConnected;
      case 0x0008: return Kind.kSourceDisconnected;
      case 0x0010: return Kind.kSourceVideoModesUpdated;
      case 0x0020: return Kind.kSourceVideoModeChanged;
      case 0x0040: return Kind.kSourcePropertyCreated;
      case 0x0080: return Kind.kSourcePropertyValueUpdated;
      case 0x0100: return Kind.kSourcePropertyChoicesUpdated;
      case 0x0200: return Kind.kSinkSourceChanged;
      case 0x0400: return Kind.kSinkCreated;
      case 0x0800: return Kind.kSinkDestroyed;
      case 0x1000: return Kind.kSinkEnabled;
      case 0x2000: return Kind.kSinkDisabled;
      case 0x4000: return Kind.kNetworkInterfacesChanged;
      case 0x10000: return Kind.kSinkPropertyCreated;
      case 0x20000: return Kind.kSinkPropertyValueUpdated;
      case 0x40000: return Kind.kSinkPropertyChoicesUpdated;
      default: return Kind.kUnknown;
    }
  }

  @SuppressWarnings("PMD.ExcessiveParameterList")
  VideoEvent(int kind, int source, int sink, String name, int pixelFormat,
             int width, int height, int fps, int property, int propertyKind,
             int value, String valueStr) {
    this.kind = getKindFromInt(kind);
    this.sourceHandle = source;
    this.sinkHandle = sink;
    this.name = name;
    this.mode = new VideoMode(pixelFormat, width, height, fps);
    this.propertyHandle = property;
    this.propertyKind = VideoProperty.getKindFromInt(propertyKind);
    this.value = value;
    this.valueStr = valueStr;
  }

  @SuppressWarnings("MemberName")
  public Kind kind;

  // Valid for kSource* and kSink* respectively
  @SuppressWarnings("MemberName")
  public int sourceHandle;
  @SuppressWarnings("MemberName")
  public int sinkHandle;

  // Source/sink/property name
  @SuppressWarnings("MemberName")
  public String name;

  // Fields for kSourceVideoModeChanged event
  @SuppressWarnings("MemberName")
  public VideoMode mode;

  // Fields for kSourceProperty* events
  @SuppressWarnings("MemberName")
  public int propertyHandle;
  @SuppressWarnings("MemberName")
  public VideoProperty.Kind propertyKind;
  @SuppressWarnings("MemberName")
  public int value;
  @SuppressWarnings("MemberName")
  public String valueStr;

  public VideoSource getSource() {
    return new VideoSource(CameraServerJNI.copySource(sourceHandle));
  }

  public VideoSink getSink() {
    return new VideoSink(CameraServerJNI.copySink(sinkHandle));
  }

  public VideoProperty getProperty() {
    return new VideoProperty(propertyHandle, propertyKind);
  }

}
