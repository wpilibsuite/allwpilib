/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <wpi/math>

#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/kinematics/DifferentialDriveOdometry.h"
#include "gtest/gtest.h"

static constexpr double kEpsilon = 1E-9;

using namespace frc;

TEST(DifferentialDriveOdometry, EncoderDistances) {
  DifferentialDriveOdometry odometry{Rotation2d(45_deg)};

  const auto& pose = odometry.Update(Rotation2d(135_deg), 0_m,
                                     units::meter_t(5 * wpi::math::pi));

  EXPECT_NEAR(pose.X().to<double>(), 5.0, kEpsilon);
  EXPECT_NEAR(pose.Y().to<double>(), 5.0, kEpsilon);
  EXPECT_NEAR(pose.Rotation().Degrees().to<double>(), 90.0, kEpsilon);
}
