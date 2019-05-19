/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore;

public abstract class ImageSink extends VideoSink {
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
   * Get error string.  Call this if WaitForFrame() returns 0 to determine
   * what the error is.
   */
  public String getError() {
    return CameraServerJNI.getSinkError(m_handle);
  }

  /**
   * Enable or disable getting new frames.
   * Disabling will cause processFrame (for callback-based CvSinks) to not
   * be called and WaitForFrame() to not return.  This can be used to save
   * processor resources when frames are not needed.
   */
  public void setEnabled(boolean enabled) {
    CameraServerJNI.setSinkEnabled(m_handle, enabled);
  }
}
