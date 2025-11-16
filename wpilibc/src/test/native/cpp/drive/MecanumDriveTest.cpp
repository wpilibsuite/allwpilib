// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/drive/MecanumDrive.hpp"

#include <gtest/gtest.h>

#include "motorcontrol/MockPWMMotorController.hpp"

TEST(MecanumDriveTest, CartesianIK) {
  // Forward
  auto speeds = wpi::MecanumDrive::DriveCartesianIK(1.0, 0.0, 0.0);
  EXPECT_DOUBLE_EQ(1.0, speeds.frontLeft);
  EXPECT_DOUBLE_EQ(1.0, speeds.frontRight);
  EXPECT_DOUBLE_EQ(1.0, speeds.rearLeft);
  EXPECT_DOUBLE_EQ(1.0, speeds.rearRight);

  // Left
  speeds = wpi::MecanumDrive::DriveCartesianIK(0.0, -1.0, 0.0);
  EXPECT_DOUBLE_EQ(-1.0, speeds.frontLeft);
  EXPECT_DOUBLE_EQ(1.0, speeds.frontRight);
  EXPECT_DOUBLE_EQ(1.0, speeds.rearLeft);
  EXPECT_DOUBLE_EQ(-1.0, speeds.rearRight);

  // Right
  speeds = wpi::MecanumDrive::DriveCartesianIK(0.0, 1.0, 0.0);
  EXPECT_DOUBLE_EQ(1.0, speeds.frontLeft);
  EXPECT_DOUBLE_EQ(-1.0, speeds.frontRight);
  EXPECT_DOUBLE_EQ(-1.0, speeds.rearLeft);
  EXPECT_DOUBLE_EQ(1.0, speeds.rearRight);

  // Rotate CCW
  speeds = wpi::MecanumDrive::DriveCartesianIK(0.0, 0.0, -1.0);
  EXPECT_DOUBLE_EQ(-1.0, speeds.frontLeft);
  EXPECT_DOUBLE_EQ(1.0, speeds.frontRight);
  EXPECT_DOUBLE_EQ(-1.0, speeds.rearLeft);
  EXPECT_DOUBLE_EQ(1.0, speeds.rearRight);

  // Rotate CW
  speeds = wpi::MecanumDrive::DriveCartesianIK(0.0, 0.0, 1.0);
  EXPECT_DOUBLE_EQ(1.0, speeds.frontLeft);
  EXPECT_DOUBLE_EQ(-1.0, speeds.frontRight);
  EXPECT_DOUBLE_EQ(1.0, speeds.rearLeft);
  EXPECT_DOUBLE_EQ(-1.0, speeds.rearRight);
}

TEST(MecanumDriveTest, CartesianIKGyro90CW) {
  // Forward in global frame; left in robot frame
  auto speeds = wpi::MecanumDrive::DriveCartesianIK(1.0, 0.0, 0.0, 90_deg);
  EXPECT_DOUBLE_EQ(-1.0, speeds.frontLeft);
  EXPECT_DOUBLE_EQ(1.0, speeds.frontRight);
  EXPECT_DOUBLE_EQ(1.0, speeds.rearLeft);
  EXPECT_DOUBLE_EQ(-1.0, speeds.rearRight);

  // Left in global frame; backward in robot frame
  speeds = wpi::MecanumDrive::DriveCartesianIK(0.0, -1.0, 0.0, 90_deg);
  EXPECT_DOUBLE_EQ(-1.0, speeds.frontLeft);
  EXPECT_DOUBLE_EQ(-1.0, speeds.frontRight);
  EXPECT_DOUBLE_EQ(-1.0, speeds.rearLeft);
  EXPECT_DOUBLE_EQ(-1.0, speeds.rearRight);

  // Right in global frame; forward in robot frame
  speeds = wpi::MecanumDrive::DriveCartesianIK(0.0, 1.0, 0.0, 90_deg);
  EXPECT_DOUBLE_EQ(1.0, speeds.frontLeft);
  EXPECT_DOUBLE_EQ(1.0, speeds.frontRight);
  EXPECT_DOUBLE_EQ(1.0, speeds.rearLeft);
  EXPECT_DOUBLE_EQ(1.0, speeds.rearRight);

  // Rotate CCW
  speeds = wpi::MecanumDrive::DriveCartesianIK(0.0, 0.0, -1.0, 90_deg);
  EXPECT_DOUBLE_EQ(-1.0, speeds.frontLeft);
  EXPECT_DOUBLE_EQ(1.0, speeds.frontRight);
  EXPECT_DOUBLE_EQ(-1.0, speeds.rearLeft);
  EXPECT_DOUBLE_EQ(1.0, speeds.rearRight);

  // Rotate CW
  speeds = wpi::MecanumDrive::DriveCartesianIK(0.0, 0.0, 1.0, 90_deg);
  EXPECT_DOUBLE_EQ(1.0, speeds.frontLeft);
  EXPECT_DOUBLE_EQ(-1.0, speeds.frontRight);
  EXPECT_DOUBLE_EQ(1.0, speeds.rearLeft);
  EXPECT_DOUBLE_EQ(-1.0, speeds.rearRight);
}

