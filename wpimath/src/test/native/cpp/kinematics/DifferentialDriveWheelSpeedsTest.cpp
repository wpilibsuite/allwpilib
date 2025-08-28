// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/math/kinematics/DifferentialDriveWheelSpeeds.h"

TEST(DifferentialDriveWheelSpeedsTest, Plus) {
  const wpi::math::DifferentialDriveWheelSpeeds left{1.0_mps, 0.5_mps};
  const wpi::math::DifferentialDriveWheelSpeeds right{2.0_mps, 1.5_mps};

  const wpi::math::DifferentialDriveWheelSpeeds result = left + right;

  EXPECT_EQ(3.0, result.left.value());
  EXPECT_EQ(2.0, result.right.value());
}

TEST(DifferentialDriveWheelSpeedsTest, Minus) {
  const wpi::math::DifferentialDriveWheelSpeeds left{1.0_mps, 0.5_mps};
  const wpi::math::DifferentialDriveWheelSpeeds right{2.0_mps, 0.5_mps};

  const wpi::math::DifferentialDriveWheelSpeeds result = left - right;

  EXPECT_EQ(-1.0, result.left.value());
  EXPECT_EQ(0, result.right.value());
}

TEST(DifferentialDriveWheelSpeedsTest, UnaryMinus) {
  const wpi::math::DifferentialDriveWheelSpeeds speeds{1.0_mps, 0.5_mps};

  const wpi::math::DifferentialDriveWheelSpeeds result = -speeds;

  EXPECT_EQ(-1.0, result.left.value());
  EXPECT_EQ(-0.5, result.right.value());
}

TEST(DifferentialDriveWheelSpeedsTest, Multiplication) {
  const wpi::math::DifferentialDriveWheelSpeeds speeds{1.0_mps, 0.5_mps};

  const wpi::math::DifferentialDriveWheelSpeeds result = speeds * 2;

  EXPECT_EQ(2.0, result.left.value());
  EXPECT_EQ(1.0, result.right.value());
}

TEST(DifferentialDriveWheelSpeedsTest, Division) {
  const wpi::math::DifferentialDriveWheelSpeeds speeds{1.0_mps, 0.5_mps};

  const wpi::math::DifferentialDriveWheelSpeeds result = speeds / 2;

  EXPECT_EQ(0.5, result.left.value());
  EXPECT_EQ(0.25, result.right.value());
}
