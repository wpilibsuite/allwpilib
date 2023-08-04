// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.drive;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.wpilibj.motorcontrol.MockMotorController;
import org.junit.jupiter.api.Test;

class MecanumDriveTest {
  @Test
  void testCartesianIK() {
    // Forward
    var speeds = MecanumDrive.driveCartesianIK(1.0, 0.0, 0.0);
    assertEquals(1.0, speeds.frontLeft, 1e-9);
    assertEquals(1.0, speeds.frontRight, 1e-9);
    assertEquals(1.0, speeds.rearLeft, 1e-9);
    assertEquals(1.0, speeds.rearRight, 1e-9);

    // Left
    speeds = MecanumDrive.driveCartesianIK(0.0, -1.0, 0.0);
    assertEquals(-1.0, speeds.frontLeft, 1e-9);
    assertEquals(1.0, speeds.frontRight, 1e-9);
    assertEquals(1.0, speeds.rearLeft, 1e-9);
    assertEquals(-1.0, speeds.rearRight, 1e-9);

    // Right
    speeds = MecanumDrive.driveCartesianIK(0.0, 1.0, 0.0);
    assertEquals(1.0, speeds.frontLeft, 1e-9);
    assertEquals(-1.0, speeds.frontRight, 1e-9);
    assertEquals(-1.0, speeds.rearLeft, 1e-9);
    assertEquals(1.0, speeds.rearRight, 1e-9);

    // Rotate CCW
    speeds = MecanumDrive.driveCartesianIK(0.0, 0.0, -1.0);
    assertEquals(-1.0, speeds.frontLeft, 1e-9);
    assertEquals(1.0, speeds.frontRight, 1e-9);
    assertEquals(-1.0, speeds.rearLeft, 1e-9);
    assertEquals(1.0, speeds.rearRight, 1e-9);

    // Rotate CW
    speeds = MecanumDrive.driveCartesianIK(0.0, 0.0, 1.0);
    assertEquals(1.0, speeds.frontLeft, 1e-9);
    assertEquals(-1.0, speeds.frontRight, 1e-9);
    assertEquals(1.0, speeds.rearLeft, 1e-9);
    assertEquals(-1.0, speeds.rearRight, 1e-9);
  }

  @Test
  void testCartesianIKGyro90CW() {
    // Forward in global frame; left in robot frame
    var speeds = MecanumDrive.driveCartesianIK(1.0, 0.0, 0.0, Rotation2d.fromDegrees(90.0));
    assertEquals(-1.0, speeds.frontLeft, 1e-9);
    assertEquals(1.0, speeds.frontRight, 1e-9);
    assertEquals(1.0, speeds.rearLeft, 1e-9);
    assertEquals(-1.0, speeds.rearRight, 1e-9);

    // Left in global frame; backward in robot frame
    speeds = MecanumDrive.driveCartesianIK(0.0, -1.0, 0.0, Rotation2d.fromDegrees(90.0));
    assertEquals(-1.0, speeds.frontLeft, 1e-9);
    assertEquals(-1.0, speeds.frontRight, 1e-9);
    assertEquals(-1.0, speeds.rearLeft, 1e-9);
    assertEquals(-1.0, speeds.rearRight, 1e-9);

    // Right in global frame; forward in robot frame
    speeds = MecanumDrive.driveCartesianIK(0.0, 1.0, 0.0, Rotation2d.fromDegrees(90.0));
    assertEquals(1.0, speeds.frontLeft, 1e-9);
    assertEquals(1.0, speeds.frontRight, 1e-9);
    assertEquals(1.0, speeds.rearLeft, 1e-9);
    assertEquals(1.0, speeds.rearRight, 1e-9);

    // Rotate CCW
    speeds = MecanumDrive.driveCartesianIK(0.0, 0.0, -1.0, Rotation2d.fromDegrees(90.0));
    assertEquals(-1.0, speeds.frontLeft, 1e-9);
    assertEquals(1.0, speeds.frontRight, 1e-9);
    assertEquals(-1.0, speeds.rearLeft, 1e-9);
    assertEquals(1.0, speeds.rearRight, 1e-9);

    // Rotate CW
    speeds = MecanumDrive.driveCartesianIK(0.0, 0.0, 1.0, Rotation2d.fromDegrees(90.0));
    assertEquals(1.0, speeds.frontLeft, 1e-9);
    assertEquals(-1.0, speeds.frontRight, 1e-9);
    assertEquals(1.0, speeds.rearLeft, 1e-9);
    assertEquals(-1.0, speeds.rearRight, 1e-9);
  }

