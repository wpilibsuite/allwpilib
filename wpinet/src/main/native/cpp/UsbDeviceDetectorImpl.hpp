// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <string_view>

#include "wpi/net/UsbDeviceDetector.hpp"

namespace wpi::net {

struct UsbDeviceDetector::Impl {
  virtual ~Impl() = default;
  virtual bool Start() = 0;
  virtual void Stop() = 0;
  virtual bool IsRunning() const = 0;
};

namespace detail {

std::optional<int> GetSystemCoreUsbPort(std::string_view sysname);

}  // namespace detail
}  // namespace wpi::net
