// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/system/SystemServer.hpp"

#include "wpi/hal/SystemServer.h"

namespace wpi {

wpi::nt::NetworkTableInstance SystemServer::GetSystemServer() {
  return wpi::nt::NetworkTableInstance{HAL_GetSystemServerHandle()};
}

}  // namespace wpi
