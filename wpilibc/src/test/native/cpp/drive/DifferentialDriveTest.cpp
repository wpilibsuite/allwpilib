// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/drive/DifferentialDrive.hpp"

#include <gtest/gtest.h>

#include "motorcontrol/MockPWMMotorController.hpp"

TEST(DifferentialDriveTest, ArcadeDriveIK) {
  // Forward
  auto speeds = wpi::DifferentialDrive::ArcadeDriveIK(1.0, 0.0, false);
  EXPECT_DOUBLE_EQ(1.0, speeds.left);
  EXPECT_DOUBLE_EQ(1.0, speeds.right);

  // Forward left turn
  speeds = wpi::DifferentialDrive::ArcadeDriveIK(0.5, 0.5, false);
  EXPECT_DOUBLE_EQ(0.0, speeds.left);
  EXPECT_DOUBLE_EQ(0.5, speeds.right);

  // Forward right turn
  speeds = wpi::DifferentialDrive::ArcadeDriveIK(0.5, -0.5, false);
  EXPECT_DOUBLE_EQ(0.5, speeds.left);
  EXPECT_DOUBLE_EQ(0.0, speeds.right);

  // Backward
  speeds = wpi::DifferentialDrive::ArcadeDriveIK(-1.0, 0.0, false);
  EXPECT_DOUBLE_EQ(-1.0, speeds.left);
  EXPECT_DOUBLE_EQ(-1.0, speeds.right);

  // Backward left turn
  speeds = wpi::DifferentialDrive::ArcadeDriveIK(-0.5, 0.5, false);
  EXPECT_DOUBLE_EQ(-0.5, speeds.left);
  EXPECT_DOUBLE_EQ(0.0, speeds.right);

  // Backward right turn
  speeds = wpi::DifferentialDrive::ArcadeDriveIK(-0.5, -0.5, false);
  EXPECT_DOUBLE_EQ(0.0, speeds.left);
  EXPECT_DOUBLE_EQ(-0.5, speeds.right);

  // Left turn (xSpeed with negative sign)
  speeds = wpi::DifferentialDrive::ArcadeDriveIK(-0.0, 1.0, false);
  EXPECT_DOUBLE_EQ(-1.0, speeds.left);
  EXPECT_DOUBLE_EQ(1.0, speeds.right);

  // Left turn (xSpeed with positive sign)
  speeds = wpi::DifferentialDrive::ArcadeDriveIK(0.0, 1.0, false);
  EXPECT_DOUBLE_EQ(-1.0, speeds.left);
  EXPECT_DOUBLE_EQ(1.0, speeds.right);

  // Right turn (xSpeed with negative sign)
  speeds = wpi::DifferentialDrive::ArcadeDriveIK(-0.0, -1.0, false);
  EXPECT_DOUBLE_EQ(1.0, speeds.left);
  EXPECT_DOUBLE_EQ(-1.0, speeds.right);

  // Right turn (xSpeed with positive sign)
  speeds = wpi::DifferentialDrive::ArcadeDriveIK(0.0, -1.0, false);
  EXPECT_DOUBLE_EQ(1.0, speeds.left);
  EXPECT_DOUBLE_EQ(-1.0, speeds.right);
}

