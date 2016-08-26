/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cameraserver;

/// USB camera information
public class USBCameraInfo {
  public USBCameraInfo(int dev, String path, String name, int channels) {
    this.dev = dev;
    this.path = path;
    this.name = name;
    this.channels = channels;
  }

  /// Device number (e.g. N in '/dev/videoN' on Linux)
  public int dev;
  /// Path to device if available (e.g. '/dev/video0' on Linux)
  public String path;
  /// Vendor/model name of the camera as provided by the USB driver
  public String name;
  /// Number of channels the camera provides (usually 1, but some cameras such
  /// as stereo or depth cameras may provide multiple channels).
  public int channels;
}
