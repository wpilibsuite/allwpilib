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
public class VideoSource {
  public enum Kind {
    kUnknown(0), kUsb(1), kHttp(2), kCv(4);
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
      case 1: return Kind.kUsb;
      case 2: return Kind.kHttp;
      case 4: return Kind.kCv;
      default: return Kind.kUnknown;
    }
  }

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

  public int getHandle() {
    return m_handle;
  }

  public boolean equals(Object other) {
    if (this == other) return true;
    if (other == null) return false;
    if (getClass() != other.getClass()) return false;
    VideoSource source = (VideoSource) other;
    return m_handle == source.m_handle;
  }

  public int hashCode() {
    return m_handle;
  }

  /**
   * Get the kind of the source.
   */
  public Kind getKind() {
    return getKindFromInt(CameraServerJNI.getSourceKind(m_handle));
  }

  /**
   * Get the name of the source.  The name is an arbitrary identifier
   * provided when the source is created, and should be unique.
   */
  public String getName() {
    return CameraServerJNI.getSourceName(m_handle);
  }

  /**
   * Get the source description.  This is source-kind specific.
   */
  public String getDescription() {
    return CameraServerJNI.getSourceDescription(m_handle);
  }

  /**
   * Get the last time a frame was captured.
   * @return Time in 1 us increments.
   */
  public long getLastFrameTime() {
    return CameraServerJNI.getSourceLastFrameTime(m_handle);
  }

  /**
   * Is the source currently connected to whatever is providing the images?
   */
  public boolean isConnected() {
    return CameraServerJNI.isSourceConnected(m_handle);
  }

  /**
   * Get a property.
   * @param name Property name
   * @return Property contents (of kind Property::kNone if no property with
   *         the given name exists)
   */
  public VideoProperty getProperty(String name) {
    return new VideoProperty(CameraServerJNI.getSourceProperty(m_handle, name));
  }

  /**
   * Enumerate all properties of this source.
   */
  public VideoProperty[] enumerateProperties() {
    int[] handles = CameraServerJNI.enumerateSourceProperties(m_handle);
    VideoProperty[] rv = new VideoProperty[handles.length];
    for (int i=0; i<handles.length; i++) {
      rv[i] = new VideoProperty(handles[i]);
    }
    return rv;
  }

  /**
   * Get the current video mode.
   */
  public VideoMode getVideoMode() {
    return CameraServerJNI.getSourceVideoMode(m_handle);
  }

  /**
   * Set the video mode.
   * @param mode Video mode
   */
  public boolean setVideoMode(VideoMode mode) {
    return CameraServerJNI.setSourceVideoMode(m_handle, mode.pixelFormat.getValue(), mode.width, mode.height, mode.fps);
  }

  /**
   * Set the video mode.
   * @param pixelFormat desired pixel format
   * @param width desired width
   * @param height desired height
   * @param fps desired FPS
   * @return True if set successfully
   */
  public boolean setVideoMode(VideoMode.PixelFormat pixelFormat, int width, int height, int fps) {
    return CameraServerJNI.setSourceVideoMode(m_handle, pixelFormat.getValue(), width, height, fps);
  }

  /**
   * Set the pixel format.
   * @param pixelFormat desired pixel format
   * @return True if set successfully
   */
  public boolean setPixelFormat(VideoMode.PixelFormat pixelFormat) {
    return CameraServerJNI.setSourcePixelFormat(m_handle, pixelFormat.getValue());
  }

  /**
   * Set the resolution.
   * @param width desired width
   * @param height desired height
   * @return True if set successfully
   */
  public boolean setResolution(int width, int height) {
    return CameraServerJNI.setSourceResolution(m_handle, width, height);
  }

  /**
   * Set the frames per second (FPS).
   * @param fps desired FPS
   * @return True if set successfully
   */
  public boolean setFPS(int fps) {
    return CameraServerJNI.setSourceFPS(m_handle, fps);
  }

  /**
   * Enumerate all known video modes for this source.
   */
  public VideoMode[] enumerateVideoModes() {
    return CameraServerJNI.enumerateSourceVideoModes(m_handle);
  }

  /**
   * Enumerate all sinks connected to this source.
   * @return Vector of sinks.
   */
  public VideoSink[] enumerateSinks() {
    int[] handles = CameraServerJNI.enumerateSourceSinks(m_handle);
    VideoSink[] rv = new VideoSink[handles.length];
    for (int i=0; i<handles.length; i++) {
      rv[i] = new VideoSink(handles[i]);
    }
    return rv;
  }

  /**
   * Enumerate all existing sources.
   * @return Vector of sources.
   */
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