TEST(DifferentialDriveTest, ArcadeDriveIKSquared) {
  // Forward
  auto speeds = wpi::DifferentialDrive::ArcadeDriveIK(1.0, 0.0, true);
  EXPECT_DOUBLE_EQ(1.0, speeds.left);
  EXPECT_DOUBLE_EQ(1.0, speeds.right);

  // Forward left turn
  speeds = wpi::DifferentialDrive::ArcadeDriveIK(0.5, 0.5, true);
  EXPECT_DOUBLE_EQ(0.0, speeds.left);
  EXPECT_DOUBLE_EQ(0.25, speeds.right);

  // Forward right turn
  speeds = wpi::DifferentialDrive::ArcadeDriveIK(0.5, -0.5, true);
  EXPECT_DOUBLE_EQ(0.25, speeds.left);
  EXPECT_DOUBLE_EQ(0.0, speeds.right);

  // Backward
  speeds = wpi::DifferentialDrive::ArcadeDriveIK(-1.0, 0.0, true);
  EXPECT_DOUBLE_EQ(-1.0, speeds.left);
  EXPECT_DOUBLE_EQ(-1.0, speeds.right);

  // Backward left turn
  speeds = wpi::DifferentialDrive::ArcadeDriveIK(-0.5, 0.5, true);
  EXPECT_DOUBLE_EQ(-0.25, speeds.left);
  EXPECT_DOUBLE_EQ(0.0, speeds.right);

  // Backward right turn
  speeds = wpi::DifferentialDrive::ArcadeDriveIK(-0.5, -0.5, true);
  EXPECT_DOUBLE_EQ(0.0, speeds.left);
  EXPECT_DOUBLE_EQ(-0.25, speeds.right);

  // Left turn (xSpeed with negative sign)
  speeds = wpi::DifferentialDrive::ArcadeDriveIK(-0.0, 1.0, false);
  EXPECT_DOUBLE_EQ(-1.0, speeds.left);
  EXPECT_DOUBLE_EQ(1.0, speeds.right);

  // Left turn (xSpeed with positive sign)
  speeds = wpi::DifferentialDrive::ArcadeDriveIK(0.0, 1.0, false);
  EXPECT_DOUBLE_EQ(-1.0, speeds.left);
  EXPECT_DOUBLE_EQ(1.0, speeds.right);

  // Right turn (xSpeed with negative sign)
  speeds = wpi::DifferentialDrive::ArcadeDriveIK(-0.0, -1.0, false);
  EXPECT_DOUBLE_EQ(1.0, speeds.left);
  EXPECT_DOUBLE_EQ(-1.0, speeds.right);

  // Right turn (xSpeed with positive sign)
  speeds = wpi::DifferentialDrive::ArcadeDriveIK(0.0, -1.0, false);
  EXPECT_DOUBLE_EQ(1.0, speeds.left);
  EXPECT_DOUBLE_EQ(-1.0, speeds.right);
}

TEST(DifferentialDriveTest, CurvatureDriveIK) {
  // Forward
  auto speeds = wpi::DifferentialDrive::CurvatureDriveIK(1.0, 0.0, false);
  EXPECT_DOUBLE_EQ(1.0, speeds.left);
  EXPECT_DOUBLE_EQ(1.0, speeds.right);

  // Forward left turn
  speeds = wpi::DifferentialDrive::CurvatureDriveIK(0.5, 0.5, false);
  EXPECT_DOUBLE_EQ(0.25, speeds.left);
  EXPECT_DOUBLE_EQ(0.75, speeds.right);

  // Forward right turn
  speeds = wpi::DifferentialDrive::CurvatureDriveIK(0.5, -0.5, false);
  EXPECT_DOUBLE_EQ(0.75, speeds.left);
  EXPECT_DOUBLE_EQ(0.25, speeds.right);

  // Backward
  speeds = wpi::DifferentialDrive::CurvatureDriveIK(-1.0, 0.0, false);
  EXPECT_DOUBLE_EQ(-1.0, speeds.left);
  EXPECT_DOUBLE_EQ(-1.0, speeds.right);

  // Backward left turn
  speeds = wpi::DifferentialDrive::CurvatureDriveIK(-0.5, 0.5, false);
  EXPECT_DOUBLE_EQ(-0.75, speeds.left);
  EXPECT_DOUBLE_EQ(-0.25, speeds.right);

  // Backward right turn
  speeds = wpi::DifferentialDrive::CurvatureDriveIK(-0.5, -0.5, false);
  EXPECT_DOUBLE_EQ(-0.25, speeds.left);
  EXPECT_DOUBLE_EQ(-0.75, speeds.right);
}

TEST(DifferentialDriveTest, CurvatureDriveIKTurnInPlace) {
  // Forward
  auto speeds = wpi::DifferentialDrive::CurvatureDriveIK(1.0, 0.0, true);
  EXPECT_DOUBLE_EQ(1.0, speeds.left);
  EXPECT_DOUBLE_EQ(1.0, speeds.right);

  // Forward left turn
  speeds = wpi::DifferentialDrive::CurvatureDriveIK(0.5, 0.5, true);
  EXPECT_DOUBLE_EQ(0.0, speeds.left);
  EXPECT_DOUBLE_EQ(1.0, speeds.right);

  // Forward right turn
  speeds = wpi::DifferentialDrive::CurvatureDriveIK(0.5, -0.5, true);
  EXPECT_DOUBLE_EQ(1.0, speeds.left);
  EXPECT_DOUBLE_EQ(0.0, speeds.right);

  // Backward
  speeds = wpi::DifferentialDrive::CurvatureDriveIK(-1.0, 0.0, true);
  EXPECT_DOUBLE_EQ(-1.0, speeds.left);
  EXPECT_DOUBLE_EQ(-1.0, speeds.right);

  // Backward left turn
  speeds = wpi::DifferentialDrive::CurvatureDriveIK(-0.5, 0.5, true);
  EXPECT_DOUBLE_EQ(-1.0, speeds.left);
  EXPECT_DOUBLE_EQ(0.0, speeds.right);

  // Backward right turn
  speeds = wpi::DifferentialDrive::CurvatureDriveIK(-0.5, -0.5, true);
  EXPECT_DOUBLE_EQ(0.0, speeds.left);
  EXPECT_DOUBLE_EQ(-1.0, speeds.right);
}

