// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/DifferentialDriveOdometry3d.hpp"

#include <numbers>

#include <gtest/gtest.h>

using namespace wpi::math;

static constexpr double kEpsilon = 1E-9;

TEST(DifferentialDriveOdometry3dTest, Initialize) {
  DifferentialDriveOdometry3d odometry{
      wpi::math::Rotation3d{0_deg, 0_deg, 90_deg}, 0_m, 0_m,
      wpi::math::Pose3d{1_m, 2_m, 0_m,
                        wpi::math::Rotation3d{0_deg, 0_deg, 45_deg}}};

  const wpi::math::Pose3d& pose = odometry.GetPose();

  EXPECT_NEAR(pose.X().value(), 1, kEpsilon);
  EXPECT_NEAR(pose.Y().value(), 2, kEpsilon);
  EXPECT_NEAR(pose.Z().value(), 0, kEpsilon);
  EXPECT_NEAR(pose.Rotation().ToRotation2d().Degrees().value(), 45, kEpsilon);
}

TEST(DifferentialDriveOdometry3dTest, EncoderDistances) {
  DifferentialDriveOdometry3d odometry{
      wpi::math::Rotation3d{0_deg, 0_deg, 45_deg}, 0_m, 0_m};

  const auto& pose =
      odometry.Update(wpi::math::Rotation3d{0_deg, 0_deg, 135_deg}, 0_m,
                      wpi::units::meter_t{5 * std::numbers::pi});

  EXPECT_NEAR(pose.X().value(), 5.0, kEpsilon);
  EXPECT_NEAR(pose.Y().value(), 5.0, kEpsilon);
  EXPECT_NEAR(pose.Z().value(), 0.0, kEpsilon);
  EXPECT_NEAR(pose.Rotation().ToRotation2d().Degrees().value(), 90.0, kEpsilon);
}

TEST(DifferentialDriveOdometry3dTest, GyroOffset) {
  DifferentialDriveOdometry3d odometry{
      wpi::math::Rotation3d{0_deg, 5_deg, 0_deg}, 0_m, 0_m,
      wpi::math::Pose3d{wpi::math::Translation3d{},
                        wpi::math::Rotation3d{0_deg, 0_deg, 90_deg}}};
  const auto& pose =
      odometry.Update(wpi::math::Rotation3d{0_deg, 10_deg, 0_deg}, 0_m, 0_m);

  EXPECT_NEAR(pose.X().value(), 0.0, kEpsilon);
  EXPECT_NEAR(pose.Y().value(), 0.0, kEpsilon);
  EXPECT_NEAR(pose.Z().value(), 0.0, kEpsilon);
  EXPECT_NEAR(wpi::units::degree_t{pose.Rotation().X()}.value(), 0.0, kEpsilon);
  EXPECT_NEAR(wpi::units::degree_t{pose.Rotation().Y()}.value(), 5.0, kEpsilon);
  EXPECT_NEAR(wpi::units::degree_t{pose.Rotation().Z()}.value(), 90.0,
              kEpsilon);
}
