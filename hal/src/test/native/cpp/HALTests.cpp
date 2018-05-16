/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/HAL.h"
#include "HAL/Solenoid.h"
#include "gtest/gtest.h"

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
