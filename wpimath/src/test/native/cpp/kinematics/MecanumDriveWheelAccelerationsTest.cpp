// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/MecanumDriveWheelAccelerations.hpp"

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/units/acceleration.hpp"

using namespace wpi::math;

static constexpr double EPSILON = 1E-9;

TEST_CASE("MecanumDriveWheelAccelerationsTest DefaultConstructor",
          "[wpimath]") {
  MecanumDriveWheelAccelerations wheelAccelerations;

  CHECK_NEAR(wheelAccelerations.frontLeft.value(), 0.0, EPSILON);
  CHECK_NEAR(wheelAccelerations.frontRight.value(), 0.0, EPSILON);
  CHECK_NEAR(wheelAccelerations.rearLeft.value(), 0.0, EPSILON);
  CHECK_NEAR(wheelAccelerations.rearRight.value(), 0.0, EPSILON);
}

TEST_CASE("MecanumDriveWheelAccelerationsTest ParameterizedConstructor",
          "[wpimath]") {
  MecanumDriveWheelAccelerations wheelAccelerations{1.0_mps2, 2.0_mps2,
                                                    3.0_mps2, 4.0_mps2};

  CHECK_NEAR(wheelAccelerations.frontLeft.value(), 1.0, EPSILON);
  CHECK_NEAR(wheelAccelerations.frontRight.value(), 2.0, EPSILON);
  CHECK_NEAR(wheelAccelerations.rearLeft.value(), 3.0, EPSILON);
  CHECK_NEAR(wheelAccelerations.rearRight.value(), 4.0, EPSILON);
}

TEST_CASE("MecanumDriveWheelAccelerationsTest Plus", "[wpimath]") {
  const MecanumDriveWheelAccelerations left{1.0_mps2, 0.5_mps2, 2.0_mps2,
                                            1.5_mps2};
  const MecanumDriveWheelAccelerations right{2.0_mps2, 1.5_mps2, 0.5_mps2,
                                             1.0_mps2};

  const auto wheelAccelerations = left + right;

  CHECK_NEAR(wheelAccelerations.frontLeft.value(), 3.0, EPSILON);
  CHECK_NEAR(wheelAccelerations.frontRight.value(), 2.0, EPSILON);
  CHECK_NEAR(wheelAccelerations.rearLeft.value(), 2.5, EPSILON);
  CHECK_NEAR(wheelAccelerations.rearRight.value(), 2.5, EPSILON);
}

TEST_CASE("MecanumDriveWheelAccelerationsTest Minus", "[wpimath]") {
  const MecanumDriveWheelAccelerations left{5.0_mps2, 4.0_mps2, 6.0_mps2,
                                            2.5_mps2};
  const MecanumDriveWheelAccelerations right{1.0_mps2, 2.0_mps2, 3.0_mps2,
                                             0.5_mps2};

  const auto wheelAccelerations = left - right;

  CHECK_NEAR(wheelAccelerations.frontLeft.value(), 4.0, EPSILON);
  CHECK_NEAR(wheelAccelerations.frontRight.value(), 2.0, EPSILON);
  CHECK_NEAR(wheelAccelerations.rearLeft.value(), 3.0, EPSILON);
  CHECK_NEAR(wheelAccelerations.rearRight.value(), 2.0, EPSILON);
}

TEST_CASE("MecanumDriveWheelAccelerationsTest UnaryMinus", "[wpimath]") {
  const auto wheelAccelerations =
      -MecanumDriveWheelAccelerations{1.0_mps2, -2.0_mps2, 3.0_mps2, -4.0_mps2};

  CHECK_NEAR(wheelAccelerations.frontLeft.value(), -1.0, EPSILON);
  CHECK_NEAR(wheelAccelerations.frontRight.value(), 2.0, EPSILON);
  CHECK_NEAR(wheelAccelerations.rearLeft.value(), -3.0, EPSILON);
  CHECK_NEAR(wheelAccelerations.rearRight.value(), 4.0, EPSILON);
}

TEST_CASE("MecanumDriveWheelAccelerationsTest Multiplication", "[wpimath]") {
  const auto wheelAccelerations =
      MecanumDriveWheelAccelerations{2.0_mps2, 2.5_mps2, 3.0_mps2, 3.5_mps2} *
      2.0;

  CHECK_NEAR(wheelAccelerations.frontLeft.value(), 4.0, EPSILON);
  CHECK_NEAR(wheelAccelerations.frontRight.value(), 5.0, EPSILON);
  CHECK_NEAR(wheelAccelerations.rearLeft.value(), 6.0, EPSILON);
  CHECK_NEAR(wheelAccelerations.rearRight.value(), 7.0, EPSILON);
}

TEST_CASE("MecanumDriveWheelAccelerationsTest Division", "[wpimath]") {
  const auto wheelAccelerations =
      MecanumDriveWheelAccelerations{2.0_mps2, 2.5_mps2, 1.5_mps2, 1.0_mps2} /
      2.0;

  CHECK_NEAR(wheelAccelerations.frontLeft.value(), 1.0, EPSILON);
  CHECK_NEAR(wheelAccelerations.frontRight.value(), 1.25, EPSILON);
  CHECK_NEAR(wheelAccelerations.rearLeft.value(), 0.75, EPSILON);
  CHECK_NEAR(wheelAccelerations.rearRight.value(), 0.5, EPSILON);
}
