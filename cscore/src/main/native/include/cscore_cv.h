// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_CSCORE_CV_H_
#define CSCORE_CSCORE_CV_H_

#include <functional>

#include <opencv2/core/mat.hpp>

#include "cscore_oo.h"
#include "cscore_raw.h"

namespace cs {
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
  CvSource(std::string_view name, const VideoMode& mode);

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
           int height, int fps);

  /**
   * Put an OpenCV image and notify sinks.
   *
   * <p>Only 8-bit single-channel or 3-channel (with BGR channel order) images
   * are supported. If the format, depth or channel order is different, use
   * cv::Mat::convertTo() and/or cv::cvtColor() to convert it first.
   *
   * @param image OpenCV image
   */
  void PutFrame(cv::Mat& image);
};

/**
 * A source for user code to accept video frames as OpenCV images.
 *
 * This is not dependent on any opencv binary ABI, and can be used
 * with versions of most versions of OpenCV.
 */
class CvSink : public ImageSink {
 public:
  CvSink() = default;
  CvSink(const CvSink& sink);

  /**
   * Create a sink for accepting OpenCV images.
   *
   * <p>WaitForFrame() must be called on the created sink to get each new
   * image.
   *
   * @param name Source name (arbitrary unique identifier)
   */
  explicit CvSink(std::string_view name, VideoMode::PixelFormat pixelFormat =
                                             VideoMode::PixelFormat::kBGR);

  /**
   * Wait for the next frame and get the image.
   * Times out (returning 0) after timeout seconds.
   * The provided image will have the pixelFormat this class was constructed
   * with.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in the same time base as wpi::Now(),
   *         and is in 1 us increments.
   */
  [[nodiscard]]
  uint64_t GrabFrame(cv::Mat& image, double timeout = 0.225);

  /**
   * Wait for the next frame and get the image.  May block forever.
   * The provided image will have the pixelFormat this class was constructed
   * with.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in the same time base as wpi::Now(),
   *         and is in 1 us increments.
   */
  [[nodiscard]]
  uint64_t GrabFrameNoTimeout(cv::Mat& image);

  /**
   * Wait for the next frame and get the image.
   * Times out (returning 0) after timeout seconds.
   * The provided image will have the pixelFormat this class was constructed
   * with. The data is backed by data in the CvSink. It will be invalidated by
   * any grabFrame*() call on the sink.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in the same time base as wpi::Now(),
   *         and is in 1 us increments.
   */
  [[nodiscard]]
  uint64_t GrabFrameDirect(cv::Mat& image, double timeout = 0.225);

  /**
   * Wait for the next frame and get the image.  May block forever.
   * The provided image will have the pixelFormat this class was constructed
   * with. The data is backed by data in the CvSink. It will be invalidated by
   * any grabFrame*() call on the sink.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in the same time base as wpi::Now(),
   *         and is in 1 us increments.
   */
  [[nodiscard]]
  uint64_t GrabFrameNoTimeoutDirect(cv::Mat& image);

 private:
  wpi::RawFrame rawFrame;
  VideoMode::PixelFormat pixelFormat;
};

inline CvSource::CvSource(std::string_view name, const VideoMode& mode) {
  m_handle = CreateRawSource(name, true, mode, &m_status);
}

inline CvSource::CvSource(std::string_view name, VideoMode::PixelFormat format,
                          int width, int height, int fps) {
  m_handle = CreateRawSource(name, true, VideoMode{format, width, height, fps},
                             &m_status);
}

inline void CvSource::PutFrame(cv::Mat& image) {
  // We only support 8-bit images; convert if necessary.
  cv::Mat finalImage;
  if (image.depth() == CV_8U) {
    finalImage = image;
  } else {
    image.convertTo(finalImage, CV_8U);
  }

  int channels = finalImage.channels();
  WPI_PixelFormat format;
  if (channels == 1) {
    format = WPI_PIXFMT_GRAY;
  } else if (channels == 3) {
    format = WPI_PIXFMT_BGR;
  } else {
    // TODO Error
    return;
  }
  // TODO old code supported BGRA, but the only way I can support that is slow.
  // Update cscore to support BGRA for raw frames

  WPI_RawFrame frame;  // use WPI_Frame because we don't want the destructor
  frame.data = finalImage.data;
  frame.freeFunc = nullptr;
  frame.freeCbData = nullptr;
  frame.size = finalImage.total() * channels;
  frame.width = finalImage.cols;
  frame.height = finalImage.rows;
  frame.stride = finalImage.cols;
  frame.pixelFormat = format;
  m_status = 0;
  PutSourceFrame(m_handle, frame, &m_status);
}

inline CvSink::CvSink(std::string_view name,
                      VideoMode::PixelFormat pixelFormat) {
  m_handle = CreateRawSink(name, true, &m_status);
  this->pixelFormat = pixelFormat;
}

inline CvSink::CvSink(const CvSink& sink) : ImageSink{sink} {}

inline uint64_t CvSink::GrabFrame(cv::Mat& image, double timeout) {
  cv::Mat tmpnam;
  auto retVal = GrabFrameDirect(tmpnam);
  if (retVal <= 0) {
    return retVal;
  }
  tmpnam.copyTo(image);
  return retVal;
}

inline uint64_t CvSink::GrabFrameNoTimeout(cv::Mat& image) {
  cv::Mat tmpnam;
  auto retVal = GrabFrameNoTimeoutDirect(tmpnam);
  if (retVal <= 0) {
    return retVal;
  }
  tmpnam.copyTo(image);
  return retVal;
}

inline constexpr int GetCvFormat(WPI_PixelFormat pixelFormat) {
  int type = 0;
  switch (pixelFormat) {
    case WPI_PIXFMT_YUYV:
    case WPI_PIXFMT_RGB565:
      type = CV_8UC2;
      break;
    case WPI_PIXFMT_BGR:
      type = CV_8UC3;
      break;
    case WPI_PIXFMT_GRAY:
    case WPI_PIXFMT_MJPEG:
    default:
      type = CV_8UC1;
      break;
  }
  return type;
}

inline uint64_t CvSink::GrabFrameDirect(cv::Mat& image, double timeout) {
  rawFrame.height = 0;
  rawFrame.width = 0;
  rawFrame.pixelFormat = pixelFormat;
  m_status = GrabSinkFrameTimeout(m_handle, rawFrame, timeout, &m_status);
  if (m_status <= 0) {
    return m_status;
  }
  image =
      cv::Mat{rawFrame.height, rawFrame.width,
              GetCvFormat(static_cast<WPI_PixelFormat>(rawFrame.pixelFormat)),
              rawFrame.data};
  return m_status;
}

inline uint64_t CvSink::GrabFrameNoTimeoutDirect(cv::Mat& image) {
  rawFrame.height = 0;
  rawFrame.width = 0;
  rawFrame.pixelFormat = pixelFormat;
  m_status = GrabSinkFrame(m_handle, rawFrame, &m_status);
  if (m_status <= 0) {
    return m_status;
  }
  image =
      cv::Mat{rawFrame.height, rawFrame.width,
              GetCvFormat(static_cast<WPI_PixelFormat>(rawFrame.pixelFormat)),
              rawFrame.data};
  return m_status;
}

}  // namespace cs

#endif  // CSCORE_CSCORE_RAW_CV_H_
