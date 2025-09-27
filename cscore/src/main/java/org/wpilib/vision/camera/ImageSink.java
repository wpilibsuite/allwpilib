// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

/** A base class for single image reading sinks. */
public abstract class ImageSink extends VideoSink {
  /**
   * Constructs an ImageSink.
   *
   * @param handle The image sink handle.
   */
  protected ImageSink(int handle) {
    super(handle);
  }

  /**
   * Set sink description.
   *
   * @param description Description
   */
  public void setDescription(String description) {
    CameraServerJNI.setSinkDescription(m_handle, description);
  }

  /**
   * Get error string. Call this if WaitForFrame() returns 0 to determine what the error is.
   *
   * @return Error string.
   */
  public String getError() {
    return CameraServerJNI.getSinkError(m_handle);
  }

  /**
   * Enable or disable getting new frames. Disabling will cause processFrame (for callback-based
   * CvSinks) to not be called and WaitForFrame() to not return. This can be used to save processor
   * resources when frames are not needed.
   *
   * @param enabled Enable to get new frames.
   */
  public void setEnabled(boolean enabled) {
    CameraServerJNI.setSinkEnabled(m_handle, enabled);
  }
}
