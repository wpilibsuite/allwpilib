// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "wpi/cs/cscore_cpp.hpp"
#include "wpi/cs/VideoSink.hpp"

namespace wpi::cs {

/**
 * A base class for single image reading sinks.
 */
class ImageSink : public VideoSink {
 protected:
  ImageSink() = default;

 public:
  /**
   * Set sink description.
   *
   * @param description Description
   */
  void SetDescription(std::string_view description) {
    m_status = 0;
    SetSinkDescription(m_handle, description, &m_status);
  }

  /**
   * Get error string.  Call this if WaitForFrame() returns 0 to determine
   * what the error is.
   */
  std::string GetError() const {
    m_status = 0;
    return GetSinkError(m_handle, &m_status);
  }

  /**
   * Enable or disable getting new frames.
   *
   * <p>Disabling will cause processFrame (for callback-based CvSinks) to not
   * be called and WaitForFrame() to not return.  This can be used to save
   * processor resources when frames are not needed.
   */
  void SetEnabled(bool enabled) {
    m_status = 0;
    SetSinkEnabled(m_handle, enabled, &m_status);
  }
};

}  // namespace wpi::cs
