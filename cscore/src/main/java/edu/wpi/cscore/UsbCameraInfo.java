/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore;

/**
 * USB camera information.
 */
public class UsbCameraInfo {
  /**
   * Create a new set of UsbCameraInfo.
   *
   * @param dev Device number (e.g. N in '/dev/videoN' on Linux)
   * @param path Path to device if available (e.g. '/dev/video0' on Linux)
   * @param name Vendor/model name of the camera as provided by the USB driver
   * @param otherPaths Other path aliases to device
   */
  @SuppressWarnings("PMD.ArrayIsStoredDirectly")
  public UsbCameraInfo(int dev, String path, String name, String[] otherPaths) {
    this.dev = dev;
    this.path = path;
    this.name = name;
    this.otherPaths = otherPaths;
  }

  /**
   * Device number (e.g. N in '/dev/videoN' on Linux).
   */
  @SuppressWarnings("MemberName")
  public int dev;

  /**
   * Path to device if available (e.g. '/dev/video0' on Linux).
   */
  @SuppressWarnings("MemberName")
  public String path;

  /**
   * Vendor/model name of the camera as provided by the USB driver.
   */
  @SuppressWarnings("MemberName")
  public String name;

  /**
   * Other path aliases to device (e.g. '/dev/v4l/by-id/...' etc on Linux).
   */
  @SuppressWarnings("MemberName")
  public String[] otherPaths;
}
