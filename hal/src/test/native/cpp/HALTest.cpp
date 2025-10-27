// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/hal/HAL.h"

namespace wpi::hal {
TEST(HALTest, RuntimeType) {
  EXPECT_EQ(HAL_RuntimeType::HAL_Runtime_Simulation, HAL_GetRuntimeType());
}
}  // namespace wpi::hal
