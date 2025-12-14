// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.drive;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.hardware.motor.MockPWMMotorController;
import org.wpilib.math.geometry.Rotation2d;

@SuppressWarnings("resource")
class MecanumDriveTest {
  @Test
  void testCartesianIK() {
    // Forward
    var velocities = MecanumDrive.driveCartesianIK(1.0, 0.0, 0.0);
    assertEquals(1.0, velocities.frontLeft, 1e-9);
    assertEquals(1.0, velocities.frontRight, 1e-9);
    assertEquals(1.0, velocities.rearLeft, 1e-9);
    assertEquals(1.0, velocities.rearRight, 1e-9);

    // Left
    velocities = MecanumDrive.driveCartesianIK(0.0, -1.0, 0.0);
    assertEquals(-1.0, velocities.frontLeft, 1e-9);
    assertEquals(1.0, velocities.frontRight, 1e-9);
    assertEquals(1.0, velocities.rearLeft, 1e-9);
    assertEquals(-1.0, velocities.rearRight, 1e-9);

    // Right
    velocities = MecanumDrive.driveCartesianIK(0.0, 1.0, 0.0);
    assertEquals(1.0, velocities.frontLeft, 1e-9);
    assertEquals(-1.0, velocities.frontRight, 1e-9);
    assertEquals(-1.0, velocities.rearLeft, 1e-9);
    assertEquals(1.0, velocities.rearRight, 1e-9);

    // Rotate CCW
    velocities = MecanumDrive.driveCartesianIK(0.0, 0.0, -1.0);
    assertEquals(-1.0, velocities.frontLeft, 1e-9);
    assertEquals(1.0, velocities.frontRight, 1e-9);
    assertEquals(-1.0, velocities.rearLeft, 1e-9);
    assertEquals(1.0, velocities.rearRight, 1e-9);

    // Rotate CW
    velocities = MecanumDrive.driveCartesianIK(0.0, 0.0, 1.0);
    assertEquals(1.0, velocities.frontLeft, 1e-9);
    assertEquals(-1.0, velocities.frontRight, 1e-9);
    assertEquals(1.0, velocities.rearLeft, 1e-9);
    assertEquals(-1.0, velocities.rearRight, 1e-9);
  }

  @Test
  void testCartesianIKGyro90CW() {
    // Forward in global frame; left in robot frame
    var velocities = MecanumDrive.driveCartesianIK(1.0, 0.0, 0.0, Rotation2d.kCCW_Pi_2);
    assertEquals(-1.0, velocities.frontLeft, 1e-9);
    assertEquals(1.0, velocities.frontRight, 1e-9);
    assertEquals(1.0, velocities.rearLeft, 1e-9);
    assertEquals(-1.0, velocities.rearRight, 1e-9);

    // Left in global frame; backward in robot frame
    velocities = MecanumDrive.driveCartesianIK(0.0, -1.0, 0.0, Rotation2d.kCCW_Pi_2);
    assertEquals(-1.0, velocities.frontLeft, 1e-9);
    assertEquals(-1.0, velocities.frontRight, 1e-9);
    assertEquals(-1.0, velocities.rearLeft, 1e-9);
    assertEquals(-1.0, velocities.rearRight, 1e-9);

    // Right in global frame; forward in robot frame
    velocities = MecanumDrive.driveCartesianIK(0.0, 1.0, 0.0, Rotation2d.kCCW_Pi_2);
    assertEquals(1.0, velocities.frontLeft, 1e-9);
    assertEquals(1.0, velocities.frontRight, 1e-9);
    assertEquals(1.0, velocities.rearLeft, 1e-9);
    assertEquals(1.0, velocities.rearRight, 1e-9);

    // Rotate CCW
    velocities = MecanumDrive.driveCartesianIK(0.0, 0.0, -1.0, Rotation2d.kCCW_Pi_2);
    assertEquals(-1.0, velocities.frontLeft, 1e-9);
    assertEquals(1.0, velocities.frontRight, 1e-9);
    assertEquals(-1.0, velocities.rearLeft, 1e-9);
    assertEquals(1.0, velocities.rearRight, 1e-9);

    // Rotate CW
    velocities = MecanumDrive.driveCartesianIK(0.0, 0.0, 1.0, Rotation2d.kCCW_Pi_2);
    assertEquals(1.0, velocities.frontLeft, 1e-9);
    assertEquals(-1.0, velocities.frontRight, 1e-9);
    assertEquals(1.0, velocities.rearLeft, 1e-9);
    assertEquals(-1.0, velocities.rearRight, 1e-9);
  }

