// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/geometry/Rotation2d.h"
#include "frc/kinematics/SwerveModuleState.h"

static constexpr double kEpsilon = 1E-9;

TEST(SwerveModuleStateTest, Optimize) {
  frc::Rotation2d angleA{45_deg};
  frc::SwerveModuleState refA{-2_mps, 180_deg};
  auto optimizedA = frc::SwerveModuleState::Optimize(refA, angleA);

  EXPECT_NEAR(optimizedA.speed.value(), 2.0, kEpsilon);
  EXPECT_NEAR(optimizedA.angle.Degrees().value(), 0.0, kEpsilon);

  frc::Rotation2d angleB{-50_deg};
  frc::SwerveModuleState refB{4.7_mps, 41_deg};
  auto optimizedB = frc::SwerveModuleState::Optimize(refB, angleB);

  EXPECT_NEAR(optimizedB.speed.value(), -4.7, kEpsilon);
  EXPECT_NEAR(optimizedB.angle.Degrees().value(), -139.0, kEpsilon);
}

TEST(SwerveModuleStateTest, NoOptimize) {
  frc::Rotation2d angleA{0_deg};
  frc::SwerveModuleState refA{2_mps, 89_deg};
  auto optimizedA = frc::SwerveModuleState::Optimize(refA, angleA);

  EXPECT_NEAR(optimizedA.speed.value(), 2.0, kEpsilon);
  EXPECT_NEAR(optimizedA.angle.Degrees().value(), 89.0, kEpsilon);

  frc::Rotation2d angleB{0_deg};
  frc::SwerveModuleState refB{-2_mps, -2_deg};
  auto optimizedB = frc::SwerveModuleState::Optimize(refB, angleB);

  EXPECT_NEAR(optimizedB.speed.value(), -2.0, kEpsilon);
  EXPECT_NEAR(optimizedB.angle.Degrees().value(), -2.0, kEpsilon);
}

TEST(SwerveModuleStateTest, Equality) {
  frc::SwerveModuleState state1{2_mps, 90_deg};
  frc::SwerveModuleState state2{2_mps, 90_deg};

  EXPECT_EQ(state1, state2);
}

TEST(SwerveModuleStateTest, Inequality) {
  frc::SwerveModuleState state1{1_mps, 90_deg};
  frc::SwerveModuleState state2{2_mps, 90_deg};
  frc::SwerveModuleState state3{1_mps, 89_deg};

  EXPECT_NE(state1, state2);
  EXPECT_NE(state1, state3);
}
