/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore.raw;

import edu.wpi.cscore.CameraServerJNI;
import edu.wpi.cscore.ImageSource;
import edu.wpi.cscore.VideoMode;

public class RawSource extends ImageSource {
  public RawSource(String name, VideoMode mode) {
    super(CameraServerJNI.createRawSource(name,
        mode.pixelFormat.getValue(),
        mode.width, mode.height,
        mode.fps));
  }

  public RawSource(String name, VideoMode.PixelFormat pixelFormat, int width, int height, int fps) {
    super(CameraServerJNI.createRawSource(name,
        pixelFormat.getValue(),
        width, height,
        fps));
  }

  protected void putFrame(RawFrame image) {
    CameraServerJNI.putRawSourceFrame(m_handle, image);
  }

  protected void putFrame(long data, int width, int height, int pixelFormat, int totalData) {
    CameraServerJNI.putRawSourceFrame(m_handle, data, width, height, pixelFormat, totalData);
  }

  protected void putFrame(long data, int width, int height, VideoMode.PixelFormat pixelFormat, int totalData) {
    CameraServerJNI.putRawSourceFrame(m_handle, data, width, height, pixelFormat.getValue(), totalData);
  }
}