  @Test
  void testCartesian() {
    var fl = new MockPWMMotorController();
    var rl = new MockPWMMotorController();
    var fr = new MockPWMMotorController();
    var rr = new MockPWMMotorController();
    var drive =
        new MecanumDrive(fl::setDutyCycle, rl::setDutyCycle, fr::setDutyCycle, rr::setDutyCycle);
    drive.setDeadband(0.0);

    // Forward
    drive.driveCartesian(1.0, 0.0, 0.0);
    assertEquals(1.0, fl.getDutyCycle(), 1e-9);
    assertEquals(1.0, fr.getDutyCycle(), 1e-9);
    assertEquals(1.0, rl.getDutyCycle(), 1e-9);
    assertEquals(1.0, rr.getDutyCycle(), 1e-9);

    // Left
    drive.driveCartesian(0.0, -1.0, 0.0);
    assertEquals(-1.0, fl.getDutyCycle(), 1e-9);
    assertEquals(1.0, fr.getDutyCycle(), 1e-9);
    assertEquals(1.0, rl.getDutyCycle(), 1e-9);
    assertEquals(-1.0, rr.getDutyCycle(), 1e-9);

    // Right
    drive.driveCartesian(0.0, 1.0, 0.0);
    assertEquals(1.0, fl.getDutyCycle(), 1e-9);
    assertEquals(-1.0, fr.getDutyCycle(), 1e-9);
    assertEquals(-1.0, rl.getDutyCycle(), 1e-9);
    assertEquals(1.0, rr.getDutyCycle(), 1e-9);

    // Rotate CCW
    drive.driveCartesian(0.0, 0.0, -1.0);
    assertEquals(-1.0, fl.getDutyCycle(), 1e-9);
    assertEquals(1.0, fr.getDutyCycle(), 1e-9);
    assertEquals(-1.0, rl.getDutyCycle(), 1e-9);
    assertEquals(1.0, rr.getDutyCycle(), 1e-9);

    // Rotate CW
    drive.driveCartesian(0.0, 0.0, 1.0);
    assertEquals(1.0, fl.getDutyCycle(), 1e-9);
    assertEquals(-1.0, fr.getDutyCycle(), 1e-9);
    assertEquals(1.0, rl.getDutyCycle(), 1e-9);
    assertEquals(-1.0, rr.getDutyCycle(), 1e-9);
  }

