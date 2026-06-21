// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/ChassisAccelerations.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angular_acceleration.hpp"

using namespace wpi::math;

static constexpr double kEpsilon = 1E-9;

TEST_CASE("ChassisAccelerationsTest DefaultConstructor", "[wpimath]") {
  ChassisAccelerations accelerations;

  CHECK_NEAR(accelerations.ax.value(), 0.0, kEpsilon);
  CHECK_NEAR(accelerations.ay.value(), 0.0, kEpsilon);
  CHECK_NEAR(accelerations.alpha.value(), 0.0, kEpsilon);
}

TEST_CASE("ChassisAccelerationsTest ParameterizedConstructor", "[wpimath]") {
  ChassisAccelerations accelerations{1.0_mps_sq, 2.0_mps_sq, 3.0_rad_per_s_sq};

  CHECK_NEAR(accelerations.ax.value(), 1.0, kEpsilon);
  CHECK_NEAR(accelerations.ay.value(), 2.0, kEpsilon);
  CHECK_NEAR(accelerations.alpha.value(), 3.0, kEpsilon);
}

TEST_CASE("ChassisAccelerationsTest ToRobotRelative", "[wpimath]") {
  const auto chassisAccelerations =
      ChassisAccelerations{1.0_mps_sq, 0.0_mps_sq, 0.5_rad_per_s_sq}
          .ToRobotRelative(Rotation2d{-90_deg});

  CHECK_NEAR(chassisAccelerations.ax.value(), 0.0, kEpsilon);
  CHECK_NEAR(chassisAccelerations.ay.value(), 1.0, kEpsilon);
  CHECK_NEAR(chassisAccelerations.alpha.value(), 0.5, kEpsilon);
}

TEST_CASE("ChassisAccelerationsTest ToFieldRelative", "[wpimath]") {
  const auto chassisAccelerations =
      ChassisAccelerations{1.0_mps_sq, 0.0_mps_sq, 0.5_rad_per_s_sq}
          .ToFieldRelative(Rotation2d{45_deg});

  CHECK_NEAR(chassisAccelerations.ax.value(), 1.0 / std::sqrt(2.0), kEpsilon);
  CHECK_NEAR(chassisAccelerations.ay.value(), 1.0 / std::sqrt(2.0), kEpsilon);
  CHECK_NEAR(chassisAccelerations.alpha.value(), 0.5, kEpsilon);
}

TEST_CASE("ChassisAccelerationsTest Plus", "[wpimath]") {
  const ChassisAccelerations left{1.0_mps_sq, 0.5_mps_sq, 0.75_rad_per_s_sq};
  const ChassisAccelerations right{2.0_mps_sq, 1.5_mps_sq, 0.25_rad_per_s_sq};

  const auto chassisAccelerations = left + right;

  CHECK_NEAR(chassisAccelerations.ax.value(), 3.0, kEpsilon);
  CHECK_NEAR(chassisAccelerations.ay.value(), 2.0, kEpsilon);
  CHECK_NEAR(chassisAccelerations.alpha.value(), 1.0, kEpsilon);
}

TEST_CASE("ChassisAccelerationsTest Minus", "[wpimath]") {
  const ChassisAccelerations left{1.0_mps_sq, 0.5_mps_sq, 0.75_rad_per_s_sq};
  const ChassisAccelerations right{2.0_mps_sq, 0.5_mps_sq, 0.25_rad_per_s_sq};

  const auto chassisAccelerations = left - right;

  CHECK_NEAR(chassisAccelerations.ax.value(), -1.0, kEpsilon);
  CHECK_NEAR(chassisAccelerations.ay.value(), 0.0, kEpsilon);
  CHECK_NEAR(chassisAccelerations.alpha.value(), 0.5, kEpsilon);
}

TEST_CASE("ChassisAccelerationsTest UnaryMinus", "[wpimath]") {
  const auto chassisAccelerations =
      -ChassisAccelerations{1.0_mps_sq, 0.5_mps_sq, 0.75_rad_per_s_sq};

  CHECK_NEAR(chassisAccelerations.ax.value(), -1.0, kEpsilon);
  CHECK_NEAR(chassisAccelerations.ay.value(), -0.5, kEpsilon);
  CHECK_NEAR(chassisAccelerations.alpha.value(), -0.75, kEpsilon);
}

TEST_CASE("ChassisAccelerationsTest Multiplication", "[wpimath]") {
  const auto chassisAccelerations =
      ChassisAccelerations{1.0_mps_sq, 0.5_mps_sq, 0.75_rad_per_s_sq} * 2.0;

  CHECK_NEAR(chassisAccelerations.ax.value(), 2.0, kEpsilon);
  CHECK_NEAR(chassisAccelerations.ay.value(), 1.0, kEpsilon);
  CHECK_NEAR(chassisAccelerations.alpha.value(), 1.5, kEpsilon);
}

TEST_CASE("ChassisAccelerationsTest Division", "[wpimath]") {
  const auto chassisAccelerations =
      ChassisAccelerations{2.0_mps_sq, 1.0_mps_sq, 1.5_rad_per_s_sq} / 2.0;

  CHECK_NEAR(chassisAccelerations.ax.value(), 1.0, kEpsilon);
  CHECK_NEAR(chassisAccelerations.ay.value(), 0.5, kEpsilon);
  CHECK_NEAR(chassisAccelerations.alpha.value(), 0.75, kEpsilon);
}
