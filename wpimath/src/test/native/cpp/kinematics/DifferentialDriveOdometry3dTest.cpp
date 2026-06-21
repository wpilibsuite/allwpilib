// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/DifferentialDriveOdometry3d.hpp"

#include <numbers>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace wpi::math;

static constexpr double kEpsilon = 1E-9;

TEST_CASE("DifferentialDriveOdometry3dTest Initialize", "[wpimath]") {
  DifferentialDriveOdometry3d odometry{
      wpi::math::Rotation3d{0_deg, 0_deg, 90_deg}, 0_m, 0_m,
      wpi::math::Pose3d{1_m, 2_m, 0_m,
                        wpi::math::Rotation3d{0_deg, 0_deg, 45_deg}}};

  const wpi::math::Pose3d& pose = odometry.GetPose();

  CHECK(pose.X().value() == Catch::Approx(1).margin(kEpsilon));
  CHECK(pose.Y().value() == Catch::Approx(2).margin(kEpsilon));
  CHECK(pose.Z().value() == Catch::Approx(0).margin(kEpsilon));
  CHECK(pose.Rotation().ToRotation2d().Degrees().value() ==
        Catch::Approx(45).margin(kEpsilon));
}

TEST_CASE("DifferentialDriveOdometry3dTest EncoderDistances", "[wpimath]") {
  DifferentialDriveOdometry3d odometry{
      wpi::math::Rotation3d{0_deg, 0_deg, 45_deg}, 0_m, 0_m};

  const auto& pose =
      odometry.Update(wpi::math::Rotation3d{0_deg, 0_deg, 135_deg}, 0_m,
                      wpi::units::meter_t{5 * std::numbers::pi});

  CHECK(pose.X().value() == Catch::Approx(5.0).margin(kEpsilon));
  CHECK(pose.Y().value() == Catch::Approx(5.0).margin(kEpsilon));
  CHECK(pose.Z().value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(pose.Rotation().ToRotation2d().Degrees().value() ==
        Catch::Approx(90.0).margin(kEpsilon));
}

TEST_CASE("DifferentialDriveOdometry3dTest GyroOffset", "[wpimath]") {
  DifferentialDriveOdometry3d odometry{
      wpi::math::Rotation3d{0_deg, 5_deg, 0_deg}, 0_m, 0_m,
      wpi::math::Pose3d{wpi::math::Translation3d{},
                        wpi::math::Rotation3d{0_deg, 0_deg, 90_deg}}};
  const auto& pose =
      odometry.Update(wpi::math::Rotation3d{0_deg, 10_deg, 0_deg}, 0_m, 0_m);

  CHECK(pose.X().value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(pose.Y().value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(pose.Z().value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(wpi::units::degree_t{pose.Rotation().X()}.value() ==
        Catch::Approx(0.0).margin(kEpsilon));
  CHECK(wpi::units::degree_t{pose.Rotation().Y()}.value() ==
        Catch::Approx(5.0).margin(kEpsilon));
  CHECK(wpi::units::degree_t{pose.Rotation().Z()}.value() ==
        Catch::Approx(90.0).margin(kEpsilon));
}
