// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/ntcore/NetworkTableInstance.hpp>

namespace wpi {

class SystemServer {
 public:
  SystemServer() = delete;

  static wpi::nt::NetworkTableInstance GetSystemServer();
};

}  // namespace wpi
