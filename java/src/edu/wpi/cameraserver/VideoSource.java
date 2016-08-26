package edu.wpi.cameraserver;

/// A source for video that provides a sequence of frames.  Each frame may
/// consist of multiple images (e.g. from a stereo or depth camera); these
/// are called channels.
public class VideoSource {
  protected VideoSource(int handle) {
    m_handle = handle;
  }

  public synchronized void free() {
    if (m_handle != 0) {
      CameraServerJNI.releaseSource(m_handle);
    }
    m_handle = 0;
  }

  public boolean isValid() {
    return m_handle != 0;
  }

  /// Get the name of the source.  The name is an arbitrary identifier
  /// provided when the source is created, and should be unique.
  public String getName() {
    return CameraServerJNI.getSourceName(m_handle);
  }

  /// Get the source description.  This is source-type specific.
  public String getDescription() {
    return CameraServerJNI.getSourceDescription(m_handle);
  }

  /// Get the last time a frame was captured.
  public long getLastFrameTime() {
    return CameraServerJNI.getSourceLastFrameTime(m_handle);
  }

  /// Get the number of channels this source provides.
  public int getNumChannels() {
    return CameraServerJNI.getSourceNumChannels(m_handle);
  }

  /// Is the source currently connected to whatever is providing the images?
  public boolean isConnected() {
    return CameraServerJNI.isSourceConnected(m_handle);
  }

  /// Get a property.
  /// @param name Property name
  /// @return Property contents (of type Property::kNone if no property with
  ///         the given name exists)
  public VideoProperty getProperty(String name) {
    return new VideoProperty(CameraServerJNI.getSourceProperty(m_handle, name));
  }

  /// Enumerate all properties of this source.
  public VideoProperty[] enumerateProperties() {
    int[] handles = CameraServerJNI.enumerateSourceProperties(m_handle);
    VideoProperty[] rv = new VideoProperty[handles.length];
    for (int i=0; i<handles.length; i++) {
      rv[i] = new VideoProperty(handles[i]);
    }
    return rv;
  }

  /// Enumerate all existing sources.
  /// @return Vector of sources.
  public static VideoSource[] enumerateSources() {
    int[] handles = CameraServerJNI.enumerateSources();
    VideoSource[] rv = new VideoSource[handles.length];
    for (int i=0; i<handles.length; i++) {
      rv[i] = new VideoSource(handles[i]);
    }
    return rv;
  }

  protected int m_handle;
}
