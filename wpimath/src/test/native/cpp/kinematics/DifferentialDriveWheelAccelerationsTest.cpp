// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/DifferentialDriveWheelAccelerations.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/units/acceleration.hpp"

using namespace wpi::math;

static constexpr double kEpsilon = 1E-9;

TEST_CASE("DifferentialDriveWheelAccelerationsTest DefaultConstructor",
          "[wpimath]") {
  DifferentialDriveWheelAccelerations wheelAccelerations;

  CHECK_NEAR(wheelAccelerations.left.value(), 0.0, kEpsilon);
  CHECK_NEAR(wheelAccelerations.right.value(), 0.0, kEpsilon);
}

TEST_CASE("DifferentialDriveWheelAccelerationsTest ParameterizedConstructor",
          "[wpimath]") {
  DifferentialDriveWheelAccelerations wheelAccelerations{1.5_mps_sq,
                                                         2.5_mps_sq};

  CHECK_NEAR(wheelAccelerations.left.value(), 1.5, kEpsilon);
  CHECK_NEAR(wheelAccelerations.right.value(), 2.5, kEpsilon);
}

TEST_CASE("DifferentialDriveWheelAccelerationsTest Plus", "[wpimath]") {
  const DifferentialDriveWheelAccelerations left{1.0_mps_sq, 0.5_mps_sq};
  const DifferentialDriveWheelAccelerations right{2.0_mps_sq, 1.5_mps_sq};

  const auto wheelAccelerations = left + right;

  CHECK_NEAR(wheelAccelerations.left.value(), 3.0, kEpsilon);
  CHECK_NEAR(wheelAccelerations.right.value(), 2.0, kEpsilon);
}

TEST_CASE("DifferentialDriveWheelAccelerationsTest Minus", "[wpimath]") {
  const DifferentialDriveWheelAccelerations left{1.0_mps_sq, 0.5_mps_sq};
  const DifferentialDriveWheelAccelerations right{2.0_mps_sq, 0.5_mps_sq};

  const auto wheelAccelerations = left - right;

  CHECK_NEAR(wheelAccelerations.left.value(), -1.0, kEpsilon);
  CHECK_NEAR(wheelAccelerations.right.value(), 0.0, kEpsilon);
}

TEST_CASE("DifferentialDriveWheelAccelerationsTest UnaryMinus", "[wpimath]") {
  const auto wheelAccelerations =
      -DifferentialDriveWheelAccelerations{1.0_mps_sq, 0.5_mps_sq};

  CHECK_NEAR(wheelAccelerations.left.value(), -1.0, kEpsilon);
  CHECK_NEAR(wheelAccelerations.right.value(), -0.5, kEpsilon);
}

TEST_CASE("DifferentialDriveWheelAccelerationsTest Multiplication",
          "[wpimath]") {
  const auto wheelAccelerations =
      DifferentialDriveWheelAccelerations{1.0_mps_sq, 0.5_mps_sq} * 2.0;

  CHECK_NEAR(wheelAccelerations.left.value(), 2.0, kEpsilon);
  CHECK_NEAR(wheelAccelerations.right.value(), 1.0, kEpsilon);
}

TEST_CASE("DifferentialDriveWheelAccelerationsTest Division", "[wpimath]") {
  const auto wheelAccelerations =
      DifferentialDriveWheelAccelerations{1.0_mps_sq, 0.5_mps_sq} / 2.0;

  CHECK_NEAR(wheelAccelerations.left.value(), 0.5, kEpsilon);
  CHECK_NEAR(wheelAccelerations.right.value(), 0.25, kEpsilon);
}