TEST(MecanumDriveTest, Cartesian) {
  wpi::MockPWMMotorController fl;
  wpi::MockPWMMotorController rl;
  wpi::MockPWMMotorController fr;
  wpi::MockPWMMotorController rr;
  wpi::MecanumDrive drive{[&](double output) { fl.Set(output); },
                          [&](double output) { rl.Set(output); },
                          [&](double output) { fr.Set(output); },
                          [&](double output) { rr.Set(output); }};
  drive.SetDeadband(0.0);

  // Forward
  drive.DriveCartesian(1.0, 0.0, 0.0);
  EXPECT_DOUBLE_EQ(1.0, fl.Get());
  EXPECT_DOUBLE_EQ(1.0, fr.Get());
  EXPECT_DOUBLE_EQ(1.0, rl.Get());
  EXPECT_DOUBLE_EQ(1.0, rr.Get());

  // Left
  drive.DriveCartesian(0.0, -1.0, 0.0);
  EXPECT_DOUBLE_EQ(-1.0, fl.Get());
  EXPECT_DOUBLE_EQ(1.0, fr.Get());
  EXPECT_DOUBLE_EQ(1.0, rl.Get());
  EXPECT_DOUBLE_EQ(-1.0, rr.Get());

  // Right
  drive.DriveCartesian(0.0, 1.0, 0.0);
  EXPECT_DOUBLE_EQ(1.0, fl.Get());
  EXPECT_DOUBLE_EQ(-1.0, fr.Get());
  EXPECT_DOUBLE_EQ(-1.0, rl.Get());
  EXPECT_DOUBLE_EQ(1.0, rr.Get());

  // Rotate CCW
  drive.DriveCartesian(0.0, 0.0, -1.0);
  EXPECT_DOUBLE_EQ(-1.0, fl.Get());
  EXPECT_DOUBLE_EQ(1.0, fr.Get());
  EXPECT_DOUBLE_EQ(-1.0, rl.Get());
  EXPECT_DOUBLE_EQ(1.0, rr.Get());

  // Rotate CW
  drive.DriveCartesian(0.0, 0.0, 1.0);
  EXPECT_DOUBLE_EQ(1.0, fl.Get());
  EXPECT_DOUBLE_EQ(-1.0, fr.Get());
  EXPECT_DOUBLE_EQ(1.0, rl.Get());
  EXPECT_DOUBLE_EQ(-1.0, rr.Get());
}

