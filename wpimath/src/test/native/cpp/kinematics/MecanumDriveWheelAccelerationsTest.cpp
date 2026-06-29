// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/MecanumDriveWheelAccelerations.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/units/acceleration.hpp"

using namespace wpi::math;

static constexpr double kEpsilon = 1E-9;

TEST_CASE("MecanumDriveWheelAccelerationsTest DefaultConstructor",
          "[wpimath]") {
  MecanumDriveWheelAccelerations wheelAccelerations;

  CHECK(wheelAccelerations.frontLeft.value() ==
        Catch::Approx(0.0).margin(kEpsilon));
  CHECK(wheelAccelerations.frontRight.value() ==
        Catch::Approx(0.0).margin(kEpsilon));
  CHECK(wheelAccelerations.rearLeft.value() ==
        Catch::Approx(0.0).margin(kEpsilon));
  CHECK(wheelAccelerations.rearRight.value() ==
        Catch::Approx(0.0).margin(kEpsilon));
}

TEST_CASE("MecanumDriveWheelAccelerationsTest ParameterizedConstructor",
          "[wpimath]") {
  MecanumDriveWheelAccelerations wheelAccelerations{1.0_mps_sq, 2.0_mps_sq,
                                                    3.0_mps_sq, 4.0_mps_sq};

  CHECK(wheelAccelerations.frontLeft.value() ==
        Catch::Approx(1.0).margin(kEpsilon));
  CHECK(wheelAccelerations.frontRight.value() ==
        Catch::Approx(2.0).margin(kEpsilon));
  CHECK(wheelAccelerations.rearLeft.value() ==
        Catch::Approx(3.0).margin(kEpsilon));
  CHECK(wheelAccelerations.rearRight.value() ==
        Catch::Approx(4.0).margin(kEpsilon));
}

TEST_CASE("MecanumDriveWheelAccelerationsTest Plus", "[wpimath]") {
  const MecanumDriveWheelAccelerations left{1.0_mps_sq, 0.5_mps_sq, 2.0_mps_sq,
                                            1.5_mps_sq};
  const MecanumDriveWheelAccelerations right{2.0_mps_sq, 1.5_mps_sq, 0.5_mps_sq,
                                             1.0_mps_sq};

  const auto wheelAccelerations = left + right;

  CHECK(wheelAccelerations.frontLeft.value() ==
        Catch::Approx(3.0).margin(kEpsilon));
  CHECK(wheelAccelerations.frontRight.value() ==
        Catch::Approx(2.0).margin(kEpsilon));
  CHECK(wheelAccelerations.rearLeft.value() ==
        Catch::Approx(2.5).margin(kEpsilon));
  CHECK(wheelAccelerations.rearRight.value() ==
        Catch::Approx(2.5).margin(kEpsilon));
}

TEST_CASE("MecanumDriveWheelAccelerationsTest Minus", "[wpimath]") {
  const MecanumDriveWheelAccelerations left{5.0_mps_sq, 4.0_mps_sq, 6.0_mps_sq,
                                            2.5_mps_sq};
  const MecanumDriveWheelAccelerations right{1.0_mps_sq, 2.0_mps_sq, 3.0_mps_sq,
                                             0.5_mps_sq};

  const auto wheelAccelerations = left - right;

  CHECK(wheelAccelerations.frontLeft.value() ==
        Catch::Approx(4.0).margin(kEpsilon));
  CHECK(wheelAccelerations.frontRight.value() ==
        Catch::Approx(2.0).margin(kEpsilon));
  CHECK(wheelAccelerations.rearLeft.value() ==
        Catch::Approx(3.0).margin(kEpsilon));
  CHECK(wheelAccelerations.rearRight.value() ==
        Catch::Approx(2.0).margin(kEpsilon));
}

TEST_CASE("MecanumDriveWheelAccelerationsTest UnaryMinus", "[wpimath]") {
  const auto wheelAccelerations = -MecanumDriveWheelAccelerations{
      1.0_mps_sq, -2.0_mps_sq, 3.0_mps_sq, -4.0_mps_sq};

  CHECK(wheelAccelerations.frontLeft.value() ==
        Catch::Approx(-1.0).margin(kEpsilon));
  CHECK(wheelAccelerations.frontRight.value() ==
        Catch::Approx(2.0).margin(kEpsilon));
  CHECK(wheelAccelerations.rearLeft.value() ==
        Catch::Approx(-3.0).margin(kEpsilon));
  CHECK(wheelAccelerations.rearRight.value() ==
        Catch::Approx(4.0).margin(kEpsilon));
}

TEST_CASE("MecanumDriveWheelAccelerationsTest Multiplication", "[wpimath]") {
  const auto wheelAccelerations =
      MecanumDriveWheelAccelerations{2.0_mps_sq, 2.5_mps_sq, 3.0_mps_sq,
                                     3.5_mps_sq} *
      2.0;

  CHECK(wheelAccelerations.frontLeft.value() ==
        Catch::Approx(4.0).margin(kEpsilon));
  CHECK(wheelAccelerations.frontRight.value() ==
        Catch::Approx(5.0).margin(kEpsilon));
  CHECK(wheelAccelerations.rearLeft.value() ==
        Catch::Approx(6.0).margin(kEpsilon));
  CHECK(wheelAccelerations.rearRight.value() ==
        Catch::Approx(7.0).margin(kEpsilon));
}

TEST_CASE("MecanumDriveWheelAccelerationsTest Division", "[wpimath]") {
  const auto wheelAccelerations =
      MecanumDriveWheelAccelerations{2.0_mps_sq, 2.5_mps_sq, 1.5_mps_sq,
                                     1.0_mps_sq} /
      2.0;

  CHECK(wheelAccelerations.frontLeft.value() ==
        Catch::Approx(1.0).margin(kEpsilon));
  CHECK(wheelAccelerations.frontRight.value() ==
        Catch::Approx(1.25).margin(kEpsilon));
  CHECK(wheelAccelerations.rearLeft.value() ==
        Catch::Approx(0.75).margin(kEpsilon));
  CHECK(wheelAccelerations.rearRight.value() ==
        Catch::Approx(0.5).margin(kEpsilon));
}
