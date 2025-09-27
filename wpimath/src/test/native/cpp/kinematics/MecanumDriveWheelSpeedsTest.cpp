// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/math/kinematics/MecanumDriveWheelSpeeds.hpp"

TEST(MecanumDriveWheelSpeedsTest, Plus) {
  const frc::MecanumDriveWheelSpeeds left{1.0_mps, 0.5_mps, 2.0_mps, 1.5_mps};
  const frc::MecanumDriveWheelSpeeds right{2.0_mps, 1.5_mps, 0.5_mps, 1.0_mps};

  const frc::MecanumDriveWheelSpeeds result = left + right;

  EXPECT_EQ(3.0, result.frontLeft.value());
  EXPECT_EQ(2.0, result.frontRight.value());
  EXPECT_EQ(2.5, result.rearLeft.value());
  EXPECT_EQ(2.5, result.rearRight.value());
}

TEST(MecanumDriveWheelSpeedsTest, Minus) {
  const frc::MecanumDriveWheelSpeeds left{1.0_mps, 0.5_mps, 2.0_mps, 1.5_mps};
  const frc::MecanumDriveWheelSpeeds right{2.0_mps, 1.5_mps, 0.5_mps, 1.0_mps};

  const frc::MecanumDriveWheelSpeeds result = left - right;

  EXPECT_EQ(-1.0, result.frontLeft.value());
  EXPECT_EQ(-1.0, result.frontRight.value());
  EXPECT_EQ(1.5, result.rearLeft.value());
  EXPECT_EQ(0.5, result.rearRight.value());
}

TEST(MecanumDriveWheelSpeedsTest, UnaryMinus) {
  const frc::MecanumDriveWheelSpeeds speeds{1.0_mps, 0.5_mps, 2.0_mps, 1.5_mps};

  const frc::MecanumDriveWheelSpeeds result = -speeds;

  EXPECT_EQ(-1.0, result.frontLeft.value());
  EXPECT_EQ(-0.5, result.frontRight.value());
  EXPECT_EQ(-2.0, result.rearLeft.value());
  EXPECT_EQ(-1.5, result.rearRight.value());
}

TEST(MecanumDriveWheelSpeedsTest, Multiplication) {
  const frc::MecanumDriveWheelSpeeds speeds{1.0_mps, 0.5_mps, 2.0_mps, 1.5_mps};

  const frc::MecanumDriveWheelSpeeds result = speeds * 2;

  EXPECT_EQ(2.0, result.frontLeft.value());
  EXPECT_EQ(1.0, result.frontRight.value());
  EXPECT_EQ(4.0, result.rearLeft.value());
  EXPECT_EQ(3.0, result.rearRight.value());
}

TEST(MecanumDriveWheelSpeedsTest, Division) {
  const frc::MecanumDriveWheelSpeeds speeds{1.0_mps, 0.5_mps, 2.0_mps, 1.5_mps};

  const frc::MecanumDriveWheelSpeeds result = speeds / 2;

  EXPECT_EQ(0.5, result.frontLeft.value());
  EXPECT_EQ(0.25, result.frontRight.value());
  EXPECT_EQ(1.0, result.rearLeft.value());
  EXPECT_EQ(0.75, result.rearRight.value());
}
