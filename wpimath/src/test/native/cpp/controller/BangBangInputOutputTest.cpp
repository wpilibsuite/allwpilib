// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/controller/BangBangController.hpp"

TEST_CASE("BangBangInputOutputTest ShouldOutput", "[wpimath]") {
  wpi::math::BangBangController controller;

  CHECK(controller.Calculate(0, 1) == Catch::Approx(1));
}

TEST_CASE("BangBangInputOutputTest ShouldNotOutput", "[wpimath]") {
  wpi::math::BangBangController controller;

  CHECK(controller.Calculate(1, 0) == Catch::Approx(0));
}
