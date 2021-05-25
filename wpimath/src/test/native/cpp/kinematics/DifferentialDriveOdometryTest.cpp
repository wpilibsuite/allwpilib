// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <wpi/numbers>

#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/kinematics/DifferentialDriveOdometry.h"
#include "gtest/gtest.h"

static constexpr double kEpsilon = 1E-9;

using namespace frc;

TEST(DifferentialDriveOdometry, EncoderDistances) {
  DifferentialDriveOdometry odometry{Rotation2d(45_deg)};

  const auto& pose = odometry.Update(Rotation2d(135_deg), 0_m,
                                     units::meter_t(5 * wpi::numbers::pi));

  EXPECT_NEAR(pose.X().to<double>(), 5.0, kEpsilon);
  EXPECT_NEAR(pose.Y().to<double>(), 5.0, kEpsilon);
  EXPECT_NEAR(pose.Rotation().Degrees().to<double>(), 90.0, kEpsilon);
}
