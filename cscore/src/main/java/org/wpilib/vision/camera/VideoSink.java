// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

/**
 * A sink for video that accepts a sequence of frames. Each frame may consist of multiple images
 * (e.g. from a stereo or depth camera); these are called channels.
 */
public class VideoSink implements AutoCloseable {
  /** Video sink types. */
  public enum Kind {
    /** Unknown video sink type. */
    kUnknown(0),
    /** MJPEG video sink. */
    kMjpeg(2),
    /** CV video sink. */
    kCv(4),
    /** Raw video sink. */
    kRaw(8);

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
      case 2 -> Kind.kMjpeg;
      case 4 -> Kind.kCv;
      case 8 -> Kind.kRaw;
      default -> Kind.kUnknown;
    };
  }

  /**
   * Constructs a VideoSink.
   *
   * @param handle The video sink handle.
   */
  protected VideoSink(int handle) {
    m_handle = handle;
  }

  @Override
  public synchronized void close() {
    if (m_handle != 0) {
      CameraServerJNI.releaseSink(m_handle);
    }
    m_handle = 0;
  }

  /**
   * Returns true if the VideoSink is valid.
   *
   * @return True if the VideoSink is valid.
   */
  public boolean isValid() {
    return m_handle != 0;
  }

  /**
   * Returns the video sink handle.
   *
   * @return The video sink handle.
   */
  public int getHandle() {
    return m_handle;
  }

  @Override
  public boolean equals(Object other) {
    if (this == other) {
      return true;
    }
    if (other == null) {
      return false;
    }
    return other instanceof VideoSink sink && m_handle == sink.m_handle;
  }

  @Override
  public int hashCode() {
    return m_handle;
  }

  /**
   * Get the kind of the sink.
   *
   * @return The kind of the sink.
   */
  public Kind getKind() {
    return getKindFromInt(CameraServerJNI.getSinkKind(m_handle));
  }

  /**
   * Get the name of the sink. The name is an arbitrary identifier provided when the sink is
   * created, and should be unique.
   *
   * @return The name of the sink.
   */
  public String getName() {
    return CameraServerJNI.getSinkName(m_handle);
  }

  /**
   * Get the sink description. This is sink-kind specific.
   *
   * @return The sink description.
   */
  public String getDescription() {
    return CameraServerJNI.getSinkDescription(m_handle);
  }

  /**
   * Get a property of the sink.
   *
   * @param name Property name
   * @return Property (kind Property::kNone if no property with the given name exists)
   */
  public VideoProperty getProperty(String name) {
    return new VideoProperty(CameraServerJNI.getSinkProperty(m_handle, name));
  }

  /**
   * Enumerate all properties of this sink.
   *
   * @return List of properties.
   */
  public VideoProperty[] enumerateProperties() {
    int[] handles = CameraServerJNI.enumerateSinkProperties(m_handle);
    VideoProperty[] rv = new VideoProperty[handles.length];
    for (int i = 0; i < handles.length; i++) {
      rv[i] = new VideoProperty(handles[i]);
    }
    return rv;
  }

  /**
   * Set properties from a JSON configuration string.
   *
   * <p>The format of the JSON input is:
   *
   * <pre>
   * {
   *     "properties": [
   *         {
   *             "name": property name
   *             "value": property value
   *         }
   *     ]
   * }
   * </pre>
   *
   * @param config configuration
   * @return True if set successfully
   */
  public boolean setConfigJson(String config) {
    return CameraServerJNI.setSinkConfigJson(m_handle, config);
  }

  /**
   * Get a JSON configuration string.
   *
   * @return JSON configuration string
   */
  public String getConfigJson() {
    return CameraServerJNI.getSinkConfigJson(m_handle);
  }

  /**
   * Configure which source should provide frames to this sink. Each sink can accept frames from
   * only a single source, but a single source can provide frames to multiple clients.
   *
   * @param source Source
   */
  public void setSource(VideoSource source) {
    if (source == null) {
      CameraServerJNI.setSinkSource(m_handle, 0);
    } else {
      CameraServerJNI.setSinkSource(m_handle, source.m_handle);
    }
  }

  /**
   * Get the connected source.
   *
   * @return Connected source; nullptr if no source connected.
   */
  public VideoSource getSource() {
    // While VideoSource.close() will call releaseSource(), getSinkSource()
    // increments the internal reference count so this is okay to do.
    return new VideoSource(CameraServerJNI.getSinkSource(m_handle));
  }

  /**
   * Get a property of the associated source.
   *
   * @param name Property name
   * @return Property (kind Property::kNone if no property with the given name exists or no source
   *     connected)
   */
  public VideoProperty getSourceProperty(String name) {
    return new VideoProperty(CameraServerJNI.getSinkSourceProperty(m_handle, name));
  }

  /**
   * Enumerate all existing sinks.
   *
   * @return Vector of sinks.
   */
  public static VideoSink[] enumerateSinks() {
    int[] handles = CameraServerJNI.enumerateSinks();
    VideoSink[] rv = new VideoSink[handles.length];
    for (int i = 0; i < handles.length; i++) {
      rv[i] = new VideoSink(handles[i]);
    }
    return rv;
  }

  /** The VideoSink handle. */
  protected int m_handle;
}
