// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/controller/PIDController.hpp"

static constexpr double kSetpoint = 50.0;
static constexpr double kRange = 200;
static constexpr double kTolerance = 10.0;

TEST_CASE("PIDToleranceTest InitialTolerance", "[wpimath]") {
  wpi::math::PIDController controller{0.5, 0.0, 0.0};
  controller.EnableContinuousInput(-kRange / 2, kRange / 2);

  CHECK_FALSE(controller.AtSetpoint());
}

TEST_CASE("PIDToleranceTest AbsoluteTolerance", "[wpimath]") {
  wpi::math::PIDController controller{0.5, 0.0, 0.0};
  controller.EnableContinuousInput(-kRange / 2, kRange / 2);

  CHECK_FALSE(controller.AtSetpoint());

  controller.SetTolerance(kTolerance);
  controller.SetSetpoint(kSetpoint);

  UNSCOPED_INFO(
      "Error was in tolerance when it should not have been. Error was "
      << controller.GetError());
  CHECK_FALSE(controller.AtSetpoint());

  controller.Calculate(0.0);

  UNSCOPED_INFO(
      "Error was in tolerance when it should not have been. Error was "
      << controller.GetError());
  CHECK_FALSE(controller.AtSetpoint());

  controller.Calculate(kSetpoint + kTolerance / 2);

  UNSCOPED_INFO(
      "Error was not in tolerance when it should have been. Error was "
      << controller.GetError());
  CHECK(controller.AtSetpoint());

  controller.Calculate(kSetpoint + 10 * kTolerance);

  UNSCOPED_INFO(
      "Error was in tolerance when it should not have been. Error was "
      << controller.GetError());
  CHECK_FALSE(controller.AtSetpoint());
}
