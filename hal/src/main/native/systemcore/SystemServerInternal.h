// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <networktables/NetworkTableInstance.h>

namespace hal {
void InitializeSystemServer();
void ShutdownSystemServer();
nt::NetworkTableInstance GetSystemServer();
}  // namespace hal
