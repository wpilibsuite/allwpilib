// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include <opencv2/core/mat.hpp>

#include "wpi/cs/ImageSource.hpp"
#include "wpi/cs/VideoMode.hpp"
#include "wpi/cs/cscore_raw.hpp"

namespace wpi::cs {

/**
 * A source for user code to provide OpenCV images as video frames.
 *
 * This is not dependent on any opencv binary ABI, and can be used
 * with versions of most versions of OpenCV.
 */
class CvSource : public ImageSource {
 public:
  CvSource() = default;

  /**
   * Create an OpenCV source.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param mode Video mode being generated
   */
  CvSource(std::string_view name, const VideoMode& mode) {
    m_handle = CreateRawSource(name, true, mode, &m_status);
  }

  /**
   * Create an  OpenCV source.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param pixelFormat Pixel format
   * @param width width
   * @param height height
   * @param fps fps
   */
  CvSource(std::string_view name, VideoMode::PixelFormat pixelFormat, int width,
           int height, int fps) {
    m_handle = CreateRawSource(
        name, true, VideoMode{pixelFormat, width, height, fps}, &m_status);
  }

  /**
   * Put an OpenCV image and notify sinks
   *
   * <p>
   * The image format is guessed from the number of channels. The channel
   * mapping is as follows. 1: kGray 2: kYUYV 3: BGR 4: BGRA Any other channel
   * numbers will throw an error. If your image is an in alternate format, use
   * the overload that takes a PixelFormat.
   *
   * @param image OpenCV Image
   */
  void PutFrame(cv::Mat& image) {
    // We only support 8-bit images; convert if necessary.
    cv::Mat finalImage;
    if (image.depth() == CV_8U) {
      finalImage = image;
    } else {
      image.convertTo(finalImage, CV_8U);
    }

    int channels = finalImage.channels();
    VideoMode::PixelFormat format;
    if (channels == 1) {
      // 1 channel is assumed Grayscale
      format = VideoMode::PixelFormat::kGray;
    } else if (channels == 2) {
      // 2 channels is assumed YUYV
      format = VideoMode::PixelFormat::kYUYV;
    } else if (channels == 3) {
      // 3 channels is assumed BGR
      format = VideoMode::PixelFormat::kBGR;
    } else if (channels == 4) {
      // 4 channels is assumed BGRA
      format = VideoMode::PixelFormat::kBGRA;
    } else {
      // TODO Error
      return;
    }

    PutFrame(finalImage, format, true);
  }

  /**
   * Put an OpenCV image and notify sinks.
   *
   * <p>
   * The format of the Mat must match the PixelFormat. You will corrupt memory
   * if they dont. With skipVerification false, we will verify the number of
   * channels matches the pixel format. If skipVerification is true, this step
   * is skipped and is passed straight through.
   *
   * @param image            OpenCV image
   * @param pixelFormat      The pixel format of the image
   * @param skipVerification skip verifying pixel format
   */
  void PutFrame(cv::Mat& image, VideoMode::PixelFormat pixelFormat,
                bool skipVerification) {
    // We only support 8-bit images; convert if necessary.
    cv::Mat finalImage;
    if (image.depth() == CV_8U) {
      finalImage = image;
    } else {
      image.convertTo(finalImage, CV_8U);
    }

    if (!skipVerification) {
      if (!VerifyFormat(finalImage, pixelFormat)) {
        // TODO Error
        return;
      }
    }

    WPI_RawFrame frame;  // use WPI_Frame because we don't want the destructor
    frame.data = finalImage.data;
    frame.freeFunc = nullptr;
    frame.freeCbData = nullptr;
    frame.size = finalImage.total() * finalImage.channels();
    frame.width = finalImage.cols;
    frame.height = finalImage.rows;
    frame.stride = finalImage.step;
    frame.pixelFormat = pixelFormat;
    m_status = 0;
    PutSourceFrame(m_handle, frame, &m_status);
  }

 private:
  static bool VerifyFormat(cv::Mat& image, VideoMode::PixelFormat pixelFormat) {
    int channels = image.channels();
    switch (pixelFormat) {
      case VideoMode::PixelFormat::kBGR:
        if (channels == 3) {
          return true;
        }
        break;
      case VideoMode::PixelFormat::kBGRA:
        if (channels == 4) {
          return true;
        }
        break;
      case VideoMode::PixelFormat::kGray:
        if (channels == 1) {
          return true;
        }
        break;
      case VideoMode::PixelFormat::kRGB565:
        if (channels == 2) {
          return true;
        }
        break;
      case VideoMode::PixelFormat::kUYVY:
        if (channels == 2) {
          return true;
        }
        break;
      case VideoMode::PixelFormat::kY16:
        if (channels == 2) {
          return true;
        }
        break;
      case VideoMode::PixelFormat::kYUYV:
        if (channels == 2) {
          return true;
        }
        break;
      case VideoMode::PixelFormat::kMJPEG:
        if (channels == 1) {
          return true;
        }
        break;
      default:
        break;
    }
    return false;
  }
};

}  // namespace wpi::cs
