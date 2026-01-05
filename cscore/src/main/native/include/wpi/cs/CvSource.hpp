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
  CvSource(std::string_view name, wpi::util::PixelFormat pixelFormat, int width,
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
    wpi::util::PixelFormat format;
    if (channels == 1) {
      // 1 channel is assumed Grayscale
      format = wpi::util::PixelFormat::kGray;
    } else if (channels == 2) {
      // 2 channels is assumed YUYV
      format = wpi::util::PixelFormat::kYUYV;
    } else if (channels == 3) {
      // 3 channels is assumed BGR
      format = wpi::util::PixelFormat::kBGR;
    } else if (channels == 4) {
      // 4 channels is assumed BGRA
      format = wpi::util::PixelFormat::kBGRA;
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
  void PutFrame(cv::Mat& image, wpi::util::PixelFormat pixelFormat,
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
    frame.pixelFormat = static_cast<int>(pixelFormat);
    m_status = 0;
    PutSourceFrame(m_handle, frame, &m_status);
  }

 private:
  static bool VerifyFormat(cv::Mat& image, wpi::util::PixelFormat pixelFormat) {
    int channels = image.channels();
    switch (pixelFormat) {
      case wpi::util::PixelFormat::kBGR:
        if (channels == 3) {
          return true;
        }
        break;
      case wpi::util::PixelFormat::kBGRA:
        if (channels == 4) {
          return true;
        }
        break;
      case wpi::util::PixelFormat::kGray:
        if (channels == 1) {
          return true;
        }
        break;
      case wpi::util::PixelFormat::kRGB565:
        if (channels == 2) {
          return true;
        }
        break;
      case wpi::util::PixelFormat::kUYVY:
        if (channels == 2) {
          return true;
        }
        break;
      case wpi::util::PixelFormat::kY16:
        if (channels == 2) {
          return true;
        }
        break;
      case wpi::util::PixelFormat::kYUYV:
        if (channels == 2) {
          return true;
        }
        break;
      case wpi::util::PixelFormat::kMJPEG:
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