TEST(MecanumDriveTest, CartesianGyro90CW) {
  wpi::MockPWMMotorController fl;
  wpi::MockPWMMotorController rl;
  wpi::MockPWMMotorController fr;
  wpi::MockPWMMotorController rr;
  wpi::MecanumDrive drive{[&](double output) { fl.Set(output); },
                          [&](double output) { rl.Set(output); },
                          [&](double output) { fr.Set(output); },
                          [&](double output) { rr.Set(output); }};
  drive.SetDeadband(0.0);

  // Forward in global frame; left in robot frame
  drive.DriveCartesian(1.0, 0.0, 0.0, 90_deg);
  EXPECT_DOUBLE_EQ(-1.0, fl.Get());
  EXPECT_DOUBLE_EQ(1.0, fr.Get());
  EXPECT_DOUBLE_EQ(1.0, rl.Get());
  EXPECT_DOUBLE_EQ(-1.0, rr.Get());

  // Left in global frame; backward in robot frame
  drive.DriveCartesian(0.0, -1.0, 0.0, 90_deg);
  EXPECT_DOUBLE_EQ(-1.0, fl.Get());
  EXPECT_DOUBLE_EQ(-1.0, fr.Get());
  EXPECT_DOUBLE_EQ(-1.0, rl.Get());
  EXPECT_DOUBLE_EQ(-1.0, rr.Get());

  // Right in global frame; forward in robot frame
  drive.DriveCartesian(0.0, 1.0, 0.0, 90_deg);
  EXPECT_DOUBLE_EQ(1.0, fl.Get());
  EXPECT_DOUBLE_EQ(1.0, fr.Get());
  EXPECT_DOUBLE_EQ(1.0, rl.Get());
  EXPECT_DOUBLE_EQ(1.0, rr.Get());

  // Rotate CCW
  drive.DriveCartesian(0.0, 0.0, -1.0, 90_deg);
  EXPECT_DOUBLE_EQ(-1.0, fl.Get());
  EXPECT_DOUBLE_EQ(1.0, fr.Get());
  EXPECT_DOUBLE_EQ(-1.0, rl.Get());
  EXPECT_DOUBLE_EQ(1.0, rr.Get());

  // Rotate CW
  drive.DriveCartesian(0.0, 0.0, 1.0, 90_deg);
  EXPECT_DOUBLE_EQ(1.0, fl.Get());
  EXPECT_DOUBLE_EQ(-1.0, fr.Get());
  EXPECT_DOUBLE_EQ(1.0, rl.Get());
  EXPECT_DOUBLE_EQ(-1.0, rr.Get());
}

TEST(MecanumDriveTest, Polar) {
  wpi::MockPWMMotorController fl;
  wpi::MockPWMMotorController rl;
  wpi::MockPWMMotorController fr;
  wpi::MockPWMMotorController rr;
  wpi::MecanumDrive drive{[&](double output) { fl.Set(output); },
                          [&](double output) { rl.Set(output); },
                          [&](double output) { fr.Set(output); },
                          [&](double output) { rr.Set(output); }};
  drive.SetDeadband(0.0);

  // Forward
  drive.DrivePolar(1.0, 0_deg, 0.0);
  EXPECT_DOUBLE_EQ(1.0, fl.Get());
  EXPECT_DOUBLE_EQ(1.0, fr.Get());
  EXPECT_DOUBLE_EQ(1.0, rl.Get());
  EXPECT_DOUBLE_EQ(1.0, rr.Get());

  // Left
  drive.DrivePolar(1.0, -90_deg, 0.0);
  EXPECT_DOUBLE_EQ(-1.0, fl.Get());
  EXPECT_DOUBLE_EQ(1.0, fr.Get());
  EXPECT_DOUBLE_EQ(1.0, rl.Get());
  EXPECT_DOUBLE_EQ(-1.0, rr.Get());

  // Right
  drive.DrivePolar(1.0, 90_deg, 0.0);
  EXPECT_DOUBLE_EQ(1.0, fl.Get());
  EXPECT_DOUBLE_EQ(-1.0, fr.Get());
  EXPECT_DOUBLE_EQ(-1.0, rl.Get());
  EXPECT_DOUBLE_EQ(1.0, rr.Get());

  // Rotate CCW
  drive.DrivePolar(0.0, 0_deg, -1.0);
  EXPECT_DOUBLE_EQ(-1.0, fl.Get());
  EXPECT_DOUBLE_EQ(1.0, fr.Get());
  EXPECT_DOUBLE_EQ(-1.0, rl.Get());
  EXPECT_DOUBLE_EQ(1.0, rr.Get());

  // Rotate CW
  drive.DrivePolar(0.0, 0_deg, 1.0);
  EXPECT_DOUBLE_EQ(1.0, fl.Get());
  EXPECT_DOUBLE_EQ(-1.0, fr.Get());
  EXPECT_DOUBLE_EQ(1.0, rl.Get());
  EXPECT_DOUBLE_EQ(-1.0, rr.Get());
}