TEST(DifferentialDriveTest, TankDriveIK) {
  // Forward
  auto speeds = wpi::DifferentialDrive::TankDriveIK(1.0, 1.0, false);
  EXPECT_DOUBLE_EQ(1.0, speeds.left);
  EXPECT_DOUBLE_EQ(1.0, speeds.right);

  // Forward left turn
  speeds = wpi::DifferentialDrive::TankDriveIK(0.5, 1.0, false);
  EXPECT_DOUBLE_EQ(0.5, speeds.left);
  EXPECT_DOUBLE_EQ(1.0, speeds.right);

  // Forward right turn
  speeds = wpi::DifferentialDrive::TankDriveIK(1.0, 0.5, false);
  EXPECT_DOUBLE_EQ(1.0, speeds.left);
  EXPECT_DOUBLE_EQ(0.5, speeds.right);

  // Backward
  speeds = wpi::DifferentialDrive::TankDriveIK(-1.0, -1.0, false);
  EXPECT_DOUBLE_EQ(-1.0, speeds.left);
  EXPECT_DOUBLE_EQ(-1.0, speeds.right);

  // Backward left turn
  speeds = wpi::DifferentialDrive::TankDriveIK(-0.5, -1.0, false);
  EXPECT_DOUBLE_EQ(-0.5, speeds.left);
  EXPECT_DOUBLE_EQ(-1.0, speeds.right);

  // Backward right turn
  speeds = wpi::DifferentialDrive::TankDriveIK(-0.5, 1.0, false);
  EXPECT_DOUBLE_EQ(-0.5, speeds.left);
  EXPECT_DOUBLE_EQ(1.0, speeds.right);
}

TEST(DifferentialDriveTest, TankDriveIKSquared) {
  // Forward
  auto speeds = wpi::DifferentialDrive::TankDriveIK(1.0, 1.0, true);
  EXPECT_DOUBLE_EQ(1.0, speeds.left);
  EXPECT_DOUBLE_EQ(1.0, speeds.right);

  // Forward left turn
  speeds = wpi::DifferentialDrive::TankDriveIK(0.5, 1.0, true);
  EXPECT_DOUBLE_EQ(0.25, speeds.left);
  EXPECT_DOUBLE_EQ(1.0, speeds.right);

  // Forward right turn
  speeds = wpi::DifferentialDrive::TankDriveIK(1.0, 0.5, true);
  EXPECT_DOUBLE_EQ(1.0, speeds.left);
  EXPECT_DOUBLE_EQ(0.25, speeds.right);

  // Backward
  speeds = wpi::DifferentialDrive::TankDriveIK(-1.0, -1.0, true);
  EXPECT_DOUBLE_EQ(-1.0, speeds.left);
  EXPECT_DOUBLE_EQ(-1.0, speeds.right);

  // Backward left turn
  speeds = wpi::DifferentialDrive::TankDriveIK(-0.5, -1.0, true);
  EXPECT_DOUBLE_EQ(-0.25, speeds.left);
  EXPECT_DOUBLE_EQ(-1.0, speeds.right);

  // Backward right turn
  speeds = wpi::DifferentialDrive::TankDriveIK(-1.0, -0.5, true);
  EXPECT_DOUBLE_EQ(-1.0, speeds.left);
  EXPECT_DOUBLE_EQ(-0.25, speeds.right);
}

TEST(DifferentialDriveTest, ArcadeDrive) {
  wpi::MockPWMMotorController left;
  wpi::MockPWMMotorController right;
  wpi::DifferentialDrive drive{[&](double output) { left.Set(output); },
                               [&](double output) { right.Set(output); }};
  drive.SetDeadband(0.0);

  // Forward
  drive.ArcadeDrive(1.0, 0.0, false);
  EXPECT_DOUBLE_EQ(1.0, left.Get());
  EXPECT_DOUBLE_EQ(1.0, right.Get());

  // Forward left turn
  drive.ArcadeDrive(0.5, 0.5, false);
  EXPECT_DOUBLE_EQ(0.0, left.Get());
  EXPECT_DOUBLE_EQ(0.5, right.Get());

  // Forward right turn
  drive.ArcadeDrive(0.5, -0.5, false);
  EXPECT_DOUBLE_EQ(0.5, left.Get());
  EXPECT_DOUBLE_EQ(0.0, right.Get());

  // Backward
  drive.ArcadeDrive(-1.0, 0.0, false);
  EXPECT_DOUBLE_EQ(-1.0, left.Get());
  EXPECT_DOUBLE_EQ(-1.0, right.Get());

  // Backward left turn
  drive.ArcadeDrive(-0.5, 0.5, false);
  EXPECT_DOUBLE_EQ(-0.5, left.Get());
  EXPECT_DOUBLE_EQ(0.0, right.Get());

  // Backward right turn
  drive.ArcadeDrive(-0.5, -0.5, false);
  EXPECT_DOUBLE_EQ(0.0, left.Get());
  EXPECT_DOUBLE_EQ(-0.5, right.Get());
}

