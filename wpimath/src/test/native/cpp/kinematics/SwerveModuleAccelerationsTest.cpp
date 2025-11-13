// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/SwerveModuleAccelerations.hpp"

#include <numbers>

#include <gtest/gtest.h>

#include "wpi/units/acceleration.hpp"

using namespace wpi::math;

static constexpr double kEpsilon = 1E-9;

TEST(SwerveModuleAccelerationsTest, DefaultConstructor) {
  SwerveModuleAccelerations moduleAccelerations;

  EXPECT_NEAR(moduleAccelerations.acceleration.value(), 0.0, kEpsilon);
  EXPECT_NEAR(moduleAccelerations.angle.Radians().value(), 0.0, kEpsilon);
}

TEST(SwerveModuleAccelerationsTest, ParameterizedConstructor) {
  SwerveModuleAccelerations moduleAccelerations{2.5_mps_sq,
                                                Rotation2d{1.5_rad}};

  EXPECT_NEAR(moduleAccelerations.acceleration.value(), 2.5, kEpsilon);
  EXPECT_NEAR(moduleAccelerations.angle.Radians().value(), 1.5, kEpsilon);
}

TEST(SwerveModuleAccelerationsTest, Equals) {
  SwerveModuleAccelerations moduleAccelerations1{2.0_mps_sq,
                                                 Rotation2d{1.5_rad}};
  SwerveModuleAccelerations moduleAccelerations2{2.0_mps_sq,
                                                 Rotation2d{1.5_rad}};
  SwerveModuleAccelerations moduleAccelerations3{2.1_mps_sq,
                                                 Rotation2d{1.5_rad}};

  EXPECT_EQ(moduleAccelerations1, moduleAccelerations2);
  EXPECT_NE(moduleAccelerations1, moduleAccelerations3);
}
