// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "HALInitializer.hpp"

#include "wpi/hal/HALBase.h"

namespace wpi::hal::init {
std::atomic_bool HAL_IsInitialized{false};
void RunInitialize() {
  HAL_Initialize(500, 0);
}
}  // namespace wpi::hal::init
