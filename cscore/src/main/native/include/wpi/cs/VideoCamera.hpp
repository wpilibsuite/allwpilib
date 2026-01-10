// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/cs/cscore_cpp.hpp"
#include "wpi/cs/VideoSource.hpp"

namespace wpi::cs {

/**
 * A source that represents a video camera.
 */
class VideoCamera : public VideoSource {
 public:
  /**
   * White balance.
   */
  enum WhiteBalance {
    /// Fixed indoor white balance.
    kFixedIndoor = 3000,
    /// Fixed outdoor white balance 1.
    kFixedOutdoor1 = 4000,
    /// Fixed outdoor white balance 2.
    kFixedOutdoor2 = 5000,
    /// Fixed fluorescent white balance 1.
    kFixedFluorescent1 = 5100,
    /// Fixed fluorescent white balance 2.
    kFixedFlourescent2 = 5200
  };

  VideoCamera() = default;

  /**
   * Set the brightness, as a percentage (0-100).
   */
  void SetBrightness(int brightness) {
    m_status = 0;
    SetCameraBrightness(m_handle, brightness, &m_status);
  }

  /**
   * Get the brightness, as a percentage (0-100).
   */
  int GetBrightness() {
    m_status = 0;
    return GetCameraBrightness(m_handle, &m_status);
  }

  /**
   * Set the white balance to auto.
   */
  void SetWhiteBalanceAuto() {
    m_status = 0;
    SetCameraWhiteBalanceAuto(m_handle, &m_status);
  }

  /**
   * Set the white balance to hold current.
   */
  void SetWhiteBalanceHoldCurrent() {
    m_status = 0;
    SetCameraWhiteBalanceHoldCurrent(m_handle, &m_status);
  }

  /**
   * Set the white balance to manual, with specified color temperature.
   */
  void SetWhiteBalanceManual(int value) {
    m_status = 0;
    SetCameraWhiteBalanceManual(m_handle, value, &m_status);
  }

  /**
   * Set the exposure to auto aperture.
   */
  void SetExposureAuto() {
    m_status = 0;
    SetCameraExposureAuto(m_handle, &m_status);
  }

  /**
   * Set the exposure to hold current.
   */
  void SetExposureHoldCurrent() {
    m_status = 0;
    SetCameraExposureHoldCurrent(m_handle, &m_status);
  }

  /**
   * Set the exposure to manual, as a percentage (0-100).
   */
  void SetExposureManual(int value) {
    m_status = 0;
    SetCameraExposureManual(m_handle, value, &m_status);
  }

 protected:
  explicit VideoCamera(CS_Source handle) : VideoSource(handle) {}
};

}  // namespace wpi::cs
