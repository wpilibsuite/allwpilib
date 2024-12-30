// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_CSCORE_CV_H_
#define CSCORE_CSCORE_CV_H_

#include <functional>

#include <opencv2/core/mat.hpp>
#include <wpi/RawFrame.h>

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
  void PutFrame(cv::Mat& image);

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
                bool skipVerification);

 private:
  static bool VerifyFormat(cv::Mat& image, VideoMode::PixelFormat pixelFormat);
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
   * @param pixelFormat The pixel format to read
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
   *         message); the frame time is in the same time base as wpi::Now(),
   *         and is in 1 us increments.
   */
  [[nodiscard]]
  uint64_t GrabFrameDirectLastTime(cv::Mat& image, uint64_t lastFrameTime,
                                   double timeout = 0.225);

  /**
   * Get the last time a frame was grabbed. This uses the same time base as
   * wpi::Now().
   *
   * @return Time in 1 us increments.
   */
  [[nodiscard]]
  uint64_t LastFrameTime();

  /**
   * Get the time source for the timestamp the last frame was grabbed at.
   *
   * @return Time source
   */
  [[nodiscard]]
  WPI_TimestampSource LastFrameTimeSource();

 private:
  constexpr int GetCvFormat(WPI_PixelFormat pixelFormat);

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

inline bool CvSource::VerifyFormat(cv::Mat& image,
                                   VideoMode::PixelFormat pixelFormat) {
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

inline void CvSource::PutFrame(cv::Mat& image) {
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

inline void CvSource::PutFrame(cv::Mat& image,
                               VideoMode::PixelFormat pixelFormat,
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

inline CvSink::CvSink(std::string_view name,
                      VideoMode::PixelFormat pixelFormat) {
  m_handle = CreateRawSink(name, true, &m_status);
  this->pixelFormat = pixelFormat;
}

inline CvSink::CvSink(const CvSink& sink)
    : ImageSink{sink}, pixelFormat{sink.pixelFormat} {}

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

inline constexpr int CvSink::GetCvFormat(WPI_PixelFormat pixelFormat) {
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

inline uint64_t CvSink::GrabFrameDirect(cv::Mat& image, double timeout) {
  rawFrame.height = 0;
  rawFrame.width = 0;
  rawFrame.stride = 0;
  rawFrame.pixelFormat = pixelFormat;
  auto timestamp = GrabSinkFrameTimeout(m_handle, rawFrame, timeout, &m_status);
  if (m_status != CS_OK) {
    return 0;
  }
  image =
      cv::Mat{rawFrame.height, rawFrame.width,
              GetCvFormat(static_cast<WPI_PixelFormat>(rawFrame.pixelFormat)),
              rawFrame.data, static_cast<size_t>(rawFrame.stride)};
  return timestamp;
}

inline uint64_t CvSink::GrabFrameNoTimeoutDirect(cv::Mat& image) {
  rawFrame.height = 0;
  rawFrame.width = 0;
  rawFrame.stride = 0;
  rawFrame.pixelFormat = pixelFormat;
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

inline uint64_t CvSink::GrabFrameDirectLastTime(cv::Mat& image,
                                                uint64_t lastFrameTime,
                                                double timeout) {
  rawFrame.height = 0;
  rawFrame.width = 0;
  rawFrame.stride = 0;
  rawFrame.pixelFormat = pixelFormat;
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

inline uint64_t CvSink::LastFrameTime() {
  return rawFrame.timestamp;
}

inline WPI_TimestampSource CvSink::LastFrameTimeSource() {
  return static_cast<WPI_TimestampSource>(rawFrame.timestampSrc);
}

}  // namespace cs

#endif  // CSCORE_CSCORE_CV_H_
