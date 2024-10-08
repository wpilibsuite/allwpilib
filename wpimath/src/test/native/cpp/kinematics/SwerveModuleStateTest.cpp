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
  refA.Optimize(angleA);

  EXPECT_NEAR(refA.speed.value(), 2.0, kEpsilon);
  EXPECT_NEAR(refA.angle.Degrees().value(), 0.0, kEpsilon);

  frc::Rotation2d angleB{-50_deg};
  frc::SwerveModuleState refB{4.7_mps, 41_deg};
  refB.Optimize(angleB);

  EXPECT_NEAR(refB.speed.value(), -4.7, kEpsilon);
  EXPECT_NEAR(refB.angle.Degrees().value(), -139.0, kEpsilon);
}

TEST(SwerveModuleStateTest, NoOptimize) {
  frc::Rotation2d angleA{0_deg};
  frc::SwerveModuleState refA{2_mps, 89_deg};
  refA.Optimize(angleA);

  EXPECT_NEAR(refA.speed.value(), 2.0, kEpsilon);
  EXPECT_NEAR(refA.angle.Degrees().value(), 89.0, kEpsilon);

  frc::Rotation2d angleB{0_deg};
  frc::SwerveModuleState refB{-2_mps, -2_deg};
  refB.Optimize(angleB);

  EXPECT_NEAR(refB.speed.value(), -2.0, kEpsilon);
  EXPECT_NEAR(refB.angle.Degrees().value(), -2.0, kEpsilon);
}

TEST(SwerveModuleStateTest, CosineScaling) {
  frc::Rotation2d angleA{0_deg};
  frc::SwerveModuleState refA{2_mps, 45_deg};
  refA.CosineScale(angleA);

  EXPECT_NEAR(refA.speed.value(), std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(refA.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleB{45_deg};
  frc::SwerveModuleState refB{2_mps, 45_deg};
  refB.CosineScale(angleB);

  EXPECT_NEAR(refB.speed.value(), 2.0, kEpsilon);
  EXPECT_NEAR(refB.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleC{-45_deg};
  frc::SwerveModuleState refC{2_mps, 45_deg};
  refC.CosineScale(angleC);

  EXPECT_NEAR(refC.speed.value(), 0.0, kEpsilon);
  EXPECT_NEAR(refC.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleD{135_deg};
  frc::SwerveModuleState refD{2_mps, 45_deg};
  refD.CosineScale(angleD);

  EXPECT_NEAR(refD.speed.value(), 0.0, kEpsilon);
  EXPECT_NEAR(refD.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleE{-135_deg};
  frc::SwerveModuleState refE{2_mps, 45_deg};
  refE.CosineScale(angleE);

  EXPECT_NEAR(refE.speed.value(), -2.0, kEpsilon);
  EXPECT_NEAR(refE.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleF{180_deg};
  frc::SwerveModuleState refF{2_mps, 45_deg};
  refF.CosineScale(angleF);

  EXPECT_NEAR(refF.speed.value(), -std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(refF.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleG{0_deg};
  frc::SwerveModuleState refG{-2_mps, 45_deg};
  refG.CosineScale(angleG);

  EXPECT_NEAR(refG.speed.value(), -std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(refG.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleH{45_deg};
  frc::SwerveModuleState refH{-2_mps, 45_deg};
  refH.CosineScale(angleH);

  EXPECT_NEAR(refH.speed.value(), -2.0, kEpsilon);
  EXPECT_NEAR(refH.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleI{-45_deg};
  frc::SwerveModuleState refI{-2_mps, 45_deg};
  refI.CosineScale(angleI);

  EXPECT_NEAR(refI.speed.value(), 0.0, kEpsilon);
  EXPECT_NEAR(refI.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleJ{135_deg};
  frc::SwerveModuleState refJ{-2_mps, 45_deg};
  refJ.CosineScale(angleJ);

  EXPECT_NEAR(refJ.speed.value(), 0.0, kEpsilon);
  EXPECT_NEAR(refJ.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleK{-135_deg};
  frc::SwerveModuleState refK{-2_mps, 45_deg};
  refK.CosineScale(angleK);

  EXPECT_NEAR(refK.speed.value(), 2.0, kEpsilon);
  EXPECT_NEAR(refK.angle.Degrees().value(), 45.0, kEpsilon);

  frc::Rotation2d angleL{180_deg};
  frc::SwerveModuleState refL{-2_mps, 45_deg};
  refL.CosineScale(angleL);

  EXPECT_NEAR(refL.speed.value(), std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(refL.angle.Degrees().value(), 45.0, kEpsilon);
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
