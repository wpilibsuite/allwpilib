// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/SystemServer.h"

#include <hal/SystemServer.h>

namespace frc {

nt::NetworkTableInstance SystemServer::GetSystemServer() {
  return nt::NetworkTableInstance{HAL_GetSystemServerHandle()};
}

}  // namespace frc
