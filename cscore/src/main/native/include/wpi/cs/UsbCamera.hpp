// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "wpi/cs/cscore_cpp.hpp"
#include "wpi/cs/VideoCamera.hpp"

namespace wpi::cs {

/**
 * A source that represents a USB camera.
 */
class UsbCamera : public VideoCamera {
 public:
  UsbCamera() = default;

  /**
   * Create a source for a USB camera based on device number.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param dev Device number (e.g. 0 for /dev/video0)
   */
  UsbCamera(std::string_view name, int dev) {
    m_handle = CreateUsbCameraDev(name, dev, &m_status);
  }

  /**
   * Create a source for a USB camera based on device path.
   *
   * @param name Source name (arbitrary unique identifier)
   * @param path Path to device (e.g. "/dev/video0" on Linux)
   */
  UsbCamera(std::string_view name, std::string_view path) {
    m_handle = CreateUsbCameraPath(name, path, &m_status);
  }

  /**
   * Enumerate USB cameras on the local system.
   *
   * @return Vector of USB camera information (one for each camera)
   */
  static std::vector<UsbCameraInfo> EnumerateUsbCameras() {
    CS_Status status = 0;
    return ::wpi::cs::EnumerateUsbCameras(&status);
  }

  /**
   * Change the path to the device.
   */
  void SetPath(std::string_view path) {
    m_status = 0;
    return ::wpi::cs::SetUsbCameraPath(m_handle, path, &m_status);
  }

  /**
   * Get the path to the device.
   */
  std::string GetPath() const {
    m_status = 0;
    return ::wpi::cs::GetUsbCameraPath(m_handle, &m_status);
  }

  /**
   * Get the full camera information for the device.
   */
  UsbCameraInfo GetInfo() const {
    m_status = 0;
    return ::wpi::cs::GetUsbCameraInfo(m_handle, &m_status);
  }

  /**
   * Set how verbose the camera connection messages are.
   *
   * @param level 0=don't display Connecting message, 1=do display message
   */
  void SetConnectVerbose(int level) {
    m_status = 0;
    SetProperty(GetSourceProperty(m_handle, "connect_verbose", &m_status),
                level, &m_status);
  }
};

}  // namespace wpi::cs
