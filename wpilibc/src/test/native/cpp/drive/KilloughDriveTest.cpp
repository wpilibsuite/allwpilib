// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include "MockMotorController.h"
#include "frc/drive/KilloughDrive.h"
#include "gtest/gtest.h"

TEST(KilloughDriveTest, CartesianIK) {
  frc::MockMotorController left;
  frc::MockMotorController right;
  frc::MockMotorController back;
  frc::KilloughDrive drive{left, right, back};

  // Forward
  auto speeds = drive.DriveCartesianIK(1.0, 0.0, 0.0);
  EXPECT_DOUBLE_EQ(0.5, speeds.left);
  EXPECT_DOUBLE_EQ(-0.5, speeds.right);
  EXPECT_NEAR(0.0, speeds.back, 1e-9);

  // Left
  speeds = drive.DriveCartesianIK(0.0, -1.0, 0.0);
  EXPECT_DOUBLE_EQ(-std::sqrt(3) / 2, speeds.left);
  EXPECT_DOUBLE_EQ(-std::sqrt(3) / 2, speeds.right);
  EXPECT_DOUBLE_EQ(1.0, speeds.back);

  // Right
  speeds = drive.DriveCartesianIK(0.0, 1.0, 0.0);
  EXPECT_DOUBLE_EQ(std::sqrt(3) / 2, speeds.left);
  EXPECT_DOUBLE_EQ(std::sqrt(3) / 2, speeds.right);
  EXPECT_DOUBLE_EQ(-1.0, speeds.back);

  // Rotate CCW
  speeds = drive.DriveCartesianIK(0.0, 0.0, -1.0);
  EXPECT_DOUBLE_EQ(-1.0, speeds.left);
  EXPECT_DOUBLE_EQ(-1.0, speeds.right);
  EXPECT_DOUBLE_EQ(-1.0, speeds.back);

  // Rotate CW
  speeds = drive.DriveCartesianIK(0.0, 0.0, 1.0);
  EXPECT_DOUBLE_EQ(1.0, speeds.left);
  EXPECT_DOUBLE_EQ(1.0, speeds.right);
  EXPECT_DOUBLE_EQ(1.0, speeds.back);
}

TEST(KilloughDriveTest, CartesianIKGyro90CW) {
  frc::MockMotorController left;
  frc::MockMotorController right;
  frc::MockMotorController back;
  frc::KilloughDrive drive{left, right, back};

  // Forward in global frame; left in robot frame
  auto speeds = drive.DriveCartesianIK(1.0, 0.0, 0.0, 90.0);
  EXPECT_DOUBLE_EQ(-std::sqrt(3) / 2, speeds.left);
  EXPECT_DOUBLE_EQ(-std::sqrt(3) / 2, speeds.right);
  EXPECT_DOUBLE_EQ(1.0, speeds.back);

  // Left in global frame; backward in robot frame
  speeds = drive.DriveCartesianIK(0.0, -1.0, 0.0, 90.0);
  EXPECT_DOUBLE_EQ(-0.5, speeds.left);
  EXPECT_NEAR(0.5, speeds.right, 1e-9);
  EXPECT_NEAR(0.0, speeds.back, 1e-9);

  // Right in global frame; forward in robot frame
  speeds = drive.DriveCartesianIK(0.0, 1.0, 0.0, 90.0);
  EXPECT_DOUBLE_EQ(0.5, speeds.left);
  EXPECT_NEAR(-0.5, speeds.right, 1e-9);
  EXPECT_NEAR(0.0, speeds.back, 1e-9);

  // Rotate CCW
  speeds = drive.DriveCartesianIK(0.0, 0.0, -1.0, 90.0);
  EXPECT_DOUBLE_EQ(-1.0, speeds.left);
  EXPECT_DOUBLE_EQ(-1.0, speeds.right);
  EXPECT_DOUBLE_EQ(-1.0, speeds.back);

  // Rotate CW
  speeds = drive.DriveCartesianIK(0.0, 0.0, 1.0, 90.0);
  EXPECT_DOUBLE_EQ(1.0, speeds.left);
  EXPECT_DOUBLE_EQ(1.0, speeds.right);
  EXPECT_DOUBLE_EQ(1.0, speeds.back);
}

