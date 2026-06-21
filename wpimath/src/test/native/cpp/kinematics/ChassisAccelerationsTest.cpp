// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/ChassisAccelerations.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/units/acceleration.hpp"
#include "wpi/units/angular_acceleration.hpp"

using namespace wpi::math;

static constexpr double kEpsilon = 1E-9;

TEST_CASE("ChassisAccelerationsTest DefaultConstructor", "[wpimath]") {
  ChassisAccelerations accelerations;

  CHECK(accelerations.ax.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(accelerations.ay.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(accelerations.alpha.value() == Catch::Approx(0.0).margin(kEpsilon));
}

TEST_CASE("ChassisAccelerationsTest ParameterizedConstructor", "[wpimath]") {
  ChassisAccelerations accelerations{1.0_mps_sq, 2.0_mps_sq, 3.0_rad_per_s_sq};

  CHECK(accelerations.ax.value() == Catch::Approx(1.0).margin(kEpsilon));
  CHECK(accelerations.ay.value() == Catch::Approx(2.0).margin(kEpsilon));
  CHECK(accelerations.alpha.value() == Catch::Approx(3.0).margin(kEpsilon));
}

TEST_CASE("ChassisAccelerationsTest ToRobotRelative", "[wpimath]") {
  const auto chassisAccelerations =
      ChassisAccelerations{1.0_mps_sq, 0.0_mps_sq, 0.5_rad_per_s_sq}
          .ToRobotRelative(Rotation2d{-90_deg});

  CHECK(chassisAccelerations.ax.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(chassisAccelerations.ay.value() == Catch::Approx(1.0).margin(kEpsilon));
  CHECK(chassisAccelerations.alpha.value() ==
        Catch::Approx(0.5).margin(kEpsilon));
}

TEST_CASE("ChassisAccelerationsTest ToFieldRelative", "[wpimath]") {
  const auto chassisAccelerations =
      ChassisAccelerations{1.0_mps_sq, 0.0_mps_sq, 0.5_rad_per_s_sq}
          .ToFieldRelative(Rotation2d{45_deg});

  CHECK(chassisAccelerations.ax.value() ==
        Catch::Approx(1.0 / std::sqrt(2.0)).margin(kEpsilon));
  CHECK(chassisAccelerations.ay.value() ==
        Catch::Approx(1.0 / std::sqrt(2.0)).margin(kEpsilon));
  CHECK(chassisAccelerations.alpha.value() ==
        Catch::Approx(0.5).margin(kEpsilon));
}

TEST_CASE("ChassisAccelerationsTest Plus", "[wpimath]") {
  const ChassisAccelerations left{1.0_mps_sq, 0.5_mps_sq, 0.75_rad_per_s_sq};
  const ChassisAccelerations right{2.0_mps_sq, 1.5_mps_sq, 0.25_rad_per_s_sq};

  const auto chassisAccelerations = left + right;

  CHECK(chassisAccelerations.ax.value() == Catch::Approx(3.0).margin(kEpsilon));
  CHECK(chassisAccelerations.ay.value() == Catch::Approx(2.0).margin(kEpsilon));
  CHECK(chassisAccelerations.alpha.value() ==
        Catch::Approx(1.0).margin(kEpsilon));
}

TEST_CASE("ChassisAccelerationsTest Minus", "[wpimath]") {
  const ChassisAccelerations left{1.0_mps_sq, 0.5_mps_sq, 0.75_rad_per_s_sq};
  const ChassisAccelerations right{2.0_mps_sq, 0.5_mps_sq, 0.25_rad_per_s_sq};

  const auto chassisAccelerations = left - right;

  CHECK(chassisAccelerations.ax.value() ==
        Catch::Approx(-1.0).margin(kEpsilon));
  CHECK(chassisAccelerations.ay.value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(chassisAccelerations.alpha.value() ==
        Catch::Approx(0.5).margin(kEpsilon));
}

TEST_CASE("ChassisAccelerationsTest UnaryMinus", "[wpimath]") {
  const auto chassisAccelerations =
      -ChassisAccelerations{1.0_mps_sq, 0.5_mps_sq, 0.75_rad_per_s_sq};

  CHECK(chassisAccelerations.ax.value() ==
        Catch::Approx(-1.0).margin(kEpsilon));
  CHECK(chassisAccelerations.ay.value() ==
        Catch::Approx(-0.5).margin(kEpsilon));
  CHECK(chassisAccelerations.alpha.value() ==
        Catch::Approx(-0.75).margin(kEpsilon));
}

TEST_CASE("ChassisAccelerationsTest Multiplication", "[wpimath]") {
  const auto chassisAccelerations =
      ChassisAccelerations{1.0_mps_sq, 0.5_mps_sq, 0.75_rad_per_s_sq} * 2.0;

  CHECK(chassisAccelerations.ax.value() == Catch::Approx(2.0).margin(kEpsilon));
  CHECK(chassisAccelerations.ay.value() == Catch::Approx(1.0).margin(kEpsilon));
  CHECK(chassisAccelerations.alpha.value() ==
        Catch::Approx(1.5).margin(kEpsilon));
}

TEST_CASE("ChassisAccelerationsTest Division", "[wpimath]") {
  const auto chassisAccelerations =
      ChassisAccelerations{2.0_mps_sq, 1.0_mps_sq, 1.5_rad_per_s_sq} / 2.0;

  CHECK(chassisAccelerations.ax.value() == Catch::Approx(1.0).margin(kEpsilon));
  CHECK(chassisAccelerations.ay.value() == Catch::Approx(0.5).margin(kEpsilon));
  CHECK(chassisAccelerations.alpha.value() ==
        Catch::Approx(0.75).margin(kEpsilon));
}
