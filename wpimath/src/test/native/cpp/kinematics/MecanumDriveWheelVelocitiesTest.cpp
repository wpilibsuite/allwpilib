// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/MecanumDriveWheelVelocities.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("MecanumDriveWheelVelocitiesTest Plus", "[wpimath]") {
  const wpi::math::MecanumDriveWheelVelocities left{1.0_mps, 0.5_mps, 2.0_mps,
                                                    1.5_mps};
  const wpi::math::MecanumDriveWheelVelocities right{2.0_mps, 1.5_mps, 0.5_mps,
                                                     1.0_mps};

  const wpi::math::MecanumDriveWheelVelocities result = left + right;

  CHECK(3.0 == result.frontLeft.value());
  CHECK(2.0 == result.frontRight.value());
  CHECK(2.5 == result.rearLeft.value());
  CHECK(2.5 == result.rearRight.value());
}

TEST_CASE("MecanumDriveWheelVelocitiesTest Minus", "[wpimath]") {
  const wpi::math::MecanumDriveWheelVelocities left{1.0_mps, 0.5_mps, 2.0_mps,
                                                    1.5_mps};
  const wpi::math::MecanumDriveWheelVelocities right{2.0_mps, 1.5_mps, 0.5_mps,
                                                     1.0_mps};

  const wpi::math::MecanumDriveWheelVelocities result = left - right;

  CHECK(-1.0 == result.frontLeft.value());
  CHECK(-1.0 == result.frontRight.value());
  CHECK(1.5 == result.rearLeft.value());
  CHECK(0.5 == result.rearRight.value());
}

TEST_CASE("MecanumDriveWheelVelocitiesTest UnaryMinus", "[wpimath]") {
  const wpi::math::MecanumDriveWheelVelocities velocities{1.0_mps, 0.5_mps,
                                                          2.0_mps, 1.5_mps};

  const wpi::math::MecanumDriveWheelVelocities result = -velocities;

  CHECK(-1.0 == result.frontLeft.value());
  CHECK(-0.5 == result.frontRight.value());
  CHECK(-2.0 == result.rearLeft.value());
  CHECK(-1.5 == result.rearRight.value());
}

TEST_CASE("MecanumDriveWheelVelocitiesTest Multiplication", "[wpimath]") {
  const wpi::math::MecanumDriveWheelVelocities velocities{1.0_mps, 0.5_mps,
                                                          2.0_mps, 1.5_mps};

  const wpi::math::MecanumDriveWheelVelocities result = velocities * 2;

  CHECK(2.0 == result.frontLeft.value());
  CHECK(1.0 == result.frontRight.value());
  CHECK(4.0 == result.rearLeft.value());
  CHECK(3.0 == result.rearRight.value());
}

TEST_CASE("MecanumDriveWheelVelocitiesTest Division", "[wpimath]") {
  const wpi::math::MecanumDriveWheelVelocities velocities{1.0_mps, 0.5_mps,
                                                          2.0_mps, 1.5_mps};

  const wpi::math::MecanumDriveWheelVelocities result = velocities / 2;

  CHECK(0.5 == result.frontLeft.value());
  CHECK(0.25 == result.frontRight.value());
  CHECK(1.0 == result.rearLeft.value());
  CHECK(0.75 == result.rearRight.value());
}
