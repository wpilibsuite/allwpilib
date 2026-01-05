// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "wpi/cs/ImageSource.hpp"
#include "wpi/cs/VideoMode.hpp"
#include "wpi/cs/cscore_raw.hpp"
#include "wpi/util/RawFrame.hpp"

namespace wpi::cs {

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
  RawSource(std::string_view name, const VideoMode& mode) {
    m_handle = CreateRawSource(name, false, mode, &m_status);
  }

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
            int width, int height, int fps) {
    m_handle = CreateRawSource(
        name, false, VideoMode{pixelFormat, width, height, fps}, &m_status);
  }

 protected:
  /**
   * Put a raw image and notify sinks.
   *
   * @param image raw frame image
   */
  void PutFrame(wpi::util::RawFrame& image) {
    m_status = 0;
    PutSourceFrame(m_handle, image, &m_status);
  }
};

}  // namespace wpi::cs
