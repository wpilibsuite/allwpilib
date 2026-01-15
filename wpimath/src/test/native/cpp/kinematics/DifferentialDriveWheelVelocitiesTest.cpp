// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/DifferentialDriveWheelVelocities.hpp"

#include <gtest/gtest.h>

TEST(DifferentialDriveWheelVelocitiesTest, Plus) {
  const wpi::math::DifferentialDriveWheelVelocities left{1.0_mps, 0.5_mps};
  const wpi::math::DifferentialDriveWheelVelocities right{2.0_mps, 1.5_mps};

  const wpi::math::DifferentialDriveWheelVelocities result = left + right;

  EXPECT_EQ(3.0, result.left.value());
  EXPECT_EQ(2.0, result.right.value());
}

TEST(DifferentialDriveWheelVelocitiesTest, Minus) {
  const wpi::math::DifferentialDriveWheelVelocities left{1.0_mps, 0.5_mps};
  const wpi::math::DifferentialDriveWheelVelocities right{2.0_mps, 0.5_mps};

  const wpi::math::DifferentialDriveWheelVelocities result = left - right;

  EXPECT_EQ(-1.0, result.left.value());
  EXPECT_EQ(0, result.right.value());
}

TEST(DifferentialDriveWheelVelocitiesTest, UnaryMinus) {
  const wpi::math::DifferentialDriveWheelVelocities velocities{1.0_mps,
                                                               0.5_mps};

  const wpi::math::DifferentialDriveWheelVelocities result = -velocities;

  EXPECT_EQ(-1.0, result.left.value());
  EXPECT_EQ(-0.5, result.right.value());
}

TEST(DifferentialDriveWheelVelocitiesTest, Multiplication) {
  const wpi::math::DifferentialDriveWheelVelocities velocities{1.0_mps,
                                                               0.5_mps};

  const wpi::math::DifferentialDriveWheelVelocities result = velocities * 2;

  EXPECT_EQ(2.0, result.left.value());
  EXPECT_EQ(1.0, result.right.value());
}

TEST(DifferentialDriveWheelVelocitiesTest, Division) {
  const wpi::math::DifferentialDriveWheelVelocities velocities{1.0_mps,
                                                               0.5_mps};

  const wpi::math::DifferentialDriveWheelVelocities result = velocities / 2;

  EXPECT_EQ(0.5, result.left.value());
  EXPECT_EQ(0.25, result.right.value());
}
