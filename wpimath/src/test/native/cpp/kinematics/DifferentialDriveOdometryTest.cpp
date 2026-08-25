// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/DifferentialDriveOdometry.hpp"

#include <numbers>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"

static constexpr double EPSILON = 1E-9;

using namespace wpi::math;

TEST_CASE("DifferentialDriveOdometryTest EncoderDistances", "[wpimath]") {
  DifferentialDriveOdometry odometry{45_deg, 0_m, 0_m};

  const auto& pose =
      odometry.Update(135_deg, 0_m, wpi::units::meters<>{5 * std::numbers::pi});

  CHECK_NEAR(pose.X().value(), 5.0, EPSILON);
  CHECK_NEAR(pose.Y().value(), 5.0, EPSILON);
  CHECK_NEAR(pose.Rotation().Degrees().value(), 90.0, EPSILON);
}