  @Test
  void testCartesianGyro90CW() {
    var fl = new MockPWMMotorController();
    var rl = new MockPWMMotorController();
    var fr = new MockPWMMotorController();
    var rr = new MockPWMMotorController();
    var drive =
        new MecanumDrive(fl::setDutyCycle, rl::setDutyCycle, fr::setDutyCycle, rr::setDutyCycle);
    drive.setDeadband(0.0);

    // Forward in global frame; left in robot frame
    drive.driveCartesian(1.0, 0.0, 0.0, Rotation2d.kCCW_Pi_2);
    assertEquals(-1.0, fl.getDutyCycle(), 1e-9);
    assertEquals(1.0, fr.getDutyCycle(), 1e-9);
    assertEquals(1.0, rl.getDutyCycle(), 1e-9);
    assertEquals(-1.0, rr.getDutyCycle(), 1e-9);

    // Left in global frame; backward in robot frame
    drive.driveCartesian(0.0, -1.0, 0.0, Rotation2d.kCCW_Pi_2);
    assertEquals(-1.0, fl.getDutyCycle(), 1e-9);
    assertEquals(-1.0, fr.getDutyCycle(), 1e-9);
    assertEquals(-1.0, rl.getDutyCycle(), 1e-9);
    assertEquals(-1.0, rr.getDutyCycle(), 1e-9);

    // Right in global frame; forward in robot frame
    drive.driveCartesian(0.0, 1.0, 0.0, Rotation2d.kCCW_Pi_2);
    assertEquals(1.0, fl.getDutyCycle(), 1e-9);
    assertEquals(1.0, fr.getDutyCycle(), 1e-9);
    assertEquals(1.0, rl.getDutyCycle(), 1e-9);
    assertEquals(1.0, rr.getDutyCycle(), 1e-9);

    // Rotate CCW
    drive.driveCartesian(0.0, 0.0, -1.0, Rotation2d.kCCW_Pi_2);
    assertEquals(-1.0, fl.getDutyCycle(), 1e-9);
    assertEquals(1.0, fr.getDutyCycle(), 1e-9);
    assertEquals(-1.0, rl.getDutyCycle(), 1e-9);
    assertEquals(1.0, rr.getDutyCycle(), 1e-9);

    // Rotate CW
    drive.driveCartesian(0.0, 0.0, 1.0, Rotation2d.kCCW_Pi_2);
    assertEquals(1.0, fl.getDutyCycle(), 1e-9);
    assertEquals(-1.0, fr.getDutyCycle(), 1e-9);
    assertEquals(1.0, rl.getDutyCycle(), 1e-9);
    assertEquals(-1.0, rr.getDutyCycle(), 1e-9);
  }

  @Test
  void testPolar() {
    var fl = new MockPWMMotorController();
    var rl = new MockPWMMotorController();
    var fr = new MockPWMMotorController();
    var rr = new MockPWMMotorController();
    var drive =
        new MecanumDrive(fl::setDutyCycle, rl::setDutyCycle, fr::setDutyCycle, rr::setDutyCycle);
    drive.setDeadband(0.0);

    // Forward
    drive.drivePolar(1.0, Rotation2d.kZero, 0.0);
    assertEquals(1.0, fl.getDutyCycle(), 1e-9);
    assertEquals(1.0, fr.getDutyCycle(), 1e-9);
    assertEquals(1.0, rl.getDutyCycle(), 1e-9);
    assertEquals(1.0, rr.getDutyCycle(), 1e-9);

    // Left
    drive.drivePolar(1.0, Rotation2d.kCW_Pi_2, 0.0);
    assertEquals(-1.0, fl.getDutyCycle(), 1e-9);
    assertEquals(1.0, fr.getDutyCycle(), 1e-9);
    assertEquals(1.0, rl.getDutyCycle(), 1e-9);
    assertEquals(-1.0, rr.getDutyCycle(), 1e-9);

    // Right
    drive.drivePolar(1.0, Rotation2d.kCCW_Pi_2, 0.0);
    assertEquals(1.0, fl.getDutyCycle(), 1e-9);
    assertEquals(-1.0, fr.getDutyCycle(), 1e-9);
    assertEquals(-1.0, rl.getDutyCycle(), 1e-9);
    assertEquals(1.0, rr.getDutyCycle(), 1e-9);

    // Rotate CCW
    drive.drivePolar(0.0, Rotation2d.kZero, -1.0);
    assertEquals(-1.0, fl.getDutyCycle(), 1e-9);
    assertEquals(1.0, fr.getDutyCycle(), 1e-9);
    assertEquals(-1.0, rl.getDutyCycle(), 1e-9);
    assertEquals(1.0, rr.getDutyCycle(), 1e-9);

    // Rotate CW
    drive.drivePolar(0.0, Rotation2d.kZero, 1.0);
    assertEquals(1.0, fl.getDutyCycle(), 1e-9);
    assertEquals(-1.0, fr.getDutyCycle(), 1e-9);
    assertEquals(1.0, rl.getDutyCycle(), 1e-9);
    assertEquals(-1.0, rr.getDutyCycle(), 1e-9);
  }
}
