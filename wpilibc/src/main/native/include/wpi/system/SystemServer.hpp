// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/nt/NetworkTableInstance.hpp"

namespace frc {

class SystemServer {
 public:
  SystemServer() = delete;

  static nt::NetworkTableInstance GetSystemServer();
};

}  // namespace frc
