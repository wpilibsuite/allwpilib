// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <hal/HALBase.h>

#ifndef __FRC_SYSTEMCORE__
namespace frc::impl {
void ResetMotorSafety();
}
#endif

int main(int argc, char** argv) {
  HAL_Initialize(500, 0);
  ::testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();
#ifndef __FRC_SYSTEMCORE__
  frc::impl::ResetMotorSafety();
#endif
  return ret;
}
