// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

/** USB camera information. */
@SuppressWarnings("MemberName")
public class UsbCameraInfo {
  /**
   * Create a new set of UsbCameraInfo.
   *
   * @param dev Device number (e.g. N in '/dev/videoN' on Linux)
   * @param path Path to device if available (e.g. '/dev/video0' on Linux)
   * @param name Vendor/model name of the camera as provided by the USB driver
   * @param otherPaths Other path aliases to device
   * @param vendorId USB vendor id
   * @param productId USB product id
   */
  @SuppressWarnings("PMD.ArrayIsStoredDirectly")
  public UsbCameraInfo(
      int dev, String path, String name, String[] otherPaths, int vendorId, int productId) {
    this.dev = dev;
    this.path = path;
    this.name = name;
    this.otherPaths = otherPaths;
    this.vendorId = vendorId;
    this.productId = productId;
  }

  /** Device number (e.g. N in '/dev/videoN' on Linux). */
  public int dev;

  /** Path to device if available (e.g. '/dev/video0' on Linux). */
  public String path;

  /** Vendor/model name of the camera as provided by the USB driver. */
  public String name;

  /** Other path aliases to device (e.g. '/dev/v4l/by-id/...' etc on Linux). */
  public String[] otherPaths;

  /** USB vendor id. */
  public int vendorId;

  /** USB product id. */
  public int productId;
}
