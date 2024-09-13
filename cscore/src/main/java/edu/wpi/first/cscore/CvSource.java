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
            name, true, mode.pixelFormat.getValue(), mode.width, mode.height, mode.fps));
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
    super(CameraServerJNI.createRawSource(name, true, pixelFormat.getValue(), width, height, fps));
    OpenCvLoader.forceStaticLoad();
  }

  /**
   * Put an OpenCV image and notify sinks
   *
   * <p>The image format is guessed from the number of channels. The channel mapping is as follows.
   * 1: kGray 2: kYUYV 3: BGR 4: BGRA Any other channel numbers will throw an error. If your image
   * is an in alternate format, use the overload that takes a PixelFormat.
   *
   * @param image OpenCV Image
   */
  public void putFrame(Mat image) {
    // We only support 8 bit channels
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
      PixelFormat format =
          switch (channels) {
            case 1 -> PixelFormat.kGray; // 1 channel is assumed Grayscale
            case 2 -> PixelFormat.kYUYV; // 2 channels is assumed YUYV
            case 3 -> PixelFormat.kBGR; // 3 channels is assumed BGR
            case 4 -> PixelFormat.kBGRA; // 4 channels is assumed BGRA
            default -> throw new VideoException(
                "Unable to get pixel format for " + channels + " channels");
          };

      putFrame(finalImage, format, true);
    } finally {
      if (cleanupRequired) {
        finalImage.release();
      }
    }
  }

  /**
   * Put an OpenCV image and notify sinks.
   *
   * <p>The format of the Mat must match the PixelFormat. You will corrupt memory if they dont. With
   * skipVerification false, we will verify the number of channels matches the pixel format. If
   * skipVerification is true, this step is skipped and is passed straight through.
   *
   * @param image OpenCV image
   * @param format The pixel format of the image
   * @param skipVerification skip verifying pixel format
   */
  public void putFrame(Mat image, PixelFormat format, boolean skipVerification) {
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
      if (!skipVerification) {
        verifyFormat(finalImage, format);
      }
      long step = image.step1() * image.elemSize1();
      CameraServerJNI.putRawSourceFrameData(
          m_handle,
          finalImage.dataAddr(),
          (int) finalImage.total() * finalImage.channels(),
          finalImage.width(),
          finalImage.height(),
          (int) step,
          format.getValue());

    } finally {
      if (cleanupRequired) {
        finalImage.release();
      }
    }
  }

  private void verifyFormat(Mat image, PixelFormat pixelFormat) {
    int channels = image.channels();
    switch (pixelFormat) {
      case kBGR:
        if (channels == 3) {
          return;
        }
        break;
      case kBGRA:
        if (channels == 4) {
          return;
        }
        break;
      case kGray:
        if (channels == 1) {
          return;
        }
        break;
      case kRGB565:
        if (channels == 2) {
          return;
        }
        break;
      case kUYVY:
        if (channels == 2) {
          return;
        }
        break;
      case kY16:
        if (channels == 2) {
          return;
        }
        break;
      case kYUYV:
        if (channels == 2) {
          return;
        }
        break;
      case kMJPEG:
        if (channels == 1) {
          return;
        }
        break;
      default:
        break;
    }
  }
}
