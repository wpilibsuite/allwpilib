// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/drive/MecanumDrive.hpp"

#include <gtest/gtest.h>

#include "motorcontrol/MockPWMMotorController.hpp"

TEST(MecanumDriveTest, CartesianIK) {
  // Forward
  auto velocities = wpi::MecanumDrive::DriveCartesianIK(1.0, 0.0, 0.0);
  EXPECT_DOUBLE_EQ(1.0, velocities.frontLeft);
  EXPECT_DOUBLE_EQ(1.0, velocities.frontRight);
  EXPECT_DOUBLE_EQ(1.0, velocities.rearLeft);
  EXPECT_DOUBLE_EQ(1.0, velocities.rearRight);

  // Left
  velocities = wpi::MecanumDrive::DriveCartesianIK(0.0, -1.0, 0.0);
  EXPECT_DOUBLE_EQ(-1.0, velocities.frontLeft);
  EXPECT_DOUBLE_EQ(1.0, velocities.frontRight);
  EXPECT_DOUBLE_EQ(1.0, velocities.rearLeft);
  EXPECT_DOUBLE_EQ(-1.0, velocities.rearRight);

  // Right
  velocities = wpi::MecanumDrive::DriveCartesianIK(0.0, 1.0, 0.0);
  EXPECT_DOUBLE_EQ(1.0, velocities.frontLeft);
  EXPECT_DOUBLE_EQ(-1.0, velocities.frontRight);
  EXPECT_DOUBLE_EQ(-1.0, velocities.rearLeft);
  EXPECT_DOUBLE_EQ(1.0, velocities.rearRight);

  // Rotate CCW
  velocities = wpi::MecanumDrive::DriveCartesianIK(0.0, 0.0, -1.0);
  EXPECT_DOUBLE_EQ(-1.0, velocities.frontLeft);
  EXPECT_DOUBLE_EQ(1.0, velocities.frontRight);
  EXPECT_DOUBLE_EQ(-1.0, velocities.rearLeft);
  EXPECT_DOUBLE_EQ(1.0, velocities.rearRight);

  // Rotate CW
  velocities = wpi::MecanumDrive::DriveCartesianIK(0.0, 0.0, 1.0);
  EXPECT_DOUBLE_EQ(1.0, velocities.frontLeft);
  EXPECT_DOUBLE_EQ(-1.0, velocities.frontRight);
  EXPECT_DOUBLE_EQ(1.0, velocities.rearLeft);
  EXPECT_DOUBLE_EQ(-1.0, velocities.rearRight);
}

TEST(MecanumDriveTest, CartesianIKGyro90CW) {
  // Forward in global frame; left in robot frame
  auto velocities = wpi::MecanumDrive::DriveCartesianIK(1.0, 0.0, 0.0, 90_deg);
  EXPECT_DOUBLE_EQ(-1.0, velocities.frontLeft);
  EXPECT_DOUBLE_EQ(1.0, velocities.frontRight);
  EXPECT_DOUBLE_EQ(1.0, velocities.rearLeft);
  EXPECT_DOUBLE_EQ(-1.0, velocities.rearRight);

  // Left in global frame; backward in robot frame
  velocities = wpi::MecanumDrive::DriveCartesianIK(0.0, -1.0, 0.0, 90_deg);
  EXPECT_DOUBLE_EQ(-1.0, velocities.frontLeft);
  EXPECT_DOUBLE_EQ(-1.0, velocities.frontRight);
  EXPECT_DOUBLE_EQ(-1.0, velocities.rearLeft);
  EXPECT_DOUBLE_EQ(-1.0, velocities.rearRight);

  // Right in global frame; forward in robot frame
  velocities = wpi::MecanumDrive::DriveCartesianIK(0.0, 1.0, 0.0, 90_deg);
  EXPECT_DOUBLE_EQ(1.0, velocities.frontLeft);
  EXPECT_DOUBLE_EQ(1.0, velocities.frontRight);
  EXPECT_DOUBLE_EQ(1.0, velocities.rearLeft);
  EXPECT_DOUBLE_EQ(1.0, velocities.rearRight);

  // Rotate CCW
  velocities = wpi::MecanumDrive::DriveCartesianIK(0.0, 0.0, -1.0, 90_deg);
  EXPECT_DOUBLE_EQ(-1.0, velocities.frontLeft);
  EXPECT_DOUBLE_EQ(1.0, velocities.frontRight);
  EXPECT_DOUBLE_EQ(-1.0, velocities.rearLeft);
  EXPECT_DOUBLE_EQ(1.0, velocities.rearRight);

  // Rotate CW
  velocities = wpi::MecanumDrive::DriveCartesianIK(0.0, 0.0, 1.0, 90_deg);
  EXPECT_DOUBLE_EQ(1.0, velocities.frontLeft);
  EXPECT_DOUBLE_EQ(-1.0, velocities.frontRight);
  EXPECT_DOUBLE_EQ(1.0, velocities.rearLeft);
  EXPECT_DOUBLE_EQ(-1.0, velocities.rearRight);
}

