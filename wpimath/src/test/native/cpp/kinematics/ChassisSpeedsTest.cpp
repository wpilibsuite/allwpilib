// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/ChassisSpeeds.h"
#include "gtest/gtest.h"

static constexpr double kEpsilon = 1E-9;

TEST(ChassisSpeedsTest, FieldRelativeConstruction) {
  const auto chassisSpeeds = frc::ChassisSpeeds::FromFieldRelativeSpeeds(
      1.0_mps, 0.0_mps, 0.5_rad_per_s, -90.0_deg);

  EXPECT_NEAR(0.0, chassisSpeeds.vx.value(), kEpsilon);
  EXPECT_NEAR(1.0, chassisSpeeds.vy.value(), kEpsilon);
  EXPECT_NEAR(0.5, chassisSpeeds.omega.value(), kEpsilon);
}
