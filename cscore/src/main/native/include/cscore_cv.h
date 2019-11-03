/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef CSCORE_CSCORE_CV_H_
#define CSCORE_CSCORE_CV_H_

#include "cscore_c.h"

#ifdef CSCORE_CSCORE_RAW_CV_H_
#error "Cannot include both cscore_cv.h and cscore_raw_cv.h in the same file"
#endif

#ifdef __cplusplus
#include "cscore_oo.h"  // NOLINT(build/include_order)

#endif

#if CV_VERSION_MAJOR < 4

#ifdef __cplusplus
extern "C" {  // NOLINT(build/include_order)
#endif

struct CvMat;

void CS_PutSourceFrame(CS_Source source, struct CvMat* image,
                       CS_Status* status);

uint64_t CS_GrabSinkFrame(CS_Sink sink, struct CvMat* image, CS_Status* status);
uint64_t CS_GrabSinkFrameTimeout(CS_Sink sink, struct CvMat* image,
                                 double timeout, CS_Status* status);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // CV_VERSION_MAJOR < 4

#ifdef __cplusplus

#include "cscore_oo.h"

namespace cv {
class Mat;
}  // namespace cv

namespace cs {

/**
 * @defgroup cscore_cpp_opencv_special cscore C functions taking a cv::Mat*
 *
 * These are needed for specific interop implementations.
 * @{
 */
extern "C" {
uint64_t CS_GrabSinkFrameCpp(CS_Sink sink, cv::Mat* image, CS_Status* status);
uint64_t CS_GrabSinkFrameTimeoutCpp(CS_Sink sink, cv::Mat* image,
                                    double timeout, CS_Status* status);
void CS_PutSourceFrameCpp(CS_Source source, cv::Mat* image, CS_Status* status);
}  // extern "C"
/** @} */

void PutSourceFrame(CS_Source source, cv::Mat& image, CS_Status* status);
uint64_t GrabSinkFrame(CS_Sink sink, cv::Mat& image, CS_Status* status);
uint64_t GrabSinkFrameTimeout(CS_Sink sink, cv::Mat& image, double timeout,
                              CS_Status* status);

/**
 * A source for user code to provide OpenCV images as video frames.
 * These sources require the WPILib OpenCV builds.
 * For an alternate OpenCV, include "cscore_raw_cv.h" instead, and
 * include your Mat header before that header.
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
  CvSource(const wpi::Twine& name, const VideoMode& mode);

  /**
   * Create an OpenCV source.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param pixelFormat Pixel format
   * @param width width
   * @param height height
   * @param fps fps
   */
  CvSource(const wpi::Twine& name, VideoMode::PixelFormat pixelFormat,
           int width, int height, int fps);

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
 * A sink for user code to accept video frames as OpenCV images.
 * These sinks require the WPILib OpenCV builds.
 * For an alternate OpenCV, include "cscore_raw_cv.h" instead, and
 * include your Mat header before that header.
 */
class CvSink : public ImageSink {
 public:
  CvSink() = default;

  /**
   * Create a sink for accepting OpenCV images.
   *
   * <p>WaitForFrame() must be called on the created sink to get each new
   * image.
   *
   * @param name Source name (arbitrary unique identifier)
   */
  explicit CvSink(const wpi::Twine& name);

  /**
   * Create a sink for accepting OpenCV images in a separate thread.
   *
   * <p>A thread will be created that calls WaitForFrame() and calls the
   * processFrame() callback each time a new frame arrives.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param processFrame Frame processing function; will be called with a
   *        time=0 if an error occurred.  processFrame should call GetImage()
   *        or GetError() as needed, but should not call (except in very
   *        unusual circumstances) WaitForImage().
   */
  CvSink(const wpi::Twine& name,
         std::function<void(uint64_t time)> processFrame);

  /**
   * Wait for the next frame and get the image.
   * Times out (returning 0) after timeout seconds.
   * The provided image will have three 8-bit channels stored in BGR order.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in the same time base as wpi::Now(),
   *         and is in 1 us increments.
   */
  uint64_t GrabFrame(cv::Mat& image, double timeout = 0.225) const;

  /**
   * Wait for the next frame and get the image.  May block forever.
   * The provided image will have three 8-bit channels stored in BGR order.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in the same time base as wpi::Now(),
   *         and is in 1 us increments.
   */
  uint64_t GrabFrameNoTimeout(cv::Mat& image) const;
};

inline CvSource::CvSource(const wpi::Twine& name, const VideoMode& mode) {
  m_handle = CreateCvSource(name, mode, &m_status);
}

inline CvSource::CvSource(const wpi::Twine& name, VideoMode::PixelFormat format,
                          int width, int height, int fps) {
  m_handle =
      CreateCvSource(name, VideoMode{format, width, height, fps}, &m_status);
}

inline void CvSource::PutFrame(cv::Mat& image) {
  m_status = 0;
  PutSourceFrame(m_handle, image, &m_status);
}

inline CvSink::CvSink(const wpi::Twine& name) {
  m_handle = CreateCvSink(name, &m_status);
}

inline CvSink::CvSink(const wpi::Twine& name,
                      std::function<void(uint64_t time)> processFrame) {
  m_handle = CreateCvSinkCallback(name, processFrame, &m_status);
}

inline uint64_t CvSink::GrabFrame(cv::Mat& image, double timeout) const {
  m_status = 0;
  return GrabSinkFrameTimeout(m_handle, image, timeout, &m_status);
}

inline uint64_t CvSink::GrabFrameNoTimeout(cv::Mat& image) const {
  m_status = 0;
  return GrabSinkFrame(m_handle, image, &m_status);
}

}  // namespace cs

#endif

#endif  // CSCORE_CSCORE_CV_H_
