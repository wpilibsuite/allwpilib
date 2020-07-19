/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore;

/**
 * A source that represents a USB camera.
 */
public class UsbCamera extends VideoCamera {
  /**
   * Create a source for a USB camera based on device number.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param dev Device number (e.g. 0 for /dev/video0)
   */
  public UsbCamera(String name, int dev) {
    super(CameraServerJNI.createUsbCameraDev(name, dev));
  }

  /**
   * Create a source for a USB camera based on device path.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param path Path to device (e.g. "/dev/video0" on Linux)
   */
  public UsbCamera(String name, String path) {
    super(CameraServerJNI.createUsbCameraPath(name, path));
  }

  /**
   * Enumerate USB cameras on the local system.
   *
   * @return Vector of USB camera information (one for each camera)
   */
  public static UsbCameraInfo[] enumerateUsbCameras() {
    return CameraServerJNI.enumerateUsbCameras();
  }

  /**
   * Change the path to the device.
   */
  void setPath(String path) {
    CameraServerJNI.setUsbCameraPath(m_handle, path);
  }

  /**
   * Get the path to the device.
   */
  public String getPath() {
    return CameraServerJNI.getUsbCameraPath(m_handle);
  }

  /**
   * Get the full camera information for the device.
   */
  public UsbCameraInfo getInfo() {
    return CameraServerJNI.getUsbCameraInfo(m_handle);
  }

  /**
   * Set how verbose the camera connection messages are.
   *
   * @param level 0=don't display Connecting message, 1=do display message
   */
  public void setConnectVerbose(int level) {
    CameraServerJNI.setProperty(CameraServerJNI.getSourceProperty(m_handle, "connect_verbose"),
                                level);
  }
}
