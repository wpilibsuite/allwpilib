// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/MecanumDriveWheelVelocities.hpp"

#include <gtest/gtest.h>

TEST(MecanumDriveWheelVelocitiesTest, Plus) {
  const wpi::math::MecanumDriveWheelVelocities left{1.0_mps, 0.5_mps, 2.0_mps,
                                                    1.5_mps};
  const wpi::math::MecanumDriveWheelVelocities right{2.0_mps, 1.5_mps, 0.5_mps,
                                                     1.0_mps};

  const wpi::math::MecanumDriveWheelVelocities result = left + right;

  EXPECT_EQ(3.0, result.frontLeft.value());
  EXPECT_EQ(2.0, result.frontRight.value());
  EXPECT_EQ(2.5, result.rearLeft.value());
  EXPECT_EQ(2.5, result.rearRight.value());
}

TEST(MecanumDriveWheelVelocitiesTest, Minus) {
  const wpi::math::MecanumDriveWheelVelocities left{1.0_mps, 0.5_mps, 2.0_mps,
                                                    1.5_mps};
  const wpi::math::MecanumDriveWheelVelocities right{2.0_mps, 1.5_mps, 0.5_mps,
                                                     1.0_mps};

  const wpi::math::MecanumDriveWheelVelocities result = left - right;

  EXPECT_EQ(-1.0, result.frontLeft.value());
  EXPECT_EQ(-1.0, result.frontRight.value());
  EXPECT_EQ(1.5, result.rearLeft.value());
  EXPECT_EQ(0.5, result.rearRight.value());
}

TEST(MecanumDriveWheelVelocitiesTest, UnaryMinus) {
  const wpi::math::MecanumDriveWheelVelocities velocities{1.0_mps, 0.5_mps,
                                                          2.0_mps, 1.5_mps};

  const wpi::math::MecanumDriveWheelVelocities result = -velocities;

  EXPECT_EQ(-1.0, result.frontLeft.value());
  EXPECT_EQ(-0.5, result.frontRight.value());
  EXPECT_EQ(-2.0, result.rearLeft.value());
  EXPECT_EQ(-1.5, result.rearRight.value());
}

TEST(MecanumDriveWheelVelocitiesTest, Multiplication) {
  const wpi::math::MecanumDriveWheelVelocities velocities{1.0_mps, 0.5_mps,
                                                          2.0_mps, 1.5_mps};

  const wpi::math::MecanumDriveWheelVelocities result = velocities * 2;

  EXPECT_EQ(2.0, result.frontLeft.value());
  EXPECT_EQ(1.0, result.frontRight.value());
  EXPECT_EQ(4.0, result.rearLeft.value());
  EXPECT_EQ(3.0, result.rearRight.value());
}

TEST(MecanumDriveWheelVelocitiesTest, Division) {
  const wpi::math::MecanumDriveWheelVelocities velocities{1.0_mps, 0.5_mps,
                                                          2.0_mps, 1.5_mps};

  const wpi::math::MecanumDriveWheelVelocities result = velocities / 2;

  EXPECT_EQ(0.5, result.frontLeft.value());
  EXPECT_EQ(0.25, result.frontRight.value());
  EXPECT_EQ(1.0, result.rearLeft.value());
  EXPECT_EQ(0.75, result.rearRight.value());
}
