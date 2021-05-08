// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "MockMotorController.h"
#include "frc/drive/MecanumDrive.h"
#include "gtest/gtest.h"

TEST(MecanumDriveTest, Cartesian) {
  frc::MockMotorController fl;
  frc::MockMotorController fr;
  frc::MockMotorController rl;
  frc::MockMotorController rr;
  frc::MecanumDrive drive{fl, fr, rl, rr};
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
  EXPECT_DOUBLE_EQ(-1.0, fr.Get());
  EXPECT_DOUBLE_EQ(1.0, rl.Get());
  EXPECT_DOUBLE_EQ(1.0, rr.Get());

  // Rotate CW
  drive.DriveCartesian(0.0, 0.0, 1.0);
  EXPECT_DOUBLE_EQ(1.0, fl.Get());
  EXPECT_DOUBLE_EQ(1.0, fr.Get());
  EXPECT_DOUBLE_EQ(-1.0, rl.Get());
  EXPECT_DOUBLE_EQ(-1.0, rr.Get());
}

TEST(MecanumDriveTest, CartesianGyro90CW) {
  frc::MockMotorController fl;
  frc::MockMotorController fr;
  frc::MockMotorController rl;
  frc::MockMotorController rr;
  frc::MecanumDrive drive{fl, fr, rl, rr};
  drive.SetDeadband(0.0);

  // Forward in global frame; left in robot frame
  drive.DriveCartesian(1.0, 0.0, 0.0, 90.0);
  EXPECT_DOUBLE_EQ(-1.0, fl.Get());
  EXPECT_DOUBLE_EQ(1.0, fr.Get());
  EXPECT_DOUBLE_EQ(1.0, rl.Get());
  EXPECT_DOUBLE_EQ(-1.0, rr.Get());

  // Left in global frame; backward in robot frame
  drive.DriveCartesian(0.0, -1.0, 0.0, 90.0);
  EXPECT_DOUBLE_EQ(-1.0, fl.Get());
  EXPECT_DOUBLE_EQ(-1.0, fr.Get());
  EXPECT_DOUBLE_EQ(-1.0, rl.Get());
  EXPECT_DOUBLE_EQ(-1.0, rr.Get());

  // Right in global frame; forward in robot frame
  drive.DriveCartesian(0.0, 1.0, 0.0, 90.0);
  EXPECT_DOUBLE_EQ(1.0, fl.Get());
  EXPECT_DOUBLE_EQ(1.0, fr.Get());
  EXPECT_DOUBLE_EQ(1.0, rl.Get());
  EXPECT_DOUBLE_EQ(1.0, rr.Get());

  // Rotate CCW
  drive.DriveCartesian(0.0, 0.0, -1.0, 90.0);
  EXPECT_DOUBLE_EQ(-1.0, fl.Get());
  EXPECT_DOUBLE_EQ(-1.0, fr.Get());
  EXPECT_DOUBLE_EQ(1.0, rl.Get());
  EXPECT_DOUBLE_EQ(1.0, rr.Get());

  // Rotate CW
  drive.DriveCartesian(0.0, 0.0, 1.0, 90.0);
  EXPECT_DOUBLE_EQ(1.0, fl.Get());
  EXPECT_DOUBLE_EQ(1.0, fr.Get());
  EXPECT_DOUBLE_EQ(-1.0, rl.Get());
  EXPECT_DOUBLE_EQ(-1.0, rr.Get());
}

TEST(MecanumDriveTest, Polar) {
  frc::MockMotorController fl;
  frc::MockMotorController fr;
  frc::MockMotorController rl;
  frc::MockMotorController rr;
  frc::MecanumDrive drive{fl, fr, rl, rr};
  drive.SetDeadband(0.0);

  // Forward
  drive.DrivePolar(1.0, 0.0, 0.0);
  EXPECT_DOUBLE_EQ(1.0, fl.Get());
  EXPECT_DOUBLE_EQ(1.0, fr.Get());
  EXPECT_DOUBLE_EQ(1.0, rl.Get());
  EXPECT_DOUBLE_EQ(1.0, rr.Get());

  // Left
  drive.DrivePolar(1.0, -90.0, 0.0);
  EXPECT_DOUBLE_EQ(-1.0, fl.Get());
  EXPECT_DOUBLE_EQ(1.0, fr.Get());
  EXPECT_DOUBLE_EQ(1.0, rl.Get());
  EXPECT_DOUBLE_EQ(-1.0, rr.Get());

  // Right
  drive.DrivePolar(1.0, 90.0, 0.0);
  EXPECT_DOUBLE_EQ(1.0, fl.Get());
  EXPECT_DOUBLE_EQ(-1.0, fr.Get());
  EXPECT_DOUBLE_EQ(-1.0, rl.Get());
  EXPECT_DOUBLE_EQ(1.0, rr.Get());

  // Rotate CCW
  drive.DrivePolar(0.0, 0.0, -1.0);
  EXPECT_DOUBLE_EQ(-1.0, fl.Get());
  EXPECT_DOUBLE_EQ(-1.0, fr.Get());
  EXPECT_DOUBLE_EQ(1.0, rl.Get());
  EXPECT_DOUBLE_EQ(1.0, rr.Get());

  // Rotate CW
  drive.DrivePolar(0.0, 0.0, 1.0);
  EXPECT_DOUBLE_EQ(1.0, fl.Get());
  EXPECT_DOUBLE_EQ(1.0, fr.Get());
  EXPECT_DOUBLE_EQ(-1.0, rl.Get());
  EXPECT_DOUBLE_EQ(-1.0, rr.Get());
}