TEST(DifferentialDriveTest, ArcadeDriveSquared) {
  wpi::MockPWMMotorController left;
  wpi::MockPWMMotorController right;
  wpi::DifferentialDrive drive{[&](double output) { left.Set(output); },
                               [&](double output) { right.Set(output); }};
  drive.SetDeadband(0.0);

  // Forward
  drive.ArcadeDrive(1.0, 0.0, true);
  EXPECT_DOUBLE_EQ(1.0, left.Get());
  EXPECT_DOUBLE_EQ(1.0, right.Get());

  // Forward left turn
  drive.ArcadeDrive(0.5, 0.5, true);
  EXPECT_DOUBLE_EQ(0.0, left.Get());
  EXPECT_DOUBLE_EQ(0.25, right.Get());

  // Forward right turn
  drive.ArcadeDrive(0.5, -0.5, true);
  EXPECT_DOUBLE_EQ(0.25, left.Get());
  EXPECT_DOUBLE_EQ(0.0, right.Get());

  // Backward
  drive.ArcadeDrive(-1.0, 0.0, true);
  EXPECT_DOUBLE_EQ(-1.0, left.Get());
  EXPECT_DOUBLE_EQ(-1.0, right.Get());

  // Backward left turn
  drive.ArcadeDrive(-0.5, 0.5, true);
  EXPECT_DOUBLE_EQ(-0.25, left.Get());
  EXPECT_DOUBLE_EQ(0.0, right.Get());

  // Backward right turn
  drive.ArcadeDrive(-0.5, -0.5, true);
  EXPECT_DOUBLE_EQ(0.0, left.Get());
  EXPECT_DOUBLE_EQ(-0.25, right.Get());
}

TEST(DifferentialDriveTest, CurvatureDrive) {
  wpi::MockPWMMotorController left;
  wpi::MockPWMMotorController right;
  wpi::DifferentialDrive drive{[&](double output) { left.Set(output); },
                               [&](double output) { right.Set(output); }};
  drive.SetDeadband(0.0);

  // Forward
  drive.CurvatureDrive(1.0, 0.0, false);
  EXPECT_DOUBLE_EQ(1.0, left.Get());
  EXPECT_DOUBLE_EQ(1.0, right.Get());

  // Forward left turn
  drive.CurvatureDrive(0.5, 0.5, false);
  EXPECT_DOUBLE_EQ(0.25, left.Get());
  EXPECT_DOUBLE_EQ(0.75, right.Get());

  // Forward right turn
  drive.CurvatureDrive(0.5, -0.5, false);
  EXPECT_DOUBLE_EQ(0.75, left.Get());
  EXPECT_DOUBLE_EQ(0.25, right.Get());

  // Backward
  drive.CurvatureDrive(-1.0, 0.0, false);
  EXPECT_DOUBLE_EQ(-1.0, left.Get());
  EXPECT_DOUBLE_EQ(-1.0, right.Get());

  // Backward left turn
  drive.CurvatureDrive(-0.5, 0.5, false);
  EXPECT_DOUBLE_EQ(-0.75, left.Get());
  EXPECT_DOUBLE_EQ(-0.25, right.Get());

  // Backward right turn
  drive.CurvatureDrive(-0.5, -0.5, false);
  EXPECT_DOUBLE_EQ(-0.25, left.Get());
  EXPECT_DOUBLE_EQ(-0.75, right.Get());
}

