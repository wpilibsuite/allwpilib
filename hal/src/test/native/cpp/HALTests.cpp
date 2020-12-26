// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "gtest/gtest.h"
#include "hal/HAL.h"
#include "hal/Solenoid.h"

namespace hal {
TEST(HALTests, RuntimeType) {
  EXPECT_EQ(HAL_RuntimeType::HAL_Mock, HAL_GetRuntimeType());
}

TEST(HALSOLENOID, SolenoidTest) {
  int32_t status = 0;
  HAL_InitializeSolenoidPort(0, &status);
  EXPECT_NE(status, 0);
}
}  // namespace hal
