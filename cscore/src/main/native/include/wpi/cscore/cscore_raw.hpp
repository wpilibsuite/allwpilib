// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef CSCORE_CSCORE_RAW_H_
#define CSCORE_CSCORE_RAW_H_

#include <functional>

#include "cscore_c.h"

// NOLINTBEGIN
#ifdef __cplusplus
#include "cscore_oo.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
// NOLINTEND

/**
 * @defgroup cscore_raw_cfunc Raw Image Functions
 * @{
 */
uint64_t CS_GrabRawSinkFrame(CS_Sink sink, struct WPI_RawFrame* rawImage,
                             CS_Status* status);
uint64_t CS_GrabRawSinkFrameTimeout(CS_Sink sink, struct WPI_RawFrame* rawImage,
                                    double timeout, CS_Status* status);
uint64_t CS_GrabRawSinkFrameTimeoutWithFrameTime(CS_Sink sink,
                                                 struct WPI_RawFrame* rawImage,
                                                 double timeout,
                                                 uint64_t lastFrameTime,
                                                 CS_Status* status);

CS_Sink CS_CreateRawSink(const struct WPI_String* name, CS_Bool isCv,
                         CS_Status* status);

CS_Sink CS_CreateRawSinkCallback(
    const struct WPI_String* name, CS_Bool isCv, void* data,
    void (*processFrame)(void* data, uint64_t time), CS_Status* status);

void CS_PutRawSourceFrame(CS_Source source, const struct WPI_RawFrame* image,
                          CS_Status* status);

CS_Source CS_CreateRawSource(const struct WPI_String* name, CS_Bool isCv,
                             const CS_VideoMode* mode, CS_Status* status);
/** @} */

#ifdef __cplusplus
}  // extern "C"
#endif

#ifdef __cplusplus
namespace cs {

/**
 * @defgroup cscore_raw_func Raw Image Functions
 * @{
 */

CS_Source CreateRawSource(std::string_view name, bool isCv,
                          const VideoMode& mode, CS_Status* status);

CS_Sink CreateRawSink(std::string_view name, bool isCv, CS_Status* status);
CS_Sink CreateRawSinkCallback(std::string_view name, bool isCv,
                              std::function<void(uint64_t time)> processFrame,
                              CS_Status* status);

void PutSourceFrame(CS_Source source, const WPI_RawFrame& image,
                    CS_Status* status);
uint64_t GrabSinkFrame(CS_Sink sink, WPI_RawFrame& image, CS_Status* status);
uint64_t GrabSinkFrameTimeout(CS_Sink sink, WPI_RawFrame& image, double timeout,
                              CS_Status* status);
uint64_t GrabSinkFrameTimeoutLastTime(CS_Sink sink, WPI_RawFrame& image,
                                      double timeout, uint64_t lastFrameTime,
                                      CS_Status* status);

/**
 * A source for user code to provide video frames as raw bytes.
 *
 * This is a complex API, most cases should use CvSource.
 */
class RawSource : public ImageSource {
 public:
  RawSource() = default;

  /**
   * Create a raw frame source.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param mode Video mode being generated
   */
  RawSource(std::string_view name, const VideoMode& mode);

  /**
   * Create a raw frame source.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param pixelFormat Pixel format
   * @param width width
   * @param height height
   * @param fps fps
   */
  RawSource(std::string_view name, VideoMode::PixelFormat pixelFormat,
            int width, int height, int fps);

 protected:
  /**
   * Put a raw image and notify sinks.
   *
   * @param image raw frame image
   */
  void PutFrame(wpi::RawFrame& image);
};

/**
 * A sink for user code to accept video frames as raw bytes.
 *
 * This is a complex API, most cases should use CvSource.
 */
class RawSink : public ImageSink {
 public:
  RawSink() = default;

  /**
   * Create a sink for accepting raw images.
   *
   * <p>GrabFrame() must be called on the created sink to get each new
   * image.
   *
   * @param name Source name (arbitrary unique identifier)
   */
  explicit RawSink(std::string_view name);

  /**
   * Create a sink for accepting raws images in a separate thread.
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
  RawSink(std::string_view name,
          std::function<void(uint64_t time)> processFrame);

 protected:
  /**
   * Wait for the next frame and get the image.
   * Times out (returning 0) after timeout seconds.
   * The provided image will have three 8-bit channels stored in BGR order.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in the same time base as wpi::Now(),
   *         and is in 1 us increments.
   */
  [[nodiscard]]
  uint64_t GrabFrame(wpi::RawFrame& image, double timeout = 0.225) const;

  /**
   * Wait for the next frame and get the image.  May block forever.
   * The provided image will have three 8-bit channels stored in BGR order.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in the same time base as wpi::Now(),
   *         and is in 1 us increments.
   */
  [[nodiscard]]
  uint64_t GrabFrameNoTimeout(wpi::RawFrame& image) const;

  /**
   * Wait for the next frame and get the image.  May block forever.
   * The provided image will have three 8-bit channels stored in BGR order.
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
  uint64_t GrabFrameLastTime(wpi::RawFrame& image, uint64_t lastFrameTime,
                             double timeout = 0.225) const;
};

inline RawSource::RawSource(std::string_view name, const VideoMode& mode) {
  m_handle = CreateRawSource(name, false, mode, &m_status);
}

inline RawSource::RawSource(std::string_view name,
                            VideoMode::PixelFormat format, int width,
                            int height, int fps) {
  m_handle = CreateRawSource(name, false, VideoMode{format, width, height, fps},
                             &m_status);
}

inline void RawSource::PutFrame(wpi::RawFrame& image) {
  m_status = 0;
  PutSourceFrame(m_handle, image, &m_status);
}

inline RawSink::RawSink(std::string_view name) {
  m_handle = CreateRawSink(name, false, &m_status);
}

inline RawSink::RawSink(std::string_view name,
                        std::function<void(uint64_t time)> processFrame) {
  m_handle = CreateRawSinkCallback(name, false, processFrame, &m_status);
}

inline uint64_t RawSink::GrabFrame(wpi::RawFrame& image, double timeout) const {
  m_status = 0;
  return GrabSinkFrameTimeout(m_handle, image, timeout, &m_status);
}

inline uint64_t RawSink::GrabFrameNoTimeout(wpi::RawFrame& image) const {
  m_status = 0;
  return GrabSinkFrame(m_handle, image, &m_status);
}

inline uint64_t RawSink::GrabFrameLastTime(wpi::RawFrame& image,
                                           uint64_t lastFrameTime,
                                           double timeout) const {
  m_status = 0;
  return GrabSinkFrameTimeoutLastTime(m_handle, image, timeout, lastFrameTime,
                                      &m_status);
}
/** @} */

}  // namespace cs

#endif

#endif  // CSCORE_CSCORE_RAW_H_
