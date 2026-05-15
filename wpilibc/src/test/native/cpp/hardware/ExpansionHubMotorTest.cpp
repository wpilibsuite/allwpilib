// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/expansionhub/ExpansionHubMotor.hpp"

#include <gtest/gtest.h>

#include "wpi/hal/HAL.h"
#include "wpi/system/Errors.hpp"

namespace wpi {
TEST(ExpansionHubMotorTest, FollowerLoopDetection) {
  HAL_Initialize(500, 0);

  wpi::ExpansionHubMotor motor0{0, 0};
  wpi::ExpansionHubMotor motor1{0, 1};
  wpi::ExpansionHubMotor motor2{0, 2};

  // Test that a simple loop is detected
  motor1.Follow(motor2, wpi::ExpansionHubMotor::FollowDirection::Opposed);
  motor2.Follow(motor0, wpi::ExpansionHubMotor::FollowDirection::Opposed);
  EXPECT_THROW(
      motor0.Follow(motor1, wpi::ExpansionHubMotor::FollowDirection::Opposed),
      wpi::RuntimeError);
}
TEST(ExpansionHubMotorTest, Follower) {
  HAL_Initialize(500, 0);

  wpi::ExpansionHubMotor motor0{0, 0};
  wpi::ExpansionHubMotor motor1{0, 1};
  wpi::ExpansionHubMotor motor2{0, 2};

  motor1.Follow(motor2, wpi::ExpansionHubMotor::FollowDirection::Opposed);
  motor2.Follow(motor0, wpi::ExpansionHubMotor::FollowDirection::Opposed);
}
}  // namespace wpi
