// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/math/kinematics/SwerveModuleAcceleration.hpp"
#include "wpi/units/acceleration.hpp"

using namespace wpi::math;

static constexpr double kEpsilon = 1E-9;

TEST_CASE("SwerveModuleAccelerationsTest DefaultConstructor", "[wpimath]") {
  SwerveModuleAcceleration moduleAccelerations;

  CHECK_NEAR(moduleAccelerations.acceleration.value(), 0.0, kEpsilon);
  CHECK_NEAR(moduleAccelerations.angle.Radians().value(), 0.0, kEpsilon);
}

TEST_CASE("SwerveModuleAccelerationsTest ParameterizedConstructor",
          "[wpimath]") {
  SwerveModuleAcceleration moduleAccelerations{2.5_mps_sq, Rotation2d{1.5_rad}};

  CHECK_NEAR(moduleAccelerations.acceleration.value(), 2.5, kEpsilon);
  CHECK_NEAR(moduleAccelerations.angle.Radians().value(), 1.5, kEpsilon);
}

TEST_CASE("SwerveModuleAccelerationsTest Equals", "[wpimath]") {
  SwerveModuleAcceleration moduleAccelerations1{2.0_mps_sq,
                                                Rotation2d{1.5_rad}};
  SwerveModuleAcceleration moduleAccelerations2{2.0_mps_sq,
                                                Rotation2d{1.5_rad}};
  SwerveModuleAcceleration moduleAccelerations3{2.1_mps_sq,
                                                Rotation2d{1.5_rad}};

  CHECK(moduleAccelerations1 == moduleAccelerations2);
  CHECK(moduleAccelerations1 != moduleAccelerations3);
}
