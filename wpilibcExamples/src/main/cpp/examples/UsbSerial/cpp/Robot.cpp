// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <algorithm>
#include <array>
#include <cstdio>
#include <exception>
#include <memory>
#include <mutex>
#include <string>

#include "wpi/framework/TimedRobot.hpp"
#include "wpi/hardware/bus/SerialPort.hpp"
#include "wpi/net/UsbDeviceDetector.hpp"

class Robot : public wpi::TimedRobot {
 public:
  Robot()
      : m_detector{0,  // Physical USB port; valid values are 0-3.
                   [this](const wpi::net::UsbDevice& device) {
                     DeviceConnected(device);
                   },
                   [this](const wpi::net::UsbDevice& device) {
                     DeviceDisconnected(device);
                   }} {
    if (!m_detector.Start()) {
      std::fprintf(stderr, "USB device detection is unavailable\n");
    }
  }

  ~Robot() override {
    // Detector callbacks use the members below, so stop the callback thread
    // before those members are destroyed.
    m_detector.Stop();
  }

  void RobotPeriodic() override {
    // Detector callbacks execute on a worker thread, so protect SerialPort use
    // with the same mutex used when the callbacks open and close it.
    std::scoped_lock lock{m_mutex};
    if (!m_serial) {
      return;
    }

    try {
      std::array<char, 256> buffer;
      int available = m_serial->GetBytesReceived();
      if (available > 0) {
        int received = m_serial->Read(
            buffer.data(),
            std::min(available, static_cast<int>(buffer.size())));
        std::fwrite(buffer.data(), 1, received, stdout);
      }
    } catch (const std::exception& error) {
      std::fprintf(stderr, "Serial read failed: %s\n", error.what());
    }
  }

 private:
  void DeviceConnected(const wpi::net::UsbDevice& device) {
    // The detector is generic. A USB serial adapter has a child device in the
    // tty subsystem; cameras, HID devices, and storage devices have different
    // subsystem values and can be handled by other consumers.
    if (device.subsystem != "tty" || device.deviceNode.empty()) {
      return;
    }

    std::scoped_lock lock{m_mutex};
    if (m_serial) {
      return;
    }

    try {
      m_serial = std::make_unique<wpi::SerialPort>(115200, device.deviceNode);
      m_serialPath = device.deviceNode;
      std::printf("Opened serial device %s\n", m_serialPath.c_str());
    } catch (const std::exception& error) {
      std::fprintf(stderr, "Could not open %s: %s\n", device.deviceNode.c_str(),
                   error.what());
    }
  }

  void DeviceDisconnected(const wpi::net::UsbDevice& device) {
    std::scoped_lock lock{m_mutex};
    if (!m_serial || device.deviceNode != m_serialPath) {
      return;
    }

    m_serial.reset();
    m_serialPath.clear();
    std::printf("Closed disconnected serial device\n");
  }

  std::mutex m_mutex;
  std::unique_ptr<wpi::SerialPort> m_serial;
  std::string m_serialPath;
  wpi::net::UsbDeviceDetector m_detector;
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
