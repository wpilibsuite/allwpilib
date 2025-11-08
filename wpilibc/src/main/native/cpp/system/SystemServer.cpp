// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/SystemServer.h"
#include "wpi/system/SystemServer.hpp"

namespace frc {

nt::NetworkTableInstance SystemServer::GetSystemServer() {
  return nt::NetworkTableInstance{HAL_GetSystemServerHandle()};
}

}  // namespace frc
