// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.cscore;

import edu.wpi.cscore.VideoMode.PixelFormat;
import org.opencv.core.Mat;

public class RawCVMatSource extends ImageSource {
  /**
   * Create an OpenCV source.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param mode Video mode being generated
   */
  public RawCVMatSource(String name, VideoMode mode) {
    super(
        CameraServerJNI.createRawSource(
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
  public RawCVMatSource(
      String name, VideoMode.PixelFormat pixelFormat, int width, int height, int fps) {
    super(CameraServerJNI.createRawSource(name, pixelFormat.getValue(), width, height, fps));
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
    int channels = image.channels();
    if (channels != 1 && channels != 3) {
      throw new VideoException("Unsupported Image Type");
    }
    int imgType = channels == 1 ? PixelFormat.kGray.getValue() : PixelFormat.kBGR.getValue();
    CameraServerJNI.putRawSourceFrame(
        m_handle,
        image.dataAddr(),
        image.width(),
        image.height(),
        imgType,
        (int) image.total() * channels);
  }
}