TEST(DifferentialDriveTest, CurvatureDriveTurnInPlace) {
  wpi::MockPWMMotorController left;
  wpi::MockPWMMotorController right;
  wpi::DifferentialDrive drive{[&](double output) { left.Set(output); },
                               [&](double output) { right.Set(output); }};
  drive.SetDeadband(0.0);

  // Forward
  drive.CurvatureDrive(1.0, 0.0, true);
  EXPECT_DOUBLE_EQ(1.0, left.Get());
  EXPECT_DOUBLE_EQ(1.0, right.Get());

  // Forward left turn
  drive.CurvatureDrive(0.5, 0.5, true);
  EXPECT_DOUBLE_EQ(0.0, left.Get());
  EXPECT_DOUBLE_EQ(1.0, right.Get());

  // Forward right turn
  drive.CurvatureDrive(0.5, -0.5, true);
  EXPECT_DOUBLE_EQ(1.0, left.Get());
  EXPECT_DOUBLE_EQ(0.0, right.Get());

  // Backward
  drive.CurvatureDrive(-1.0, 0.0, true);
  EXPECT_DOUBLE_EQ(-1.0, left.Get());
  EXPECT_DOUBLE_EQ(-1.0, right.Get());

  // Backward left turn
  drive.CurvatureDrive(-0.5, 0.5, true);
  EXPECT_DOUBLE_EQ(-1.0, left.Get());
  EXPECT_DOUBLE_EQ(0.0, right.Get());

  // Backward right turn
  drive.CurvatureDrive(-0.5, -0.5, true);
  EXPECT_DOUBLE_EQ(0.0, left.Get());
  EXPECT_DOUBLE_EQ(-1.0, right.Get());
}

TEST(DifferentialDriveTest, TankDrive) {
  wpi::MockPWMMotorController left;
  wpi::MockPWMMotorController right;
  wpi::DifferentialDrive drive{[&](double output) { left.Set(output); },
                               [&](double output) { right.Set(output); }};
  drive.SetDeadband(0.0);

  // Forward
  drive.TankDrive(1.0, 1.0, false);
  EXPECT_DOUBLE_EQ(1.0, left.Get());
  EXPECT_DOUBLE_EQ(1.0, right.Get());

  // Forward left turn
  drive.TankDrive(0.5, 1.0, false);
  EXPECT_DOUBLE_EQ(0.5, left.Get());
  EXPECT_DOUBLE_EQ(1.0, right.Get());

  // Forward right turn
  drive.TankDrive(1.0, 0.5, false);
  EXPECT_DOUBLE_EQ(1.0, left.Get());
  EXPECT_DOUBLE_EQ(0.5, right.Get());

  // Backward
  drive.TankDrive(-1.0, -1.0, false);
  EXPECT_DOUBLE_EQ(-1.0, left.Get());
  EXPECT_DOUBLE_EQ(-1.0, right.Get());

  // Backward left turn
  drive.TankDrive(-0.5, -1.0, false);
  EXPECT_DOUBLE_EQ(-0.5, left.Get());
  EXPECT_DOUBLE_EQ(-1.0, right.Get());

  // Backward right turn
  drive.TankDrive(-0.5, 1.0, false);
  EXPECT_DOUBLE_EQ(-0.5, left.Get());
  EXPECT_DOUBLE_EQ(1.0, right.Get());
}

TEST(DifferentialDriveTest, TankDriveSquared) {
  wpi::MockPWMMotorController left;
  wpi::MockPWMMotorController right;
  wpi::DifferentialDrive drive{[&](double output) { left.Set(output); },
                               [&](double output) { right.Set(output); }};
  drive.SetDeadband(0.0);

  // Forward
  drive.TankDrive(1.0, 1.0, true);
  EXPECT_DOUBLE_EQ(1.0, left.Get());
  EXPECT_DOUBLE_EQ(1.0, right.Get());

  // Forward left turn
  drive.TankDrive(0.5, 1.0, true);
  EXPECT_DOUBLE_EQ(0.25, left.Get());
  EXPECT_DOUBLE_EQ(1.0, right.Get());

  // Forward right turn
  drive.TankDrive(1.0, 0.5, true);
  EXPECT_DOUBLE_EQ(1.0, left.Get());
  EXPECT_DOUBLE_EQ(0.25, right.Get());

  // Backward
  drive.TankDrive(-1.0, -1.0, true);
  EXPECT_DOUBLE_EQ(-1.0, left.Get());
  EXPECT_DOUBLE_EQ(-1.0, right.Get());

  // Backward left turn
  drive.TankDrive(-0.5, -1.0, true);
  EXPECT_DOUBLE_EQ(-0.25, left.Get());
  EXPECT_DOUBLE_EQ(-1.0, right.Get());

  // Backward right turn
  drive.TankDrive(-1.0, -0.5, true);
  EXPECT_DOUBLE_EQ(-1.0, left.Get());
  EXPECT_DOUBLE_EQ(-0.25, right.Get());
}
