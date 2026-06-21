// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/DifferentialDriveOdometry.hpp"

#include <numbers>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

static constexpr double kEpsilon = 1E-9;

using namespace wpi::math;

TEST_CASE("DifferentialDriveOdometryTest EncoderDistances", "[wpimath]") {
  DifferentialDriveOdometry odometry{45_deg, 0_m, 0_m};

  const auto& pose =
      odometry.Update(135_deg, 0_m, wpi::units::meter_t{5 * std::numbers::pi});

  CHECK(pose.X().value() == Catch::Approx(5.0).margin(kEpsilon));
  CHECK(pose.Y().value() == Catch::Approx(5.0).margin(kEpsilon));
  CHECK(pose.Rotation().Degrees().value() ==
        Catch::Approx(90.0).margin(kEpsilon));
}
