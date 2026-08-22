// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string>

namespace wpi::net {

/** Information about a device associated with a physical USB port. */
struct UsbDevice {
  /** Physical USB port index (0-3). */
  int port = -1;
  /** Sysfs path of the device that generated the event. */
  std::string syspath;
  /** Sysfs path of the physical USB device. */
  std::string usbSyspath;
  /** Kernel subsystem for the device that generated the event. */
  std::string subsystem;
  /** Kernel device type, if one is available. */
  std::string deviceType;
  /** Device node, such as /dev/ttyUSB0 or /dev/video0, if available. */
  std::string deviceNode;
  /** USB vendor ID. */
  std::string vendorId;
  /** USB product ID. */
  std::string productId;
  /** USB manufacturer name. */
  std::string manufacturer;
  /** USB product name. */
  std::string product;
  /** USB serial number. */
  std::string serialNumber;
};

/**
 * Detects devices associated with one of the four physical SystemCore USB
 * ports.
 *
 * The detector reports the physical USB device and actionable child devices
 * that have device nodes. For example, a USB serial adapter produces a USB
 * device event and a tty event whose deviceNode is /dev/ttyUSB0. Other device
 * classes can be identified through subsystem and deviceType.
 *
 * Callbacks execute on an internal worker thread. They must not destroy, start,
 * or stop this detector. Call Stop() before destroying state captured by a
 * callback.
 */
class UsbDeviceDetector {
 public:
  using Callback = std::function<void(UsbDevice)>;

  /**
   * Constructs a USB device detector.
   *
   * @param port Physical USB port index (0-3).
   * @param connected Called when a matching device is connected. Devices that
   *                  are already present are reported when Start() is called.
   * @param disconnected Called when a previously reported device is removed.
   * @throws std::out_of_range if port is outside the range 0-3.
   */
  UsbDeviceDetector(int port, Callback connected, Callback disconnected);

  ~UsbDeviceDetector();

  UsbDeviceDetector(const UsbDeviceDetector&) = delete;
  UsbDeviceDetector& operator=(const UsbDeviceDetector&) = delete;

  /**
   * Starts device detection.
   *
   * @return True if detection was started or was already running.
   */
  bool Start();

  /** Stops device detection. */
  void Stop();

  /** Returns whether device detection is running. */
  bool IsRunning() const;

  /** Returns whether a systemd device implementation is available. */
  bool HasImplementation() const;

  struct Impl;

 private:
  static std::unique_ptr<Impl> CreateImpl(int port, Callback connected,
                                          Callback disconnected);

  std::unique_ptr<Impl> m_impl;
};

}  // namespace wpi::net