TEST(KilloughDriveTest, Cartesian) {
  frc::MockMotorController left;
  frc::MockMotorController right;
  frc::MockMotorController back;
  frc::KilloughDrive drive{left, right, back};
  drive.SetDeadband(0.0);

  // Forward
  drive.DriveCartesian(1.0, 0.0, 0.0);
  EXPECT_DOUBLE_EQ(0.5, left.Get());
  EXPECT_DOUBLE_EQ(-0.5, right.Get());
  EXPECT_NEAR(0.0, back.Get(), 1e-9);

  // Left
  drive.DriveCartesian(0.0, -1.0, 0.0);
  EXPECT_DOUBLE_EQ(-std::sqrt(3) / 2, left.Get());
  EXPECT_DOUBLE_EQ(-std::sqrt(3) / 2, right.Get());
  EXPECT_DOUBLE_EQ(1.0, back.Get());

  // Right
  drive.DriveCartesian(0.0, 1.0, 0.0);
  EXPECT_DOUBLE_EQ(std::sqrt(3) / 2, left.Get());
  EXPECT_DOUBLE_EQ(std::sqrt(3) / 2, right.Get());
  EXPECT_DOUBLE_EQ(-1.0, back.Get());

  // Rotate CCW
  drive.DriveCartesian(0.0, 0.0, -1.0);
  EXPECT_DOUBLE_EQ(-1.0, left.Get());
  EXPECT_DOUBLE_EQ(-1.0, right.Get());
  EXPECT_DOUBLE_EQ(-1.0, back.Get());

  // Rotate CW
  drive.DriveCartesian(0.0, 0.0, 1.0);
  EXPECT_DOUBLE_EQ(1.0, left.Get());
  EXPECT_DOUBLE_EQ(1.0, right.Get());
  EXPECT_DOUBLE_EQ(1.0, back.Get());
}

TEST(KilloughDriveTest, CartesianGyro90CW) {
  frc::MockMotorController left;
  frc::MockMotorController right;
  frc::MockMotorController back;
  frc::KilloughDrive drive{left, right, back};
  drive.SetDeadband(0.0);

  // Forward in global frame; left in robot frame
  drive.DriveCartesian(1.0, 0.0, 0.0, 90.0);
  EXPECT_DOUBLE_EQ(-std::sqrt(3) / 2, left.Get());
  EXPECT_DOUBLE_EQ(-std::sqrt(3) / 2, right.Get());
  EXPECT_DOUBLE_EQ(1.0, back.Get());

  // Left in global frame; backward in robot frame
  drive.DriveCartesian(0.0, -1.0, 0.0, 90.0);
  EXPECT_DOUBLE_EQ(-0.5, left.Get());
  EXPECT_NEAR(0.5, right.Get(), 1e-9);
  EXPECT_NEAR(0.0, back.Get(), 1e-9);

  // Right in global frame; forward in robot frame
  drive.DriveCartesian(0.0, 1.0, 0.0, 90.0);
  EXPECT_DOUBLE_EQ(0.5, left.Get());
  EXPECT_NEAR(-0.5, right.Get(), 1e-9);
  EXPECT_NEAR(0.0, back.Get(), 1e-9);

  // Rotate CCW
  drive.DriveCartesian(0.0, 0.0, -1.0, 90.0);
  EXPECT_DOUBLE_EQ(-1.0, left.Get());
  EXPECT_DOUBLE_EQ(-1.0, right.Get());
  EXPECT_DOUBLE_EQ(-1.0, back.Get());

  // Rotate CW
  drive.DriveCartesian(0.0, 0.0, 1.0, 90.0);
  EXPECT_DOUBLE_EQ(1.0, left.Get());
  EXPECT_DOUBLE_EQ(1.0, right.Get());
  EXPECT_DOUBLE_EQ(1.0, back.Get());
}

TEST(KilloughDriveTest, Polar) {
  frc::MockMotorController left;
  frc::MockMotorController right;
  frc::MockMotorController back;
  frc::KilloughDrive drive{left, right, back};
  drive.SetDeadband(0.0);

  // Forward
  drive.DrivePolar(1.0, 0.0, 0.0);
  EXPECT_DOUBLE_EQ(std::sqrt(3) / 2, left.Get());
  EXPECT_DOUBLE_EQ(std::sqrt(3) / 2, right.Get());
  EXPECT_DOUBLE_EQ(-1.0, back.Get());

  // Left
  drive.DrivePolar(1.0, -90.0, 0.0);
  EXPECT_DOUBLE_EQ(-0.5, left.Get());
  EXPECT_DOUBLE_EQ(0.5, right.Get());
  EXPECT_NEAR(0.0, back.Get(), 1e-9);

  // Right
  drive.DrivePolar(1.0, 90.0, 0.0);
  EXPECT_DOUBLE_EQ(0.5, left.Get());
  EXPECT_NEAR(-0.5, right.Get(), 1e-9);
  EXPECT_NEAR(0.0, back.Get(), 1e-9);

  // Rotate CCW
  drive.DrivePolar(0.0, 0.0, -1.0);
  EXPECT_DOUBLE_EQ(-1.0, left.Get());
  EXPECT_DOUBLE_EQ(-1.0, right.Get());
  EXPECT_DOUBLE_EQ(-1.0, back.Get());

  // Rotate CW
  drive.DrivePolar(0.0, 0.0, 1.0);
  EXPECT_DOUBLE_EQ(1.0, left.Get());
  EXPECT_DOUBLE_EQ(1.0, right.Get());
  EXPECT_DOUBLE_EQ(1.0, back.Get());
}
