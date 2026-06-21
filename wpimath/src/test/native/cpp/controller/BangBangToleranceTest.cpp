// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/controller/BangBangController.hpp"

TEST_CASE("BangBangToleranceTest InTolerance", "[wpimath]") {
  wpi::math::BangBangController controller{0.1};

  controller.SetSetpoint(1);
  controller.Calculate(1);
  CHECK(controller.AtSetpoint());
}

TEST_CASE("BangBangToleranceTest OutOfTolerance", "[wpimath]") {
  wpi::math::BangBangController controller{0.1};

  controller.SetSetpoint(1);
  controller.Calculate(0);
  CHECK_FALSE(controller.AtSetpoint());
}
