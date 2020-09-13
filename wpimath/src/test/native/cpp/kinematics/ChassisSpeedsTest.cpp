/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/kinematics/ChassisSpeeds.h"
#include "gtest/gtest.h"

static constexpr double kEpsilon = 1E-9;

TEST(ChassisSpeeds, FieldRelativeConstruction) {
  const auto chassisSpeeds = frc::ChassisSpeeds::FromFieldRelativeSpeeds(
      1.0_mps, 0.0_mps, 0.5_rad_per_s, frc::Rotation2d(-90.0_deg));

  EXPECT_NEAR(0.0, chassisSpeeds.vx.to<double>(), kEpsilon);
  EXPECT_NEAR(1.0, chassisSpeeds.vy.to<double>(), kEpsilon);
  EXPECT_NEAR(0.5, chassisSpeeds.omega.to<double>(), kEpsilon);
}