TEST(MecanumDriveTest, Cartesian) {
  wpi::MockPWMMotorController fl;
  wpi::MockPWMMotorController rl;
  wpi::MockPWMMotorController fr;
  wpi::MockPWMMotorController rr;
  wpi::MecanumDrive drive{[&](double output) { fl.SetDutyCycle(output); },
                          [&](double output) { rl.SetDutyCycle(output); },
                          [&](double output) { fr.SetDutyCycle(output); },
                          [&](double output) { rr.SetDutyCycle(output); }};
  drive.SetDeadband(0.0);

  // Forward
  drive.DriveCartesian(1.0, 0.0, 0.0);
  EXPECT_DOUBLE_EQ(1.0, fl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, fr.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, rl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, rr.GetDutyCycle());

  // Left
  drive.DriveCartesian(0.0, -1.0, 0.0);
  EXPECT_DOUBLE_EQ(-1.0, fl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, fr.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, rl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, rr.GetDutyCycle());

  // Right
  drive.DriveCartesian(0.0, 1.0, 0.0);
  EXPECT_DOUBLE_EQ(1.0, fl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, fr.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, rl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, rr.GetDutyCycle());

  // Rotate CCW
  drive.DriveCartesian(0.0, 0.0, -1.0);
  EXPECT_DOUBLE_EQ(-1.0, fl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, fr.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, rl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, rr.GetDutyCycle());

  // Rotate CW
  drive.DriveCartesian(0.0, 0.0, 1.0);
  EXPECT_DOUBLE_EQ(1.0, fl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, fr.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, rl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, rr.GetDutyCycle());
}

TEST(MecanumDriveTest, CartesianGyro90CW) {
  wpi::MockPWMMotorController fl;
  wpi::MockPWMMotorController rl;
  wpi::MockPWMMotorController fr;
  wpi::MockPWMMotorController rr;
  wpi::MecanumDrive drive{[&](double output) { fl.SetDutyCycle(output); },
                          [&](double output) { rl.SetDutyCycle(output); },
                          [&](double output) { fr.SetDutyCycle(output); },
                          [&](double output) { rr.SetDutyCycle(output); }};
  drive.SetDeadband(0.0);

  // Forward in global frame; left in robot frame
  drive.DriveCartesian(1.0, 0.0, 0.0, 90_deg);
  EXPECT_DOUBLE_EQ(-1.0, fl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, fr.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, rl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, rr.GetDutyCycle());

  // Left in global frame; backward in robot frame
  drive.DriveCartesian(0.0, -1.0, 0.0, 90_deg);
  EXPECT_DOUBLE_EQ(-1.0, fl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, fr.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, rl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, rr.GetDutyCycle());

  // Right in global frame; forward in robot frame
  drive.DriveCartesian(0.0, 1.0, 0.0, 90_deg);
  EXPECT_DOUBLE_EQ(1.0, fl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, fr.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, rl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, rr.GetDutyCycle());

  // Rotate CCW
  drive.DriveCartesian(0.0, 0.0, -1.0, 90_deg);
  EXPECT_DOUBLE_EQ(-1.0, fl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, fr.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, rl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, rr.GetDutyCycle());

  // Rotate CW
  drive.DriveCartesian(0.0, 0.0, 1.0, 90_deg);
  EXPECT_DOUBLE_EQ(1.0, fl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, fr.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, rl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, rr.GetDutyCycle());
}

TEST(MecanumDriveTest, Polar) {
  wpi::MockPWMMotorController fl;
  wpi::MockPWMMotorController rl;
  wpi::MockPWMMotorController fr;
  wpi::MockPWMMotorController rr;
  wpi::MecanumDrive drive{[&](double output) { fl.SetDutyCycle(output); },
                          [&](double output) { rl.SetDutyCycle(output); },
                          [&](double output) { fr.SetDutyCycle(output); },
                          [&](double output) { rr.SetDutyCycle(output); }};
  drive.SetDeadband(0.0);

  // Forward
  drive.DrivePolar(1.0, 0_deg, 0.0);
  EXPECT_DOUBLE_EQ(1.0, fl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, fr.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, rl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, rr.GetDutyCycle());

  // Left
  drive.DrivePolar(1.0, -90_deg, 0.0);
  EXPECT_DOUBLE_EQ(-1.0, fl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, fr.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, rl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, rr.GetDutyCycle());

  // Right
  drive.DrivePolar(1.0, 90_deg, 0.0);
  EXPECT_DOUBLE_EQ(1.0, fl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, fr.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, rl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, rr.GetDutyCycle());

  // Rotate CCW
  drive.DrivePolar(0.0, 0_deg, -1.0);
  EXPECT_DOUBLE_EQ(-1.0, fl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, fr.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, rl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, rr.GetDutyCycle());

  // Rotate CW
  drive.DrivePolar(0.0, 0_deg, 1.0);
  EXPECT_DOUBLE_EQ(1.0, fl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, fr.GetDutyCycle());
  EXPECT_DOUBLE_EQ(1.0, rl.GetDutyCycle());
  EXPECT_DOUBLE_EQ(-1.0, rr.GetDutyCycle());
}
