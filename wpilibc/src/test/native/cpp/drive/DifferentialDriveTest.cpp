// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/drive/DifferentialDrive.hpp"

#include <gtest/gtest.h>

#include "motorcontrol/MockPWMMotorController.hpp"

TEST(DifferentialDriveTest, ArcadeDriveIK) {
  // Forward
  auto velocities = wpi::DifferentialDrive::ArcadeDriveIK(1.0, 0.0, false);
  EXPECT_DOUBLE_EQ(1.0, velocities.left);
  EXPECT_DOUBLE_EQ(1.0, velocities.right);

  // Forward left turn
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(0.5, 0.5, false);
  EXPECT_DOUBLE_EQ(0.0, velocities.left);
  EXPECT_DOUBLE_EQ(0.5, velocities.right);

  // Forward right turn
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(0.5, -0.5, false);
  EXPECT_DOUBLE_EQ(0.5, velocities.left);
  EXPECT_DOUBLE_EQ(0.0, velocities.right);

  // Backward
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-1.0, 0.0, false);
  EXPECT_DOUBLE_EQ(-1.0, velocities.left);
  EXPECT_DOUBLE_EQ(-1.0, velocities.right);

  // Backward left turn
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-0.5, 0.5, false);
  EXPECT_DOUBLE_EQ(-0.5, velocities.left);
  EXPECT_DOUBLE_EQ(0.0, velocities.right);

  // Backward right turn
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-0.5, -0.5, false);
  EXPECT_DOUBLE_EQ(0.0, velocities.left);
  EXPECT_DOUBLE_EQ(-0.5, velocities.right);

  // Left turn (xVelocity with negative sign)
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-0.0, 1.0, false);
  EXPECT_DOUBLE_EQ(-1.0, velocities.left);
  EXPECT_DOUBLE_EQ(1.0, velocities.right);

  // Left turn (xVelocity with positive sign)
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(0.0, 1.0, false);
  EXPECT_DOUBLE_EQ(-1.0, velocities.left);
  EXPECT_DOUBLE_EQ(1.0, velocities.right);

  // Right turn (xVelocity with negative sign)
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-0.0, -1.0, false);
  EXPECT_DOUBLE_EQ(1.0, velocities.left);
  EXPECT_DOUBLE_EQ(-1.0, velocities.right);

  // Right turn (xVelocity with positive sign)
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(0.0, -1.0, false);
  EXPECT_DOUBLE_EQ(1.0, velocities.left);
  EXPECT_DOUBLE_EQ(-1.0, velocities.right);
}

TEST(DifferentialDriveTest, ArcadeDriveIKSquared) {
  // Forward
  auto velocities = wpi::DifferentialDrive::ArcadeDriveIK(1.0, 0.0, true);
  EXPECT_DOUBLE_EQ(1.0, velocities.left);
  EXPECT_DOUBLE_EQ(1.0, velocities.right);

  // Forward left turn
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(0.5, 0.5, true);
  EXPECT_DOUBLE_EQ(0.0, velocities.left);
  EXPECT_DOUBLE_EQ(0.25, velocities.right);

  // Forward right turn
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(0.5, -0.5, true);
  EXPECT_DOUBLE_EQ(0.25, velocities.left);
  EXPECT_DOUBLE_EQ(0.0, velocities.right);

  // Backward
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-1.0, 0.0, true);
  EXPECT_DOUBLE_EQ(-1.0, velocities.left);
  EXPECT_DOUBLE_EQ(-1.0, velocities.right);

  // Backward left turn
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-0.5, 0.5, true);
  EXPECT_DOUBLE_EQ(-0.25, velocities.left);
  EXPECT_DOUBLE_EQ(0.0, velocities.right);

  // Backward right turn
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-0.5, -0.5, true);
  EXPECT_DOUBLE_EQ(0.0, velocities.left);
  EXPECT_DOUBLE_EQ(-0.25, velocities.right);

  // Left turn (xVelocity with negative sign)
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-0.0, 1.0, false);
  EXPECT_DOUBLE_EQ(-1.0, velocities.left);
  EXPECT_DOUBLE_EQ(1.0, velocities.right);

  // Left turn (xVelocity with positive sign)
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(0.0, 1.0, false);
  EXPECT_DOUBLE_EQ(-1.0, velocities.left);
  EXPECT_DOUBLE_EQ(1.0, velocities.right);

  // Right turn (xVelocity with negative sign)
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-0.0, -1.0, false);
  EXPECT_DOUBLE_EQ(1.0, velocities.left);
  EXPECT_DOUBLE_EQ(-1.0, velocities.right);

  // Right turn (xVelocity with positive sign)
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(0.0, -1.0, false);
  EXPECT_DOUBLE_EQ(1.0, velocities.left);
  EXPECT_DOUBLE_EQ(-1.0, velocities.right);
}

