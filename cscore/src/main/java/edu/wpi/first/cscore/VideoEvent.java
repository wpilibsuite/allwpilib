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
    UNKNOWN(0x0000),
    /** Source Created event. */
    SOURCE_CREATED(0x0001),
    /** Source Destroyed event. */
    SOURCE_DESTROYED(0x0002),
    /** Source Connected event. */
    SOURCE_CONNECTED(0x0004),
    /** Source Disconnected event. */
    SOURCE_DISCONNECTED(0x0008),
    /** Source Video Modes Updated event. */
    SOURCE_VIDEO_MODES_UPDATED(0x0010),
    /** Source VideoMode Changed event. */
    SOURCE_VIDEO_MODE_CHANGED(0x0020),
    /** Source Property Created event. */
    SOURCE_PROPERTY_CREATED(0x0040),
    /** Source Property Value Updated event. */
    SOURCE_PROPERTY_VALUE_UPDATED(0x0080),
    /** Source Property Choices Updated event. */
    SOURCE_PROPERTY_CHOICES_UPDATED(0x0100),
    /** Sink Source Changed event. */
    SINK_SOURCE_CHANGED(0x0200),
    /** Sink Created event. */
    SINK_CREATED(0x0400),
    /** Sink Destroyed event. */
    SINK_DESTROYED(0x0800),
    /** Sink Enabled event. */
    SINK_ENABLED(0x1000),
    /** Sink Disabled event. */
    SINK_DISABLED(0x2000),
    /** Network Interfaces Changed event. */
    NETWORK_INTERFACES_CHANGED(0x4000),
    /** Telemetry Updated event. */
    TELEMETRY_UPDATED(0x8000),
    /** Sink Property Created event. */
    SINK_PROPERTY_CREATED(0x10000),
    /** Sink Property Value Updated event. */
    SINK_PROPERTY_VALUE_UPDATED(0x20000),
    /** Sink Property Choices Updated event. */
    SINK_PROPERTY_CHOICES_UPDATED(0x40000),
    /** Usb Cameras Changed event. */
    USB_CAMERAS_CHANGED(0x80000);

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
      case 0x0001 -> Kind.SOURCE_CREATED;
      case 0x0002 -> Kind.SOURCE_DESTROYED;
      case 0x0004 -> Kind.SOURCE_CONNECTED;
      case 0x0008 -> Kind.SOURCE_DISCONNECTED;
      case 0x0010 -> Kind.SOURCE_VIDEO_MODES_UPDATED;
      case 0x0020 -> Kind.SOURCE_VIDEO_MODE_CHANGED;
      case 0x0040 -> Kind.SOURCE_PROPERTY_CREATED;
      case 0x0080 -> Kind.SOURCE_PROPERTY_VALUE_UPDATED;
      case 0x0100 -> Kind.SOURCE_PROPERTY_CHOICES_UPDATED;
      case 0x0200 -> Kind.SINK_SOURCE_CHANGED;
      case 0x0400 -> Kind.SINK_CREATED;
      case 0x0800 -> Kind.SINK_DESTROYED;
      case 0x1000 -> Kind.SINK_ENABLED;
      case 0x2000 -> Kind.SINK_DISABLED;
      case 0x4000 -> Kind.NETWORK_INTERFACES_CHANGED;
      case 0x10000 -> Kind.SINK_PROPERTY_CREATED;
      case 0x20000 -> Kind.SINK_PROPERTY_VALUE_UPDATED;
      case 0x40000 -> Kind.SINK_PROPERTY_CHOICES_UPDATED;
      case 0x80000 -> Kind.USB_CAMERAS_CHANGED;
      default -> Kind.UNKNOWN;
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
   * <p>Valid for SOURCE* and SINK* respectively.
   */
  public int sourceHandle;

  /** The sink handle. */
  public int sinkHandle;

  /** Source/sink/property name. */
  public String name;

  // Fields for SOURCE_VIDEO_MODE_CHANGED event.

  /** New source video mode. */
  public VideoMode mode;

  // Fields for SOURCE_PROPERTY* events.

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
