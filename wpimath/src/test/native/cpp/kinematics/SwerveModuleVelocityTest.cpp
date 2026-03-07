// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/SwerveModuleVelocity.hpp"

#include <gtest/gtest.h>

#include "wpi/math/geometry/Rotation2d.hpp"

static constexpr double kEpsilon = 1E-9;

TEST(SwerveModuleVelocityTest, Optimize) {
  wpi::math::Rotation2d angleA{45_deg};
  wpi::math::SwerveModuleVelocity refA{-2_mps, 180_deg};
  refA.Optimize(angleA);

  EXPECT_NEAR(refA.velocity.value(), 2.0, kEpsilon);
  EXPECT_NEAR(refA.angle.Degrees().value(), 0.0, kEpsilon);

  wpi::math::Rotation2d angleB{-50_deg};
  wpi::math::SwerveModuleVelocity refB{4.7_mps, 41_deg};
  refB.Optimize(angleB);

  EXPECT_NEAR(refB.velocity.value(), -4.7, kEpsilon);
  EXPECT_NEAR(refB.angle.Degrees().value(), -139.0, kEpsilon);
}

TEST(SwerveModuleVelocityTest, NoOptimize) {
  wpi::math::Rotation2d angleA{0_deg};
  wpi::math::SwerveModuleVelocity refA{2_mps, 89_deg};
  refA.Optimize(angleA);

  EXPECT_NEAR(refA.velocity.value(), 2.0, kEpsilon);
  EXPECT_NEAR(refA.angle.Degrees().value(), 89.0, kEpsilon);

  wpi::math::Rotation2d angleB{0_deg};
  wpi::math::SwerveModuleVelocity refB{-2_mps, -2_deg};
  refB.Optimize(angleB);

  EXPECT_NEAR(refB.velocity.value(), -2.0, kEpsilon);
  EXPECT_NEAR(refB.angle.Degrees().value(), -2.0, kEpsilon);
}

TEST(SwerveModuleVelocityTest, CosineScaling) {
  wpi::math::Rotation2d angleA{0_deg};
  wpi::math::SwerveModuleVelocity refA{2_mps, 45_deg};
  refA.CosineScale(angleA);

  EXPECT_NEAR(refA.velocity.value(), std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(refA.angle.Degrees().value(), 45.0, kEpsilon);

  wpi::math::Rotation2d angleB{45_deg};
  wpi::math::SwerveModuleVelocity refB{2_mps, 45_deg};
  refB.CosineScale(angleB);

  EXPECT_NEAR(refB.velocity.value(), 2.0, kEpsilon);
  EXPECT_NEAR(refB.angle.Degrees().value(), 45.0, kEpsilon);

  wpi::math::Rotation2d angleC{-45_deg};
  wpi::math::SwerveModuleVelocity refC{2_mps, 45_deg};
  refC.CosineScale(angleC);

  EXPECT_NEAR(refC.velocity.value(), 0.0, kEpsilon);
  EXPECT_NEAR(refC.angle.Degrees().value(), 45.0, kEpsilon);

  wpi::math::Rotation2d angleD{135_deg};
  wpi::math::SwerveModuleVelocity refD{2_mps, 45_deg};
  refD.CosineScale(angleD);

  EXPECT_NEAR(refD.velocity.value(), 0.0, kEpsilon);
  EXPECT_NEAR(refD.angle.Degrees().value(), 45.0, kEpsilon);

  wpi::math::Rotation2d angleE{-135_deg};
  wpi::math::SwerveModuleVelocity refE{2_mps, 45_deg};
  refE.CosineScale(angleE);

  EXPECT_NEAR(refE.velocity.value(), -2.0, kEpsilon);
  EXPECT_NEAR(refE.angle.Degrees().value(), 45.0, kEpsilon);

  wpi::math::Rotation2d angleF{180_deg};
  wpi::math::SwerveModuleVelocity refF{2_mps, 45_deg};
  refF.CosineScale(angleF);

  EXPECT_NEAR(refF.velocity.value(), -std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(refF.angle.Degrees().value(), 45.0, kEpsilon);

  wpi::math::Rotation2d angleG{0_deg};
  wpi::math::SwerveModuleVelocity refG{-2_mps, 45_deg};
  refG.CosineScale(angleG);

  EXPECT_NEAR(refG.velocity.value(), -std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(refG.angle.Degrees().value(), 45.0, kEpsilon);

  wpi::math::Rotation2d angleH{45_deg};
  wpi::math::SwerveModuleVelocity refH{-2_mps, 45_deg};
  refH.CosineScale(angleH);

  EXPECT_NEAR(refH.velocity.value(), -2.0, kEpsilon);
  EXPECT_NEAR(refH.angle.Degrees().value(), 45.0, kEpsilon);

  wpi::math::Rotation2d angleI{-45_deg};
  wpi::math::SwerveModuleVelocity refI{-2_mps, 45_deg};
  refI.CosineScale(angleI);

  EXPECT_NEAR(refI.velocity.value(), 0.0, kEpsilon);
  EXPECT_NEAR(refI.angle.Degrees().value(), 45.0, kEpsilon);

  wpi::math::Rotation2d angleJ{135_deg};
  wpi::math::SwerveModuleVelocity refJ{-2_mps, 45_deg};
  refJ.CosineScale(angleJ);

  EXPECT_NEAR(refJ.velocity.value(), 0.0, kEpsilon);
  EXPECT_NEAR(refJ.angle.Degrees().value(), 45.0, kEpsilon);

  wpi::math::Rotation2d angleK{-135_deg};
  wpi::math::SwerveModuleVelocity refK{-2_mps, 45_deg};
  refK.CosineScale(angleK);

  EXPECT_NEAR(refK.velocity.value(), 2.0, kEpsilon);
  EXPECT_NEAR(refK.angle.Degrees().value(), 45.0, kEpsilon);

  wpi::math::Rotation2d angleL{180_deg};
  wpi::math::SwerveModuleVelocity refL{-2_mps, 45_deg};
  refL.CosineScale(angleL);

  EXPECT_NEAR(refL.velocity.value(), std::sqrt(2.0), kEpsilon);
  EXPECT_NEAR(refL.angle.Degrees().value(), 45.0, kEpsilon);
}

TEST(SwerveModuleVelocityTest, Equality) {
  wpi::math::SwerveModuleVelocity state1{2_mps, 90_deg};
  wpi::math::SwerveModuleVelocity state2{2_mps, 90_deg};

  EXPECT_EQ(state1, state2);
}

TEST(SwerveModuleVelocityTest, Inequality) {
  wpi::math::SwerveModuleVelocity state1{1_mps, 90_deg};
  wpi::math::SwerveModuleVelocity state2{2_mps, 90_deg};
  wpi::math::SwerveModuleVelocity state3{1_mps, 89_deg};

  EXPECT_NE(state1, state2);
  EXPECT_NE(state1, state3);
}
