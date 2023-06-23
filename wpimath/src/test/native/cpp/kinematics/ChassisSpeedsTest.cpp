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

TEST(ChassisSpeedsTest, Sum) {
  const frc::ChassisSpeeds left{1.0_mps, 0.5_mps, 0.75_rad_per_s};
  const frc::ChassisSpeeds right{2.0_mps, 1.5_mps, 0.25_rad_per_s};

  const frc::ChassisSpeeds result = left + right;

  EXPECT_NEAR(3.0, result.vx.value(), kEpsilon);
  EXPECT_NEAR(2.0, result.vy.value(), kEpsilon);
  EXPECT_NEAR(1.0, result.omega.value(), kEpsilon);
}

TEST(ChassisSpeedsTest, Difference) {
  const frc::ChassisSpeeds left{1.0_mps, 0.5_mps, 0.75_rad_per_s};
  const frc::ChassisSpeeds right{2.0_mps, 0.5_mps, 0.25_rad_per_s};

  const frc::ChassisSpeeds result = left - right;

  EXPECT_NEAR(-1.0, result.vx.value(), kEpsilon);
  EXPECT_NEAR(0, result.vy.value(), kEpsilon);
  EXPECT_NEAR(0.5, result.omega.value(), kEpsilon);
}

TEST(ChassisSpeedsTest, UnaryMinus) {
  const frc::ChassisSpeeds speeds{1.0_mps, 0.5_mps, 0.75_rad_per_s};

  const frc::ChassisSpeeds result = -speeds;

  EXPECT_NEAR(-1.0, result.vx.value(), kEpsilon);
  EXPECT_NEAR(-0.5, result.vy.value(), kEpsilon);
  EXPECT_NEAR(-0.75, result.omega.value(), kEpsilon);
}

TEST(ChassisSpeedsTest, Multiplication) {
  const frc::ChassisSpeeds speeds{1.0_mps, 0.5_mps, 0.75_rad_per_s};

  const frc::ChassisSpeeds result = speeds * 2;

  EXPECT_NEAR(2.0, result.vx.value(), kEpsilon);
  EXPECT_NEAR(1.0, result.vy.value(), kEpsilon);
  EXPECT_NEAR(1.5, result.omega.value(), kEpsilon);
}

TEST(ChassisSpeedsTest, Division) {
  const frc::ChassisSpeeds speeds{1.0_mps, 0.5_mps, 0.75_rad_per_s};

  const frc::ChassisSpeeds result = speeds / 2;

  EXPECT_NEAR(0.5, result.vx.value(), kEpsilon);
  EXPECT_NEAR(0.25, result.vy.value(), kEpsilon);
  EXPECT_NEAR(0.375, result.omega.value(), kEpsilon);
}
