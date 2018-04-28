/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore;

/**
 * A source for video that provides a sequence of frames.  Each frame may
 * consist of multiple images (e.g. from a stereo or depth camera); these
 * are called channels.
 */
public class VideoSink {
  public enum Kind {
    kUnknown(0), kMjpeg(2), kCv(4);
    private int value;

    private Kind(int value) {
      this.value = value;
    }

    public int getValue() {
      return value;
    }
  }

  public static Kind getKindFromInt(int kind) {
    switch (kind) {
      case 2: return Kind.kMjpeg;
      case 4: return Kind.kCv;
      default: return Kind.kUnknown;
    }
  }

  protected VideoSink(int handle) {
    m_handle = handle;
  }

  public synchronized void free() {
    if (m_handle != 0) {
      CameraServerJNI.releaseSink(m_handle);
    }
    m_handle = 0;
  }

  public boolean isValid() {
    return m_handle != 0;
  }

  public int getHandle() {
    return m_handle;
  }

  public boolean equals(Object other) {
    if (this == other) return true;
    if (other == null) return false;
    if (getClass() != other.getClass()) return false;
    VideoSink sink = (VideoSink) other;
    return m_handle == sink.m_handle;
  }

  public int hashCode() {
    return m_handle;
  }

  /**
   * Get the kind of the sink.
   */
  public Kind getKind() {
    return getKindFromInt(CameraServerJNI.getSinkKind(m_handle));
  }

  /**
   * Get the name of the sink.  The name is an arbitrary identifier
   * provided when the sink is created, and should be unique.
   */
  public String getName() {
    return CameraServerJNI.getSinkName(m_handle);
  }

  /**
   * Get the sink description.  This is sink-kind specific.
   */
  public String getDescription() {
    return CameraServerJNI.getSinkDescription(m_handle);
  }

  /**
   * Configure which source should provide frames to this sink.  Each sink
   * can accept frames from only a single source, but a single source can
   * provide frames to multiple clients.
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
   * @return Connected source; nullptr if no source connected.
   */
  public VideoSource getSource() {
    // While VideoSource.free() will call releaseSource(), getSinkSource()
    // increments the internal reference count so this is okay to do.
    return new VideoSource(CameraServerJNI.getSinkSource(m_handle));
  }

  /**
   * Get a property of the associated source.
   * @param name Property name
   * @return Property (kind Property::kNone if no property with
   *         the given name exists or no source connected)
   */
  public VideoProperty getSourceProperty(String name) {
    return new VideoProperty(
        CameraServerJNI.getSinkSourceProperty(m_handle, name));
  }

  /**
   * Enumerate all existing sinks.
   * @return Vector of sinks.
   */
  public static VideoSink[] enumerateSinks() {
    int[] handles = CameraServerJNI.enumerateSinks();
    VideoSink[] rv = new VideoSink[handles.length];
    for (int i=0; i<handles.length; i++) {
      rv[i] = new VideoSink(handles[i]);
    }
    return rv;
  }

  protected int m_handle;
}
