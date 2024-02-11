// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

import edu.wpi.first.util.PixelFormat;
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
        CameraServerJNI.createRawSource(
            name, mode.pixelFormat.getValue(), mode.width, mode.height, mode.fps));
    OpenCvLoader.forceStaticLoad();
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
  public CvSource(String name, PixelFormat pixelFormat, int width, int height, int fps) {
    super(CameraServerJNI.createRawSource(name, pixelFormat.getValue(), width, height, fps));
    OpenCvLoader.forceStaticLoad();
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
    // We only support 8-bit images, convert if necessary
    boolean cleanupRequired = false;
    Mat finalImage;
    if (image.depth() == 0) {
      finalImage = image;
    } else {
      finalImage = new Mat();
      image.convertTo(finalImage, 0);
      cleanupRequired = true;
    }

    try {
      int channels = finalImage.channels();
      PixelFormat format;
      if (channels == 1) {
        format = PixelFormat.kGray;
      } else if (channels == 3) {
        format = PixelFormat.kBGR;
      } else {
        throw new VideoException("Unsupported image type");
      }
      // TODO old code supported BGRA, but the only way I can support that is slow.
      // Update cscore to support BGRA for raw frames

      CameraServerJNI.putRawSourceFrameData(
          m_handle,
          finalImage.dataAddr(),
          (int) finalImage.total() * channels,
          finalImage.width(),
          finalImage.height(),
          image.width(),
          format.getValue());

    } finally {
      if (cleanupRequired) {
        finalImage.release();
      }
    }
  }
}