TEST(DifferentialDriveTest, CurvatureDriveIK) {
  // Forward
  auto velocities = wpi::DifferentialDrive::CurvatureDriveIK(1.0, 0.0, false);
  EXPECT_DOUBLE_EQ(1.0, velocities.left);
  EXPECT_DOUBLE_EQ(1.0, velocities.right);

  // Forward left turn
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(0.5, 0.5, false);
  EXPECT_DOUBLE_EQ(0.25, velocities.left);
  EXPECT_DOUBLE_EQ(0.75, velocities.right);

  // Forward right turn
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(0.5, -0.5, false);
  EXPECT_DOUBLE_EQ(0.75, velocities.left);
  EXPECT_DOUBLE_EQ(0.25, velocities.right);

  // Backward
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(-1.0, 0.0, false);
  EXPECT_DOUBLE_EQ(-1.0, velocities.left);
  EXPECT_DOUBLE_EQ(-1.0, velocities.right);

  // Backward left turn
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(-0.5, 0.5, false);
  EXPECT_DOUBLE_EQ(-0.75, velocities.left);
  EXPECT_DOUBLE_EQ(-0.25, velocities.right);

  // Backward right turn
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(-0.5, -0.5, false);
  EXPECT_DOUBLE_EQ(-0.25, velocities.left);
  EXPECT_DOUBLE_EQ(-0.75, velocities.right);
}

TEST(DifferentialDriveTest, CurvatureDriveIKTurnInPlace) {
  // Forward
  auto velocities = wpi::DifferentialDrive::CurvatureDriveIK(1.0, 0.0, true);
  EXPECT_DOUBLE_EQ(1.0, velocities.left);
  EXPECT_DOUBLE_EQ(1.0, velocities.right);

  // Forward left turn
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(0.5, 0.5, true);
  EXPECT_DOUBLE_EQ(0.0, velocities.left);
  EXPECT_DOUBLE_EQ(1.0, velocities.right);

  // Forward right turn
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(0.5, -0.5, true);
  EXPECT_DOUBLE_EQ(1.0, velocities.left);
  EXPECT_DOUBLE_EQ(0.0, velocities.right);

  // Backward
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(-1.0, 0.0, true);
  EXPECT_DOUBLE_EQ(-1.0, velocities.left);
  EXPECT_DOUBLE_EQ(-1.0, velocities.right);

  // Backward left turn
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(-0.5, 0.5, true);
  EXPECT_DOUBLE_EQ(-1.0, velocities.left);
  EXPECT_DOUBLE_EQ(0.0, velocities.right);

  // Backward right turn
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(-0.5, -0.5, true);
  EXPECT_DOUBLE_EQ(0.0, velocities.left);
  EXPECT_DOUBLE_EQ(-1.0, velocities.right);
}

