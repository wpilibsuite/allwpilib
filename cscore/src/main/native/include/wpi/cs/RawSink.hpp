// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <string_view>

#include "wpi/cs/ImageSink.hpp"
#include "wpi/cs/cscore_raw.hpp"
#include "wpi/util/RawFrame.hpp"

namespace wpi::cs {

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
  explicit RawSink(std::string_view name) {
    m_handle = CreateRawSink(name, false, &m_status);
  }

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
          std::function<void(uint64_t time)> processFrame) {
    m_handle = CreateRawSinkCallback(name, false, processFrame, &m_status);
  }

 protected:
  /**
   * Wait for the next frame and get the image.
   * Times out (returning 0) after timeout seconds.
   * The provided image will have three 8-bit channels stored in BGR order.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in the same time base as
   * wpi::util::Now(), and is in 1 us increments.
   */
  [[nodiscard]]
  uint64_t GrabFrame(wpi::util::RawFrame& image, double timeout = 0.225) const {
    m_status = 0;
    return GrabSinkFrameTimeout(m_handle, image, timeout, &m_status);
  }

  /**
   * Wait for the next frame and get the image.  May block forever.
   * The provided image will have three 8-bit channels stored in BGR order.
   *
   * @return Frame time, or 0 on error (call GetError() to obtain the error
   *         message); the frame time is in the same time base as
   * wpi::util::Now(), and is in 1 us increments.
   */
  [[nodiscard]]
  uint64_t GrabFrameNoTimeout(wpi::util::RawFrame& image) const {
    m_status = 0;
    return GrabSinkFrame(m_handle, image, &m_status);
  }

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
   *         message); the frame time is in the same time base as
   * wpi::util::Now(), and is in 1 us increments.
   */
  [[nodiscard]]
  uint64_t GrabFrameLastTime(wpi::util::RawFrame& image, uint64_t lastFrameTime,
                             double timeout = 0.225) const {
    m_status = 0;
    return GrabSinkFrameTimeoutLastTime(m_handle, image, timeout, lastFrameTime,
                                        &m_status);
  }
};

}  // namespace wpi::cs
