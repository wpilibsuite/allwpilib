// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/DifferentialDriveWheelAccelerations.hpp"

#include <gtest/gtest.h>

#include "wpi/units/acceleration.hpp"

using namespace wpi::math;

static constexpr double kEpsilon = 1E-9;

TEST(DifferentialDriveWheelAccelerationsTest, DefaultConstructor) {
  DifferentialDriveWheelAccelerations wheelAccelerations;

  EXPECT_NEAR(wheelAccelerations.left.value(), 0.0, kEpsilon);
  EXPECT_NEAR(wheelAccelerations.right.value(), 0.0, kEpsilon);
}

TEST(DifferentialDriveWheelAccelerationsTest, ParameterizedConstructor) {
  DifferentialDriveWheelAccelerations wheelAccelerations{1.5_mps_sq,
                                                         2.5_mps_sq};

  EXPECT_NEAR(wheelAccelerations.left.value(), 1.5, kEpsilon);
  EXPECT_NEAR(wheelAccelerations.right.value(), 2.5, kEpsilon);
}

TEST(DifferentialDriveWheelAccelerationsTest, Plus) {
  const DifferentialDriveWheelAccelerations left{1.0_mps_sq, 0.5_mps_sq};
  const DifferentialDriveWheelAccelerations right{2.0_mps_sq, 1.5_mps_sq};

  const auto wheelAccelerations = left + right;

  EXPECT_NEAR(wheelAccelerations.left.value(), 3.0, kEpsilon);
  EXPECT_NEAR(wheelAccelerations.right.value(), 2.0, kEpsilon);
}

TEST(DifferentialDriveWheelAccelerationsTest, Minus) {
  const DifferentialDriveWheelAccelerations left{1.0_mps_sq, 0.5_mps_sq};
  const DifferentialDriveWheelAccelerations right{2.0_mps_sq, 0.5_mps_sq};

  const auto wheelAccelerations = left - right;

  EXPECT_NEAR(wheelAccelerations.left.value(), -1.0, kEpsilon);
  EXPECT_NEAR(wheelAccelerations.right.value(), 0.0, kEpsilon);
}

TEST(DifferentialDriveWheelAccelerationsTest, UnaryMinus) {
  const auto wheelAccelerations =
      -DifferentialDriveWheelAccelerations{1.0_mps_sq, 0.5_mps_sq};

  EXPECT_NEAR(wheelAccelerations.left.value(), -1.0, kEpsilon);
  EXPECT_NEAR(wheelAccelerations.right.value(), -0.5, kEpsilon);
}

TEST(DifferentialDriveWheelAccelerationsTest, Multiplication) {
  const auto wheelAccelerations =
      DifferentialDriveWheelAccelerations{1.0_mps_sq, 0.5_mps_sq} * 2.0;

  EXPECT_NEAR(wheelAccelerations.left.value(), 2.0, kEpsilon);
  EXPECT_NEAR(wheelAccelerations.right.value(), 1.0, kEpsilon);
}

TEST(DifferentialDriveWheelAccelerationsTest, Division) {
  const auto wheelAccelerations =
      DifferentialDriveWheelAccelerations{1.0_mps_sq, 0.5_mps_sq} / 2.0;

  EXPECT_NEAR(wheelAccelerations.left.value(), 0.5, kEpsilon);
  EXPECT_NEAR(wheelAccelerations.right.value(), 0.25, kEpsilon);
}
