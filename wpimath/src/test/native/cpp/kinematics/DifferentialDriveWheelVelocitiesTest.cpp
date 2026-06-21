// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/DifferentialDriveWheelVelocities.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("DifferentialDriveWheelVelocitiesTest Plus", "[wpimath]") {
  const wpi::math::DifferentialDriveWheelVelocities left{1.0_mps, 0.5_mps};
  const wpi::math::DifferentialDriveWheelVelocities right{2.0_mps, 1.5_mps};

  const wpi::math::DifferentialDriveWheelVelocities result = left + right;

  CHECK(3.0 == result.left.value());
  CHECK(2.0 == result.right.value());
}

TEST_CASE("DifferentialDriveWheelVelocitiesTest Minus", "[wpimath]") {
  const wpi::math::DifferentialDriveWheelVelocities left{1.0_mps, 0.5_mps};
  const wpi::math::DifferentialDriveWheelVelocities right{2.0_mps, 0.5_mps};

  const wpi::math::DifferentialDriveWheelVelocities result = left - right;

  CHECK(-1.0 == result.left.value());
  CHECK(0 == result.right.value());
}

TEST_CASE("DifferentialDriveWheelVelocitiesTest UnaryMinus", "[wpimath]") {
  const wpi::math::DifferentialDriveWheelVelocities velocities{1.0_mps,
                                                               0.5_mps};

  const wpi::math::DifferentialDriveWheelVelocities result = -velocities;

  CHECK(-1.0 == result.left.value());
  CHECK(-0.5 == result.right.value());
}

TEST_CASE("DifferentialDriveWheelVelocitiesTest Multiplication", "[wpimath]") {
  const wpi::math::DifferentialDriveWheelVelocities velocities{1.0_mps,
                                                               0.5_mps};

  const wpi::math::DifferentialDriveWheelVelocities result = velocities * 2;

  CHECK(2.0 == result.left.value());
  CHECK(1.0 == result.right.value());
}

TEST_CASE("DifferentialDriveWheelVelocitiesTest Division", "[wpimath]") {
  const wpi::math::DifferentialDriveWheelVelocities velocities{1.0_mps,
                                                               0.5_mps};

  const wpi::math::DifferentialDriveWheelVelocities result = velocities / 2;

  CHECK(0.5 == result.left.value());
  CHECK(0.25 == result.right.value());
}
