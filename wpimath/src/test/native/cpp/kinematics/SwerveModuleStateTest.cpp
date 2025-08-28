// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpimath/geometry/Rotation2d.h"
#include "wpimath/kinematics/SwerveModuleState.h"

static constexpr double kEpsilon = 1E-9;

TEST(SwerveModuleStateTest, Optimize) {
  wpimath::Rotation2d angleA{45_deg};
  wpimath::SwerveModuleState refA{-2_mps, 180_deg};
  refA.Optimize(angleA);

  EXPECT_NEAR(refA.speed.value(), 2.0, kEpsilon);
  EXPECT_NEAR(refA.angle.Degrees().value(), 0.0, kEpsilon);

  wpimath::Rotation2d angleB{-50_deg};
  wpimath::SwerveModuleState refB{4.7_mps, 41_deg};
  refB.Optimize(angleB);

  EXPECT_NEAR(refB.speed.value(), -4.7, kEpsilon);
  EXPECT_NEAR(refB.angle.Degrees().value(), -139.0, kEpsilon);
}

TEST(SwerveModuleStateTest, NoOptimize) {
  wpimath::Rotation2d angleA{0_deg};
  wpimath::SwerveModuleState refA{2_mps, 89_deg};
  refA.Optimize(angleA);

  EXPECT_NEAR(refA.speed.value(), 2.0, kEpsilon);
  EXPECT_NEAR(refA.angle.Degrees().value(), 89.0, kEpsilon);

  wpimath::Rotation2d angleB{0_deg};
  wpimath::SwerveModuleState refB{-2_mps, -2_deg};
  refB.Optimize(angleB);

  EXPECT_NEAR(refB.speed.value(), -2.0, kEpsilon);
  EXPECT_NEAR(refB.angle.Degrees().value(), -2.0, kEpsilon);
}

TEST(SwerveModuleStateTest, CosineScaling) {
  wpimath::Rotation2d angleA{0_deg};
  wpimath::SwerveModuleState refA{2_mps, 45_deg};
  refA.CosineScale(angleA);

  EXPECT_NEAR(refA.speed.value(), std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(refA.angle.Degrees().value(), 45.0, kEpsilon);

  wpimath::Rotation2d angleB{45_deg};
  wpimath::SwerveModuleState refB{2_mps, 45_deg};
  refB.CosineScale(angleB);

  EXPECT_NEAR(refB.speed.value(), 2.0, kEpsilon);
  EXPECT_NEAR(refB.angle.Degrees().value(), 45.0, kEpsilon);

  wpimath::Rotation2d angleC{-45_deg};
  wpimath::SwerveModuleState refC{2_mps, 45_deg};
  refC.CosineScale(angleC);

  EXPECT_NEAR(refC.speed.value(), 0.0, kEpsilon);
  EXPECT_NEAR(refC.angle.Degrees().value(), 45.0, kEpsilon);

  wpimath::Rotation2d angleD{135_deg};
  wpimath::SwerveModuleState refD{2_mps, 45_deg};
  refD.CosineScale(angleD);

  EXPECT_NEAR(refD.speed.value(), 0.0, kEpsilon);
  EXPECT_NEAR(refD.angle.Degrees().value(), 45.0, kEpsilon);

  wpimath::Rotation2d angleE{-135_deg};
  wpimath::SwerveModuleState refE{2_mps, 45_deg};
  refE.CosineScale(angleE);

  EXPECT_NEAR(refE.speed.value(), -2.0, kEpsilon);
  EXPECT_NEAR(refE.angle.Degrees().value(), 45.0, kEpsilon);

  wpimath::Rotation2d angleF{180_deg};
  wpimath::SwerveModuleState refF{2_mps, 45_deg};
  refF.CosineScale(angleF);

  EXPECT_NEAR(refF.speed.value(), -std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(refF.angle.Degrees().value(), 45.0, kEpsilon);

  wpimath::Rotation2d angleG{0_deg};
  wpimath::SwerveModuleState refG{-2_mps, 45_deg};
  refG.CosineScale(angleG);

  EXPECT_NEAR(refG.speed.value(), -std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(refG.angle.Degrees().value(), 45.0, kEpsilon);

  wpimath::Rotation2d angleH{45_deg};
  wpimath::SwerveModuleState refH{-2_mps, 45_deg};
  refH.CosineScale(angleH);

  EXPECT_NEAR(refH.speed.value(), -2.0, kEpsilon);
  EXPECT_NEAR(refH.angle.Degrees().value(), 45.0, kEpsilon);

  wpimath::Rotation2d angleI{-45_deg};
  wpimath::SwerveModuleState refI{-2_mps, 45_deg};
  refI.CosineScale(angleI);

  EXPECT_NEAR(refI.speed.value(), 0.0, kEpsilon);
  EXPECT_NEAR(refI.angle.Degrees().value(), 45.0, kEpsilon);

  wpimath::Rotation2d angleJ{135_deg};
  wpimath::SwerveModuleState refJ{-2_mps, 45_deg};
  refJ.CosineScale(angleJ);

  EXPECT_NEAR(refJ.speed.value(), 0.0, kEpsilon);
  EXPECT_NEAR(refJ.angle.Degrees().value(), 45.0, kEpsilon);

  wpimath::Rotation2d angleK{-135_deg};
  wpimath::SwerveModuleState refK{-2_mps, 45_deg};
  refK.CosineScale(angleK);

  EXPECT_NEAR(refK.speed.value(), 2.0, kEpsilon);
  EXPECT_NEAR(refK.angle.Degrees().value(), 45.0, kEpsilon);

  wpimath::Rotation2d angleL{180_deg};
  wpimath::SwerveModuleState refL{-2_mps, 45_deg};
  refL.CosineScale(angleL);

  EXPECT_NEAR(refL.speed.value(), std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(refL.angle.Degrees().value(), 45.0, kEpsilon);
}

TEST(SwerveModuleStateTest, Equality) {
  wpimath::SwerveModuleState state1{2_mps, 90_deg};
  wpimath::SwerveModuleState state2{2_mps, 90_deg};

  EXPECT_EQ(state1, state2);
}

TEST(SwerveModuleStateTest, Inequality) {
  wpimath::SwerveModuleState state1{1_mps, 90_deg};
  wpimath::SwerveModuleState state2{2_mps, 90_deg};
  wpimath::SwerveModuleState state3{1_mps, 89_deg};

  EXPECT_NE(state1, state2);
  EXPECT_NE(state1, state3);
}