TEST(DifferentialDriveTest, TankDriveIK) {
  // Forward
  auto velocities = wpi::DifferentialDrive::TankDriveIK(1.0, 1.0, false);
  EXPECT_DOUBLE_EQ(1.0, velocities.left);
  EXPECT_DOUBLE_EQ(1.0, velocities.right);

  // Forward left turn
  velocities = wpi::DifferentialDrive::TankDriveIK(0.5, 1.0, false);
  EXPECT_DOUBLE_EQ(0.5, velocities.left);
  EXPECT_DOUBLE_EQ(1.0, velocities.right);

  // Forward right turn
  velocities = wpi::DifferentialDrive::TankDriveIK(1.0, 0.5, false);
  EXPECT_DOUBLE_EQ(1.0, velocities.left);
  EXPECT_DOUBLE_EQ(0.5, velocities.right);

  // Backward
  velocities = wpi::DifferentialDrive::TankDriveIK(-1.0, -1.0, false);
  EXPECT_DOUBLE_EQ(-1.0, velocities.left);
  EXPECT_DOUBLE_EQ(-1.0, velocities.right);

  // Backward left turn
  velocities = wpi::DifferentialDrive::TankDriveIK(-0.5, -1.0, false);
  EXPECT_DOUBLE_EQ(-0.5, velocities.left);
  EXPECT_DOUBLE_EQ(-1.0, velocities.right);

  // Backward right turn
  velocities = wpi::DifferentialDrive::TankDriveIK(-0.5, 1.0, false);
  EXPECT_DOUBLE_EQ(-0.5, velocities.left);
  EXPECT_DOUBLE_EQ(1.0, velocities.right);
}

TEST(DifferentialDriveTest, TankDriveIKSquared) {
  // Forward
  auto velocities = wpi::DifferentialDrive::TankDriveIK(1.0, 1.0, true);
  EXPECT_DOUBLE_EQ(1.0, velocities.left);
  EXPECT_DOUBLE_EQ(1.0, velocities.right);

  // Forward left turn
  velocities = wpi::DifferentialDrive::TankDriveIK(0.5, 1.0, true);
  EXPECT_DOUBLE_EQ(0.25, velocities.left);
  EXPECT_DOUBLE_EQ(1.0, velocities.right);

  // Forward right turn
  velocities = wpi::DifferentialDrive::TankDriveIK(1.0, 0.5, true);
  EXPECT_DOUBLE_EQ(1.0, velocities.left);
  EXPECT_DOUBLE_EQ(0.25, velocities.right);

  // Backward
  velocities = wpi::DifferentialDrive::TankDriveIK(-1.0, -1.0, true);
  EXPECT_DOUBLE_EQ(-1.0, velocities.left);
  EXPECT_DOUBLE_EQ(-1.0, velocities.right);

  // Backward left turn
  velocities = wpi::DifferentialDrive::TankDriveIK(-0.5, -1.0, true);
  EXPECT_DOUBLE_EQ(-0.25, velocities.left);
  EXPECT_DOUBLE_EQ(-1.0, velocities.right);

  // Backward right turn
  velocities = wpi::DifferentialDrive::TankDriveIK(-1.0, -0.5, true);
  EXPECT_DOUBLE_EQ(-1.0, velocities.left);
  EXPECT_DOUBLE_EQ(-0.25, velocities.right);
}

