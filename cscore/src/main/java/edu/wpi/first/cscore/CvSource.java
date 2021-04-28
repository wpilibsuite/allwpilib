// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

import org.opencv.core.Mat;

/**
 * A source that represents a video camera. These sources require the WPILib OpenCV builds. For an
 * alternate OpenCV, see the documentation how to build your own with RawSource.
 */
public class CvSource extends ImageSource {
  /**
   * Create an OpenCV source.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param mode Video mode being generated
   */
  public CvSource(String name, VideoMode mode) {
    super(
        CameraServerCvJNI.createCvSource(
            name, mode.pixelFormat.getValue(), mode.width, mode.height, mode.fps));
  }

  /**
   * Create an OpenCV source.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param pixelFormat Pixel format
   * @param width width
   * @param height height
   * @param fps fps
   */
  public CvSource(String name, VideoMode.PixelFormat pixelFormat, int width, int height, int fps) {
    super(CameraServerCvJNI.createCvSource(name, pixelFormat.getValue(), width, height, fps));
  }

  /**
   * Put an OpenCV image and notify sinks.
   *
   * <p>Only 8-bit single-channel or 3-channel (with BGR channel order) images are supported. If the
   * format, depth or channel order is different, use Mat.convertTo() and/or cvtColor() to convert
   * it first.
   *
   * @param image OpenCV image
   */
  public void putFrame(Mat image) {
    CameraServerCvJNI.putSourceFrame(m_handle, image.nativeObj);
  }
}