  @Test
  void testCartesian() {
    var fl = new MockMotorController();
    var fr = new MockMotorController();
    var rl = new MockMotorController();
    var rr = new MockMotorController();
    var drive = new MecanumDrive(fl, rl, fr, rr);
    drive.setDeadband(0.0);

    // Forward
    drive.driveCartesian(1.0, 0.0, 0.0);
    assertEquals(1.0, fl.get(), 1e-9);
    assertEquals(1.0, fr.get(), 1e-9);
    assertEquals(1.0, rl.get(), 1e-9);
    assertEquals(1.0, rr.get(), 1e-9);

    // Left
    drive.driveCartesian(0.0, -1.0, 0.0);
    assertEquals(-1.0, fl.get(), 1e-9);
    assertEquals(1.0, fr.get(), 1e-9);
    assertEquals(1.0, rl.get(), 1e-9);
    assertEquals(-1.0, rr.get(), 1e-9);

    // Right
    drive.driveCartesian(0.0, 1.0, 0.0);
    assertEquals(1.0, fl.get(), 1e-9);
    assertEquals(-1.0, fr.get(), 1e-9);
    assertEquals(-1.0, rl.get(), 1e-9);
    assertEquals(1.0, rr.get(), 1e-9);

    // Rotate CCW
    drive.driveCartesian(0.0, 0.0, -1.0);
    assertEquals(-1.0, fl.get(), 1e-9);
    assertEquals(1.0, fr.get(), 1e-9);
    assertEquals(-1.0, rl.get(), 1e-9);
    assertEquals(1.0, rr.get(), 1e-9);

    // Rotate CW
    drive.driveCartesian(0.0, 0.0, 1.0);
    assertEquals(1.0, fl.get(), 1e-9);
    assertEquals(-1.0, fr.get(), 1e-9);
    assertEquals(1.0, rl.get(), 1e-9);
    assertEquals(-1.0, rr.get(), 1e-9);
  }

  @Test
  void testCartesianGyro90CW() {
    var fl = new MockMotorController();
    var fr = new MockMotorController();
    var rl = new MockMotorController();
    var rr = new MockMotorController();
    var drive = new MecanumDrive(fl, rl, fr, rr);
    drive.setDeadband(0.0);

    // Forward in global frame; left in robot frame
    drive.driveCartesian(1.0, 0.0, 0.0, Rotation2d.fromDegrees(90.0));
    assertEquals(-1.0, fl.get(), 1e-9);
    assertEquals(1.0, fr.get(), 1e-9);
    assertEquals(1.0, rl.get(), 1e-9);
    assertEquals(-1.0, rr.get(), 1e-9);

    // Left in global frame; backward in robot frame
    drive.driveCartesian(0.0, -1.0, 0.0, Rotation2d.fromDegrees(90.0));
    assertEquals(-1.0, fl.get(), 1e-9);
    assertEquals(-1.0, fr.get(), 1e-9);
    assertEquals(-1.0, rl.get(), 1e-9);
    assertEquals(-1.0, rr.get(), 1e-9);

    // Right in global frame; forward in robot frame
    drive.driveCartesian(0.0, 1.0, 0.0, Rotation2d.fromDegrees(90.0));
    assertEquals(1.0, fl.get(), 1e-9);
    assertEquals(1.0, fr.get(), 1e-9);
    assertEquals(1.0, rl.get(), 1e-9);
    assertEquals(1.0, rr.get(), 1e-9);

    // Rotate CCW
    drive.driveCartesian(0.0, 0.0, -1.0, Rotation2d.fromDegrees(90.0));
    assertEquals(-1.0, fl.get(), 1e-9);
    assertEquals(1.0, fr.get(), 1e-9);
    assertEquals(-1.0, rl.get(), 1e-9);
    assertEquals(1.0, rr.get(), 1e-9);

    // Rotate CW
    drive.driveCartesian(0.0, 0.0, 1.0, Rotation2d.fromDegrees(90.0));
    assertEquals(1.0, fl.get(), 1e-9);
    assertEquals(-1.0, fr.get(), 1e-9);
    assertEquals(1.0, rl.get(), 1e-9);
    assertEquals(-1.0, rr.get(), 1e-9);
  }

  @Test
  void testPolar() {
    var fl = new MockMotorController();
    var fr = new MockMotorController();
    var rl = new MockMotorController();
    var rr = new MockMotorController();
    var drive = new MecanumDrive(fl, rl, fr, rr);
    drive.setDeadband(0.0);

    // Forward
    drive.drivePolar(1.0, Rotation2d.fromDegrees(0.0), 0.0);
    assertEquals(1.0, fl.get(), 1e-9);
    assertEquals(1.0, fr.get(), 1e-9);
    assertEquals(1.0, rl.get(), 1e-9);
    assertEquals(1.0, rr.get(), 1e-9);

    // Left
    drive.drivePolar(1.0, Rotation2d.fromDegrees(-90.0), 0.0);
    assertEquals(-1.0, fl.get(), 1e-9);
    assertEquals(1.0, fr.get(), 1e-9);
    assertEquals(1.0, rl.get(), 1e-9);
    assertEquals(-1.0, rr.get(), 1e-9);

    // Right
    drive.drivePolar(1.0, Rotation2d.fromDegrees(90.0), 0.0);
    assertEquals(1.0, fl.get(), 1e-9);
    assertEquals(-1.0, fr.get(), 1e-9);
    assertEquals(-1.0, rl.get(), 1e-9);
    assertEquals(1.0, rr.get(), 1e-9);

    // Rotate CCW
    drive.drivePolar(0.0, Rotation2d.fromDegrees(0.0), -1.0);
    assertEquals(-1.0, fl.get(), 1e-9);
    assertEquals(1.0, fr.get(), 1e-9);
    assertEquals(-1.0, rl.get(), 1e-9);
    assertEquals(1.0, rr.get(), 1e-9);

    // Rotate CW
    drive.drivePolar(0.0, Rotation2d.fromDegrees(0.0), 1.0);
    assertEquals(1.0, fl.get(), 1e-9);
    assertEquals(-1.0, fr.get(), 1e-9);
    assertEquals(1.0, rl.get(), 1e-9);
    assertEquals(-1.0, rr.get(), 1e-9);
  }
}