TEST(DifferentialDriveTest, ArcadeDrive) {
  wpi::MockPWMMotorController left;
  wpi::MockPWMMotorController right;
  wpi::DifferentialDrive drive{
      [&](double output) { left.SetDutyCycle(output); },
      [&](double output) { right.SetDutyCycle(output); }};
  drive.SetDeadband(0.0);

  // Forward
  drive.ArcadeDrive(1.0, 0.0, false);
  EXPECT_DOUBLE_EQ(1.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, right.GetDutyCycle());

  // Forward left turn
  drive.ArcadeDrive(0.5, 0.5, false);
  EXPECT_DOUBLE_EQ(0.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(0.5, right.GetDutyCycle());

  // Forward right turn
  drive.ArcadeDrive(0.5, -0.5, false);
  EXPECT_DOUBLE_EQ(0.5, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(0.0, right.GetDutyCycle());

  // Backward
  drive.ArcadeDrive(-1.0, 0.0, false);
  EXPECT_DOUBLE_EQ(-1.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, right.GetDutyCycle());

  // Backward left turn
  drive.ArcadeDrive(-0.5, 0.5, false);
  EXPECT_DOUBLE_EQ(-0.5, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(0.0, right.GetDutyCycle());

  // Backward right turn
  drive.ArcadeDrive(-0.5, -0.5, false);
  EXPECT_DOUBLE_EQ(0.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-0.5, right.GetDutyCycle());
}

TEST(DifferentialDriveTest, ArcadeDriveSquared) {
  wpi::MockPWMMotorController left;
  wpi::MockPWMMotorController right;
  wpi::DifferentialDrive drive{
      [&](double output) { left.SetDutyCycle(output); },
      [&](double output) { right.SetDutyCycle(output); }};
  drive.SetDeadband(0.0);

  // Forward
  drive.ArcadeDrive(1.0, 0.0, true);
  EXPECT_DOUBLE_EQ(1.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, right.GetDutyCycle());

  // Forward left turn
  drive.ArcadeDrive(0.5, 0.5, true);
  EXPECT_DOUBLE_EQ(0.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(0.25, right.GetDutyCycle());

  // Forward right turn
  drive.ArcadeDrive(0.5, -0.5, true);
  EXPECT_DOUBLE_EQ(0.25, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(0.0, right.GetDutyCycle());

  // Backward
  drive.ArcadeDrive(-1.0, 0.0, true);
  EXPECT_DOUBLE_EQ(-1.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, right.GetDutyCycle());

  // Backward left turn
  drive.ArcadeDrive(-0.5, 0.5, true);
  EXPECT_DOUBLE_EQ(-0.25, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(0.0, right.GetDutyCycle());

  // Backward right turn
  drive.ArcadeDrive(-0.5, -0.5, true);
  EXPECT_DOUBLE_EQ(0.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-0.25, right.GetDutyCycle());
}

TEST(DifferentialDriveTest, CurvatureDrive) {
  wpi::MockPWMMotorController left;
  wpi::MockPWMMotorController right;
  wpi::DifferentialDrive drive{
      [&](double output) { left.SetDutyCycle(output); },
      [&](double output) { right.SetDutyCycle(output); }};
  drive.SetDeadband(0.0);

  // Forward
  drive.CurvatureDrive(1.0, 0.0, false);
  EXPECT_DOUBLE_EQ(1.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, right.GetDutyCycle());

  // Forward left turn
  drive.CurvatureDrive(0.5, 0.5, false);
  EXPECT_DOUBLE_EQ(0.25, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(0.75, right.GetDutyCycle());

  // Forward right turn
  drive.CurvatureDrive(0.5, -0.5, false);
  EXPECT_DOUBLE_EQ(0.75, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(0.25, right.GetDutyCycle());

  // Backward
  drive.CurvatureDrive(-1.0, 0.0, false);
  EXPECT_DOUBLE_EQ(-1.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, right.GetDutyCycle());

  // Backward left turn
  drive.CurvatureDrive(-0.5, 0.5, false);
  EXPECT_DOUBLE_EQ(-0.75, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-0.25, right.GetDutyCycle());

  // Backward right turn
  drive.CurvatureDrive(-0.5, -0.5, false);
  EXPECT_DOUBLE_EQ(-0.25, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-0.75, right.GetDutyCycle());
}

TEST(DifferentialDriveTest, CurvatureDriveTurnInPlace) {
  wpi::MockPWMMotorController left;
  wpi::MockPWMMotorController right;
  wpi::DifferentialDrive drive{
      [&](double output) { left.SetDutyCycle(output); },
      [&](double output) { right.SetDutyCycle(output); }};
  drive.SetDeadband(0.0);

  // Forward
  drive.CurvatureDrive(1.0, 0.0, true);
  EXPECT_DOUBLE_EQ(1.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, right.GetDutyCycle());

  // Forward left turn
  drive.CurvatureDrive(0.5, 0.5, true);
  EXPECT_DOUBLE_EQ(0.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, right.GetDutyCycle());

  // Forward right turn
  drive.CurvatureDrive(0.5, -0.5, true);
  EXPECT_DOUBLE_EQ(1.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(0.0, right.GetDutyCycle());

  // Backward
  drive.CurvatureDrive(-1.0, 0.0, true);
  EXPECT_DOUBLE_EQ(-1.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, right.GetDutyCycle());

  // Backward left turn
  drive.CurvatureDrive(-0.5, 0.5, true);
  EXPECT_DOUBLE_EQ(-1.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(0.0, right.GetDutyCycle());

  // Backward right turn
  drive.CurvatureDrive(-0.5, -0.5, true);
  EXPECT_DOUBLE_EQ(0.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, right.GetDutyCycle());
}

TEST(DifferentialDriveTest, TankDrive) {
  wpi::MockPWMMotorController left;
  wpi::MockPWMMotorController right;
  wpi::DifferentialDrive drive{
      [&](double output) { left.SetDutyCycle(output); },
      [&](double output) { right.SetDutyCycle(output); }};
  drive.SetDeadband(0.0);

  // Forward
  drive.TankDrive(1.0, 1.0, false);
  EXPECT_DOUBLE_EQ(1.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, right.GetDutyCycle());

  // Forward left turn
  drive.TankDrive(0.5, 1.0, false);
  EXPECT_DOUBLE_EQ(0.5, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, right.GetDutyCycle());

  // Forward right turn
  drive.TankDrive(1.0, 0.5, false);
  EXPECT_DOUBLE_EQ(1.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(0.5, right.GetDutyCycle());

  // Backward
  drive.TankDrive(-1.0, -1.0, false);
  EXPECT_DOUBLE_EQ(-1.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, right.GetDutyCycle());

  // Backward left turn
  drive.TankDrive(-0.5, -1.0, false);
  EXPECT_DOUBLE_EQ(-0.5, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, right.GetDutyCycle());

  // Backward right turn
  drive.TankDrive(-0.5, 1.0, false);
  EXPECT_DOUBLE_EQ(-0.5, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, right.GetDutyCycle());
}

TEST(DifferentialDriveTest, TankDriveSquared) {
  wpi::MockPWMMotorController left;
  wpi::MockPWMMotorController right;
  wpi::DifferentialDrive drive{
      [&](double output) { left.SetDutyCycle(output); },
      [&](double output) { right.SetDutyCycle(output); }};
  drive.SetDeadband(0.0);

  // Forward
  drive.TankDrive(1.0, 1.0, true);
  EXPECT_DOUBLE_EQ(1.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, right.GetDutyCycle());

  // Forward left turn
  drive.TankDrive(0.5, 1.0, true);
  EXPECT_DOUBLE_EQ(0.25, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, right.GetDutyCycle());

  // Forward right turn
  drive.TankDrive(1.0, 0.5, true);
  EXPECT_DOUBLE_EQ(1.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(0.25, right.GetDutyCycle());

  // Backward
  drive.TankDrive(-1.0, -1.0, true);
  EXPECT_DOUBLE_EQ(-1.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, right.GetDutyCycle());

  // Backward left turn
  drive.TankDrive(-0.5, -1.0, true);
  EXPECT_DOUBLE_EQ(-0.25, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, right.GetDutyCycle());

  // Backward right turn
  drive.TankDrive(-1.0, -0.5, true);
  EXPECT_DOUBLE_EQ(-1.0, left.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-0.25, right.GetDutyCycle());
}
