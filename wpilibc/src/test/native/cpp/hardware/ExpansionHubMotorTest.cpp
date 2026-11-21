// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/expansionhub/ExpansionHubMotor.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/hal/HAL.h"
#include "wpi/system/Errors.hpp"

namespace wpi {
TEST_CASE("ExpansionHubMotorTest MotorControllerThrottle",
          "[wpilibc][hardware]") {
  HAL_Initialize();
  ExpansionHubMotor motor{0, 0};
  MotorController& controller = motor;

  CHECK(controller.GetThrottle() == 0);
  for (double throttle : {0.5, -0.25, 0.0}) {
    controller.SetThrottle(throttle);
    CHECK(controller.GetThrottle() == throttle);
  }

  controller.SetThrottle(0.5);
  motor.SetPositionSetpoint(100);
  CHECK(controller.GetThrottle() == 0.5);
  motor.SetVelocitySetpoint(20);
  CHECK(controller.GetThrottle() == 0.5);
  motor.Unfollow();
  CHECK(controller.GetThrottle() == 0);
}

TEST_CASE("ExpansionHubMotorTest MotorControllerVoltage",
          "[wpilibc][hardware]") {
  HAL_Initialize();
  ExpansionHubMotor motor{0, 0};
  MotorController& controller = motor;

  controller.SetThrottle(0.5);
  for (auto voltage : {6_V, -3_V}) {
    controller.SetVoltage(voltage);
    CHECK(controller.GetThrottle() == 0.5);
  }
}

TEST_CASE("ExpansionHubMotorTest MotorControllerInverted",
          "[wpilibc][hardware]") {
  HAL_Initialize();
  ExpansionHubMotor motor{0, 0};
  MotorController& controller = motor;

  CHECK_FALSE(controller.GetInverted());
  controller.SetInverted(true);
  CHECK(controller.GetInverted());
  controller.SetThrottle(0.5);
  CHECK(controller.GetThrottle() == 0.5);

  motor.SetReversed(false);
  CHECK_FALSE(controller.GetInverted());
  motor.SetReversed(true);
  CHECK(controller.GetInverted());
  controller.SetInverted(false);
  CHECK_FALSE(controller.GetInverted());
}

TEST_CASE("ExpansionHubMotorTest FollowerLoopDetection",
          "[wpilibc][hardware]") {
  HAL_Initialize();

  wpi::ExpansionHubMotor motor0{0, 0};
  wpi::ExpansionHubMotor motor1{0, 1};
  wpi::ExpansionHubMotor motor2{0, 2};

  // Test that a simple loop is detected
  motor1.Follow(motor2, wpi::ExpansionHubMotor::FollowDirection::Opposed);
  motor2.Follow(motor0, wpi::ExpansionHubMotor::FollowDirection::Opposed);
  CHECK_THROWS_AS(
      motor0.Follow(motor1, wpi::ExpansionHubMotor::FollowDirection::Opposed),
      wpi::RuntimeError);
}
TEST_CASE("ExpansionHubMotorTest Follower", "[wpilibc][hardware]") {
  HAL_Initialize();

  wpi::ExpansionHubMotor motor0{0, 0};
  wpi::ExpansionHubMotor motor1{0, 1};
  wpi::ExpansionHubMotor motor2{0, 2};

  motor1.Follow(motor2, wpi::ExpansionHubMotor::FollowDirection::Opposed);
  motor2.Follow(motor0, wpi::ExpansionHubMotor::FollowDirection::Opposed);
}
}  // namespace wpi
