// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

/** Video event. */
@SuppressWarnings("MemberName")
public class VideoEvent {
  /** VideoEvent kind. */
  public enum Kind {
    /** Unknown video event. */
    kUnknown(0x0000),
    /** Source Created event. */
    kSourceCreated(0x0001),
    /** Source Destroyed event. */
    kSourceDestroyed(0x0002),
    /** Source Connected event. */
    kSourceConnected(0x0004),
    /** Source Disconnected event. */
    kSourceDisconnected(0x0008),
    /** Source Video Modes Updated event. */
    kSourceVideoModesUpdated(0x0010),
    /** Source VideoMode Changed event. */
    kSourceVideoModeChanged(0x0020),
    /** Source Property Created event. */
    kSourcePropertyCreated(0x0040),
    /** Source Property Value Updated event. */
    kSourcePropertyValueUpdated(0x0080),
    /** Source Property Choices Updated event. */
    kSourcePropertyChoicesUpdated(0x0100),
    /** Sink Source Changed event. */
    kSinkSourceChanged(0x0200),
    /** Sink Created event. */
    kSinkCreated(0x0400),
    /** Sink Destroyed event. */
    kSinkDestroyed(0x0800),
    /** Sink Enabled event. */
    kSinkEnabled(0x1000),
    /** Sink Disabled event. */
    kSinkDisabled(0x2000),
    /** Network Interfaces Changed event. */
    kNetworkInterfacesChanged(0x4000),
    /** Telemetry Updated event. */
    kTelemetryUpdated(0x8000),
    /** Sink Property Created event. */
    kSinkPropertyCreated(0x10000),
    /** Sink Property Value Updated event. */
    kSinkPropertyValueUpdated(0x20000),
    /** Sink Property Choices Updated event. */
    kSinkPropertyChoicesUpdated(0x40000),
    /** Usb Cameras Changed event. */
    kUsbCamerasChanged(0x80000);

    private final int value;

    Kind(int value) {
      this.value = value;
    }

    /**
     * Returns the kind value.
     *
     * @return The kind value.
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
      case 0x0001 -> Kind.kSourceCreated;
      case 0x0002 -> Kind.kSourceDestroyed;
      case 0x0004 -> Kind.kSourceConnected;
      case 0x0008 -> Kind.kSourceDisconnected;
      case 0x0010 -> Kind.kSourceVideoModesUpdated;
      case 0x0020 -> Kind.kSourceVideoModeChanged;
      case 0x0040 -> Kind.kSourcePropertyCreated;
      case 0x0080 -> Kind.kSourcePropertyValueUpdated;
      case 0x0100 -> Kind.kSourcePropertyChoicesUpdated;
      case 0x0200 -> Kind.kSinkSourceChanged;
      case 0x0400 -> Kind.kSinkCreated;
      case 0x0800 -> Kind.kSinkDestroyed;
      case 0x1000 -> Kind.kSinkEnabled;
      case 0x2000 -> Kind.kSinkDisabled;
      case 0x4000 -> Kind.kNetworkInterfacesChanged;
      case 0x10000 -> Kind.kSinkPropertyCreated;
      case 0x20000 -> Kind.kSinkPropertyValueUpdated;
      case 0x40000 -> Kind.kSinkPropertyChoicesUpdated;
      case 0x80000 -> Kind.kUsbCamerasChanged;
      default -> Kind.kUnknown;
    };
  }

  VideoEvent(
      int kind,
      int source,
      int sink,
      String name,
      int pixelFormat,
      int width,
      int height,
      int fps,
      int property,
      int propertyKind,
      int value,
      String valueStr,
      int listener) {
    this.kind = getKindFromInt(kind);
    this.sourceHandle = source;
    this.sinkHandle = sink;
    this.name = name;
    this.mode = new VideoMode(pixelFormat, width, height, fps);
    this.propertyHandle = property;
    this.propertyKind = VideoProperty.getKindFromInt(propertyKind);
    this.value = value;
    this.valueStr = valueStr;
    this.listener = listener;
  }

  /** The video event kind. */
  public Kind kind;

  /**
   * The source handle.
   *
   * <p>Valid for kSource* and kSink* respectively.
   */
  public int sourceHandle;

  /** The sink handle. */
  public int sinkHandle;

  /** Source/sink/property name. */
  public String name;

  // Fields for kSourceVideoModeChanged event.

  /** New source video mode. */
  public VideoMode mode;

  // Fields for kSourceProperty* events.

  /** Source property handle. */
  public int propertyHandle;

  /** Source property kind. */
  public VideoProperty.Kind propertyKind;

  /** Event value. */
  public int value;

  /** Event value as a string. */
  public String valueStr;

  /** Listener that was triggered. */
  public int listener;

  /**
   * Returns the source associated with the event (if any).
   *
   * @return The source associated with the event (if any).
   */
  public VideoSource getSource() {
    return new VideoSource(CameraServerJNI.copySource(sourceHandle));
  }

  /**
   * Returns the sink associated with the event (if any).
   *
   * @return The sink associated with the event (if any).
   */
  public VideoSink getSink() {
    return new VideoSink(CameraServerJNI.copySink(sinkHandle));
  }

  /**
   * Returns the property associated with the event (if any).
   *
   * @return The property associated with the event (if any).
   */
  public VideoProperty getProperty() {
    return new VideoProperty(propertyHandle, propertyKind);
  }
}
