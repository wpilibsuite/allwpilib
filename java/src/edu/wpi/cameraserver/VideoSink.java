/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cameraserver;

/// A source for video that provides a sequence of frames.  Each frame may
/// consist of multiple images (e.g. from a stereo or depth camera); these
/// are called channels.
public class VideoSink {
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

  /// Get the name of the sink.  The name is an arbitrary identifier
  /// provided when the sink is created, and should be unique.
  public String getName() {
    return CameraServerJNI.getSinkName(m_handle);
  }

  /// Get the sink description.  This is sink-type specific.
  public String getDescription() {
    return CameraServerJNI.getSinkDescription(m_handle);
  }

  /// Configure which source should provide frames to this sink.  Each sink
  /// can accept frames from only a single source, but a single source can
  /// provide frames to multiple clients.
  /// @param source Source
  public void setSource(VideoSource source) {
    CameraServerJNI.setSinkSource(m_handle, source.m_handle);
  }

  /// Get the connected source.
  /// @return Connected source; nullptr if no source connected.
  public VideoSource getSource() {
    // While VideoSource.free() will call releaseSource(), getSinkSource()
    // increments the internal reference count so this is okay to do.
    return new VideoSource(CameraServerJNI.getSinkSource(m_handle));
  }

  /// Get a property of the associated source.
  /// @param name Property name
  /// @return Property (type Property::kNone if no property with
  ///         the given name exists or no source connected)
  public VideoProperty getSourceProperty(String name) {
    return new VideoProperty(CameraServerJNI.getSourceProperty(m_handle, name));  }

  /// Enumerate all existing sinks.
  /// @return Vector of sinks.
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
