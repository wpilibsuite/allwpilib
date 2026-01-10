// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string_view>

#include <opencv2/core/mat.hpp>

#include "wpi/cs/ImageSink.hpp"
#include "wpi/cs/cscore_raw.hpp"
#include "wpi/util/PixelFormat.hpp"
#include "wpi/util/RawFrame.hpp"

namespace wpi::cs {

/**
 * A sink for user code to accept video frames as OpenCV images.
 *
 * This is not dependent on any opencv binary ABI, and can be used
 * with versions of most versions of OpenCV.
 */
class CvSink : public ImageSink {
 public:
  CvSink() = default;
  CvSink(const CvSink& sink) : ImageSink{sink}, pixelFormat{sink.pixelFormat} {}

  /**
   * Create a sink for accepting OpenCV images.
   *
   * <p>WaitForFrame() must be called on the created sink to get each new
   * image.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param pixelFormat The pixel format to read
   */
  explicit CvSink(std::string_view name, wpi::util::PixelFormat pixelFormat =
                                             wpi::util::PixelFormat::kBGR) {
    m_handle = CreateRawSink(name, true, &m_status);
    this->pixelFormat = pixelFormat;
  }

  /**
   * Wait for the next frame and get the image.
   * Times out (returning 0) after timeout seconds.
   * The provided image will have the pixelFormat this class was constructed
   * with.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in the same time base as
   * wpi::util::Now(), and is in 1 us increments.
   */
  [[nodiscard]]
  uint64_t GrabFrame(cv::Mat& image, double timeout = 0.225) {
    cv::Mat tmpnam;
    auto retVal = GrabFrameDirect(tmpnam);
    if (retVal <= 0) {
      return retVal;
    }
    tmpnam.copyTo(image);
    return retVal;
  }

  /**
   * Wait for the next frame and get the image.  May block forever.
   * The provided image will have the pixelFormat this class was constructed
   * with.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in the same time base as
   * wpi::util::Now(), and is in 1 us increments.
   */
  [[nodiscard]]
  uint64_t GrabFrameNoTimeout(cv::Mat& image) {
    cv::Mat tmpnam;
    auto retVal = GrabFrameNoTimeoutDirect(tmpnam);
    if (retVal <= 0) {
      return retVal;
    }
    tmpnam.copyTo(image);
    return retVal;
  }

  /**
   * Wait for the next frame and get the image.
   * Times out (returning 0) after timeout seconds.
   * The provided image will have the pixelFormat this class was constructed
   * with. The data is backed by data in the CvSink. It will be invalidated by
   * any grabFrame*() call on the sink.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in the same time base as
   * wpi::util::Now(), and is in 1 us increments.
   */
  [[nodiscard]]
  uint64_t GrabFrameDirect(cv::Mat& image, double timeout = 0.225) {
    rawFrame.height = 0;
    rawFrame.width = 0;
    rawFrame.stride = 0;
    rawFrame.pixelFormat = static_cast<int>(pixelFormat);
    auto timestamp =
        GrabSinkFrameTimeout(m_handle, rawFrame, timeout, &m_status);
    if (m_status != CS_OK) {
      return 0;
    }
    image =
        cv::Mat{rawFrame.height, rawFrame.width,
                GetCvFormat(static_cast<WPI_PixelFormat>(rawFrame.pixelFormat)),
                rawFrame.data, static_cast<size_t>(rawFrame.stride)};
    return timestamp;
  }

  /**
   * Wait for the next frame and get the image.  May block forever.
   * The provided image will have the pixelFormat this class was constructed
   * with. The data is backed by data in the CvSink. It will be invalidated by
   * any grabFrame*() call on the sink.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in the same time base as
   * wpi::util::Now(), and is in 1 us increments.
   */
  [[nodiscard]]
  uint64_t GrabFrameNoTimeoutDirect(cv::Mat& image) {
    rawFrame.height = 0;
    rawFrame.width = 0;
    rawFrame.stride = 0;
    rawFrame.pixelFormat = static_cast<int>(pixelFormat);
    auto timestamp = GrabSinkFrame(m_handle, rawFrame, &m_status);
    if (m_status != CS_OK) {
      return 0;
    }
    image =
        cv::Mat{rawFrame.height, rawFrame.width,
                GetCvFormat(static_cast<WPI_PixelFormat>(rawFrame.pixelFormat)),
                rawFrame.data, static_cast<size_t>(rawFrame.stride)};
    return timestamp;
  }

  /**
   * Wait for the next frame and get the image.
   * Times out (returning 0) after timeout seconds.
   * The provided image will have the pixelFormat this class was constructed
   * with. The data is backed by data in the CvSink. It will be invalidated by
   * any grabFrame*() call on the sink.
   *
   * <p>If lastFrameTime is provided and non-zero, the sink will fill image with
   * the first frame from the source that is not equal to lastFrameTime. If
   * lastFrameTime is zero, the time of the current frame owned by the CvSource
   * is used, and this function will block until the connected CvSource provides
   * a new frame.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in the same time base as
   * wpi::util::Now(), and is in 1 us increments.
   */
  [[nodiscard]]
  uint64_t GrabFrameDirectLastTime(cv::Mat& image, uint64_t lastFrameTime,
                                   double timeout = 0.225) {
    rawFrame.height = 0;
    rawFrame.width = 0;
    rawFrame.stride = 0;
    rawFrame.pixelFormat = static_cast<int>(pixelFormat);
    auto timestamp = GrabSinkFrameTimeoutLastTime(m_handle, rawFrame, timeout,
                                                  lastFrameTime, &m_status);
    if (m_status != CS_OK) {
      return 0;
    }
    image =
        cv::Mat{rawFrame.height, rawFrame.width,
                GetCvFormat(static_cast<WPI_PixelFormat>(rawFrame.pixelFormat)),
                rawFrame.data, static_cast<size_t>(rawFrame.stride)};
    return timestamp;
  }

  /**
   * Get the last time a frame was grabbed. This uses the same time base as
   * wpi::util::Now().
   *
   * @return Time in 1 us increments.
   */
  [[nodiscard]]
  uint64_t LastFrameTime() {
    return rawFrame.timestamp;
  }

  /**
   * Get the time source for the timestamp the last frame was grabbed at.
   *
   * @return Time source
   */
  [[nodiscard]]
  WPI_TimestampSource LastFrameTimeSource() {
    return static_cast<WPI_TimestampSource>(rawFrame.timestampSrc);
  }

 private:
  constexpr int GetCvFormat(WPI_PixelFormat pixelFormat) {
    int type = 0;
    switch (pixelFormat) {
      case WPI_PIXFMT_YUYV:
      case WPI_PIXFMT_RGB565:
      case WPI_PIXFMT_Y16:
      case WPI_PIXFMT_UYVY:
        type = CV_8UC2;
        break;
      case WPI_PIXFMT_BGR:
        type = CV_8UC3;
        break;
      case WPI_PIXFMT_BGRA:
        type = CV_8UC4;
        break;
      case WPI_PIXFMT_GRAY:
      case WPI_PIXFMT_MJPEG:
      default:
        type = CV_8UC1;
        break;
    }
    return type;
  }

  wpi::util::RawFrame rawFrame;
  wpi::util::PixelFormat pixelFormat;
};

}  // namespace wpi::cs
