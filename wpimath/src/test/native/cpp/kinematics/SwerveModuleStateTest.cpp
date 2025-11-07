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
  auto optimizedA = refA.Optimize(angleA);

  EXPECT_NEAR(optimizedA.speed.value(), 2.0, kEpsilon);
  EXPECT_NEAR(optimizedA.angle.Degrees().value(), 0.0, kEpsilon);

  frc::Rotation2d angleB{-50_deg};
  frc::SwerveModuleState refB{4.7_mps, 41_deg};
  auto optimizedB = refB.Optimize(angleB);

  EXPECT_NEAR(optimizedB.speed.value(), -4.7, kEpsilon);
  EXPECT_NEAR(optimizedB.angle.Degrees().value(), -139.0, kEpsilon);
}

TEST(SwerveModuleStateTest, NoOptimize) {
  frc::Rotation2d angleA{0_deg};
  frc::SwerveModuleState refA{2_mps, 89_deg};
  auto optimizedA = refA.Optimize(angleA);

  EXPECT_NEAR(optimizedA.speed.value(), 2.0, kEpsilon);
  EXPECT_NEAR(optimizedA.angle.Degrees().value(), 89.0, kEpsilon);

  frc::Rotation2d angleB{0_deg};
  frc::SwerveModuleState refB{-2_mps, -2_deg};
  auto optimizedB = refB.Optimize(angleB);

  EXPECT_NEAR(optimizedB.speed.value(), -2.0, kEpsilon);
  EXPECT_NEAR(optimizedB.angle.Degrees().value(), -2.0, kEpsilon);
}

TEST(SwerveModuleStateTest, CosineScaling) {
  frc::Rotation2d angleA{0_deg};
  frc::SwerveModuleState refA{2_mps, 45_deg};
  auto optimizedA = refA.CosineScale(angleA);

  EXPECT_NEAR(optimizedA.speed.value(), std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(optimizedA.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleB{45_deg};
  frc::SwerveModuleState refB{2_mps, 45_deg};
  auto optimizedB = refB.CosineScale(angleB);

  EXPECT_NEAR(optimizedB.speed.value(), 2.0, kEpsilon);
  EXPECT_NEAR(optimizedB.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleC{-45_deg};
  frc::SwerveModuleState refC{2_mps, 45_deg};
  auto optimizedC = refC.CosineScale(angleC);

  EXPECT_NEAR(optimizedC.speed.value(), 0.0, kEpsilon);
  EXPECT_NEAR(optimizedC.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleD{135_deg};
  frc::SwerveModuleState refD{2_mps, 45_deg};
  auto optimizedD = refD.CosineScale(angleD);

  EXPECT_NEAR(optimizedD.speed.value(), 0.0, kEpsilon);
  EXPECT_NEAR(optimizedD.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleE{-135_deg};
  frc::SwerveModuleState refE{2_mps, 45_deg};
  auto optimizedE = refE.CosineScale(angleE);

  EXPECT_NEAR(optimizedE.speed.value(), -2.0, kEpsilon);
  EXPECT_NEAR(optimizedE.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleF{180_deg};
  frc::SwerveModuleState refF{2_mps, 45_deg};
  auto optimizedF = refF.CosineScale(angleF);

  EXPECT_NEAR(optimizedF.speed.value(), -std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(optimizedF.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleG{0_deg};
  frc::SwerveModuleState refG{-2_mps, 45_deg};
  auto optimizedG = refG.CosineScale(angleG);

  EXPECT_NEAR(optimizedG.speed.value(), -std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(optimizedG.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleH{45_deg};
  frc::SwerveModuleState refH{-2_mps, 45_deg};
  auto optimizedH = refH.CosineScale(angleH);

  EXPECT_NEAR(optimizedH.speed.value(), -2.0, kEpsilon);
  EXPECT_NEAR(optimizedH.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleI{-45_deg};
  frc::SwerveModuleState refI{-2_mps, 45_deg};
  auto optimizedI = refI.CosineScale(angleI);

  EXPECT_NEAR(optimizedI.speed.value(), 0.0, kEpsilon);
  EXPECT_NEAR(optimizedI.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleJ{135_deg};
  frc::SwerveModuleState refJ{-2_mps, 45_deg};
  auto optimizedJ = refJ.CosineScale(angleJ);

  EXPECT_NEAR(optimizedJ.speed.value(), 0.0, kEpsilon);
  EXPECT_NEAR(optimizedJ.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleK{-135_deg};
  frc::SwerveModuleState refK{-2_mps, 45_deg};
  auto optimizedK = refK.CosineScale(angleK);

  EXPECT_NEAR(optimizedK.speed.value(), 2.0, kEpsilon);
  EXPECT_NEAR(optimizedK.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleL{180_deg};
  frc::SwerveModuleState refL{-2_mps, 45_deg};
  auto optimizedL = refL.CosineScale(angleL);

  EXPECT_NEAR(optimizedL.speed.value(), std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(optimizedL.angle.Degrees().value(), 45.0, kEpsilon);
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
