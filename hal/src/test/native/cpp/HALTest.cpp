// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/HAL.h"

#include <catch2/catch_test_macros.hpp>

namespace wpi::hal {
TEST_CASE("HALTest RuntimeType", "[hal]") {
  CHECK(HAL_RuntimeType::HAL_RUNTIME_SIMULATION == HAL_GetRuntimeType());
}
}  // namespace wpi::hal
