// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/hal/HALBase.h>
#include <wpi/util/print.hpp>

#include "wpi/system/WPILibVersion.hpp"

int main() {
  wpi::print("Hello World\n");
  wpi::print("{}\n", static_cast<int32_t>(HAL_GetRuntimeType()));
  wpi::print("{}\n", GetWPILibVersion());
}
