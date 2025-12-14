// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.drive;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.hardware.motor.MockPWMMotorController;

@SuppressWarnings("resource")
class DifferentialDriveTest {
  @Test
  void testArcadeDriveIK() {
    // Forward
    var velocities = DifferentialDrive.arcadeDriveIK(1.0, 0.0, false);
    assertEquals(1.0, velocities.left, 1e-9);
    assertEquals(1.0, velocities.right, 1e-9);

    // Forward left turn
    velocities = DifferentialDrive.arcadeDriveIK(0.5, 0.5, false);
    assertEquals(0.0, velocities.left, 1e-9);
    assertEquals(0.5, velocities.right, 1e-9);

    // Forward right turn
    velocities = DifferentialDrive.arcadeDriveIK(0.5, -0.5, false);
    assertEquals(0.5, velocities.left, 1e-9);
    assertEquals(0.0, velocities.right, 1e-9);

    // Backward
    velocities = DifferentialDrive.arcadeDriveIK(-1.0, 0.0, false);
    assertEquals(-1.0, velocities.left, 1e-9);
    assertEquals(-1.0, velocities.right, 1e-9);

    // Backward left turn
    velocities = DifferentialDrive.arcadeDriveIK(-0.5, 0.5, false);
    assertEquals(-0.5, velocities.left, 1e-9);
    assertEquals(0.0, velocities.right, 1e-9);

    // Backward right turn
    velocities = DifferentialDrive.arcadeDriveIK(-0.5, -0.5, false);
    assertEquals(0.0, velocities.left, 1e-9);
    assertEquals(-0.5, velocities.right, 1e-9);

    // Left turn (xVelocity with negative sign)
    velocities = DifferentialDrive.arcadeDriveIK(-0.0, 1.0, false);
    assertEquals(-1.0, velocities.left, 1e-9);
    assertEquals(1.0, velocities.right, 1e-9);

    // Left turn (xVelocity with positive sign)
    velocities = DifferentialDrive.arcadeDriveIK(0.0, 1.0, false);
    assertEquals(-1.0, velocities.left, 1e-9);
    assertEquals(1.0, velocities.right, 1e-9);

    // Right turn (xVelocity with negative sign)
    velocities = DifferentialDrive.arcadeDriveIK(-0.0, -1.0, false);
    assertEquals(1.0, velocities.left, 1e-9);
    assertEquals(-1.0, velocities.right, 1e-9);

    // Right turn (xVelocity with positive sign)
    velocities = DifferentialDrive.arcadeDriveIK(0.0, -1.0, false);
    assertEquals(1.0, velocities.left, 1e-9);
    assertEquals(-1.0, velocities.right, 1e-9);
  }

  @Test
  void testArcadeDriveIKSquared() {
    // Forward
    var velocities = DifferentialDrive.arcadeDriveIK(1.0, 0.0, true);
    assertEquals(1.0, velocities.left, 1e-9);
    assertEquals(1.0, velocities.right, 1e-9);

    // Forward left turn
    velocities = DifferentialDrive.arcadeDriveIK(0.5, 0.5, true);
    assertEquals(0.0, velocities.left, 1e-9);
    assertEquals(0.25, velocities.right, 1e-9);

    // Forward right turn
    velocities = DifferentialDrive.arcadeDriveIK(0.5, -0.5, true);
    assertEquals(0.25, velocities.left, 1e-9);
    assertEquals(0.0, velocities.right, 1e-9);

    // Backward
    velocities = DifferentialDrive.arcadeDriveIK(-1.0, 0.0, true);
    assertEquals(-1.0, velocities.left, 1e-9);
    assertEquals(-1.0, velocities.right, 1e-9);

    // Backward left turn
    velocities = DifferentialDrive.arcadeDriveIK(-0.5, 0.5, true);
    assertEquals(-0.25, velocities.left, 1e-9);
    assertEquals(0.0, velocities.right, 1e-9);

    // Backward right turn
    velocities = DifferentialDrive.arcadeDriveIK(-0.5, -0.5, true);
    assertEquals(0.0, velocities.left, 1e-9);
    assertEquals(-0.25, velocities.right, 1e-9);

    // Left turn (xVelocity with negative sign)
    velocities = DifferentialDrive.arcadeDriveIK(-0.0, 1.0, false);
    assertEquals(-1.0, velocities.left, 1e-9);
    assertEquals(1.0, velocities.right, 1e-9);

    // Left turn (xVelocity with positive sign)
    velocities = DifferentialDrive.arcadeDriveIK(0.0, 1.0, false);
    assertEquals(-1.0, velocities.left, 1e-9);
    assertEquals(1.0, velocities.right, 1e-9);

    // Right turn (xVelocity with negative sign)
    velocities = DifferentialDrive.arcadeDriveIK(-0.0, -1.0, false);
    assertEquals(1.0, velocities.left, 1e-9);
    assertEquals(-1.0, velocities.right, 1e-9);

    // Right turn (xVelocity with positive sign)
    velocities = DifferentialDrive.arcadeDriveIK(0.0, -1.0, false);
    assertEquals(1.0, velocities.left, 1e-9);
    assertEquals(-1.0, velocities.right, 1e-9);
  }

  @Test
  void testCurvatureDriveIK() {
    // Forward
    var velocities = DifferentialDrive.curvatureDriveIK(1.0, 0.0, false);
    assertEquals(1.0, velocities.left, 1e-9);
    assertEquals(1.0, velocities.right, 1e-9);

    // Forward left turn
    velocities = DifferentialDrive.curvatureDriveIK(0.5, 0.5, false);
    assertEquals(0.25, velocities.left, 1e-9);
    assertEquals(0.75, velocities.right, 1e-9);

    // Forward right turn
    velocities = DifferentialDrive.curvatureDriveIK(0.5, -0.5, false);
    assertEquals(0.75, velocities.left, 1e-9);
    assertEquals(0.25, velocities.right, 1e-9);

    // Backward
    velocities = DifferentialDrive.curvatureDriveIK(-1.0, 0.0, false);
    assertEquals(-1.0, velocities.left, 1e-9);
    assertEquals(-1.0, velocities.right, 1e-9);

    // Backward left turn
    velocities = DifferentialDrive.curvatureDriveIK(-0.5, 0.5, false);
    assertEquals(-0.75, velocities.left, 1e-9);
    assertEquals(-0.25, velocities.right, 1e-9);

    // Backward right turn
    velocities = DifferentialDrive.curvatureDriveIK(-0.5, -0.5, false);
    assertEquals(-0.25, velocities.left, 1e-9);
    assertEquals(-0.75, velocities.right, 1e-9);
  }

  @Test
  void testCurvatureDriveIKTurnInPlace() {
    // Forward
    var velocities = DifferentialDrive.curvatureDriveIK(1.0, 0.0, true);
    assertEquals(1.0, velocities.left, 1e-9);
    assertEquals(1.0, velocities.right, 1e-9);

    // Forward left turn
    velocities = DifferentialDrive.curvatureDriveIK(0.5, 0.5, true);
    assertEquals(0.0, velocities.left, 1e-9);
    assertEquals(1.0, velocities.right, 1e-9);

    // Forward right turn
    velocities = DifferentialDrive.curvatureDriveIK(0.5, -0.5, true);
    assertEquals(1.0, velocities.left, 1e-9);
    assertEquals(0.0, velocities.right, 1e-9);

    // Backward
    velocities = DifferentialDrive.curvatureDriveIK(-1.0, 0.0, true);
    assertEquals(-1.0, velocities.left, 1e-9);
    assertEquals(-1.0, velocities.right, 1e-9);

    // Backward left turn
    velocities = DifferentialDrive.curvatureDriveIK(-0.5, 0.5, true);
    assertEquals(-1.0, velocities.left, 1e-9);
    assertEquals(0.0, velocities.right, 1e-9);

    // Backward right turn
    velocities = DifferentialDrive.curvatureDriveIK(-0.5, -0.5, true);
    assertEquals(0.0, velocities.left, 1e-9);
    assertEquals(-1.0, velocities.right, 1e-9);
  }

  @Test
  void testTankDriveIK() {
    // Forward
    var velocities = DifferentialDrive.tankDriveIK(1.0, 1.0, false);
    assertEquals(1.0, velocities.left, 1e-9);
    assertEquals(1.0, velocities.right, 1e-9);

    // Forward left turn
    velocities = DifferentialDrive.tankDriveIK(0.5, 1.0, false);
    assertEquals(0.5, velocities.left, 1e-9);
    assertEquals(1.0, velocities.right, 1e-9);

    // Forward right turn
    velocities = DifferentialDrive.tankDriveIK(1.0, 0.5, false);
    assertEquals(1.0, velocities.left, 1e-9);
    assertEquals(0.5, velocities.right, 1e-9);

    // Backward
    velocities = DifferentialDrive.tankDriveIK(-1.0, -1.0, false);
    assertEquals(-1.0, velocities.left, 1e-9);
    assertEquals(-1.0, velocities.right, 1e-9);

    // Backward left turn
    velocities = DifferentialDrive.tankDriveIK(-0.5, -1.0, false);
    assertEquals(-0.5, velocities.left, 1e-9);
    assertEquals(-1.0, velocities.right, 1e-9);

    // Backward right turn
    velocities = DifferentialDrive.tankDriveIK(-0.5, 1.0, false);
    assertEquals(-0.5, velocities.left, 1e-9);
    assertEquals(1.0, velocities.right, 1e-9);
  }

  @Test
  void testTankDriveIKSquared() {
    // Forward
    var velocities = DifferentialDrive.tankDriveIK(1.0, 1.0, true);
    assertEquals(1.0, velocities.left, 1e-9);
    assertEquals(1.0, velocities.right, 1e-9);

    // Forward left turn
    velocities = DifferentialDrive.tankDriveIK(0.5, 1.0, true);
    assertEquals(0.25, velocities.left, 1e-9);
    assertEquals(1.0, velocities.right, 1e-9);

    // Forward right turn
    velocities = DifferentialDrive.tankDriveIK(1.0, 0.5, true);
    assertEquals(1.0, velocities.left, 1e-9);
    assertEquals(0.25, velocities.right, 1e-9);

    // Backward
    velocities = DifferentialDrive.tankDriveIK(-1.0, -1.0, true);
    assertEquals(-1.0, velocities.left, 1e-9);
    assertEquals(-1.0, velocities.right, 1e-9);

    // Backward left turn
    velocities = DifferentialDrive.tankDriveIK(-0.5, -1.0, true);
    assertEquals(-0.25, velocities.left, 1e-9);
    assertEquals(-1.0, velocities.right, 1e-9);

    // Backward right turn
    velocities = DifferentialDrive.tankDriveIK(-1.0, -0.5, true);
    assertEquals(-1.0, velocities.left, 1e-9);
    assertEquals(-0.25, velocities.right, 1e-9);
  }

  @Test
  void testArcadeDrive() {
    var left = new MockPWMMotorController();
    var right = new MockPWMMotorController();
    var drive = new DifferentialDrive(left::setDutyCycle, right::setDutyCycle);
    drive.setDeadband(0.0);

    // Forward
    drive.arcadeDrive(1.0, 0.0, false);
    assertEquals(1.0, left.getDutyCycle(), 1e-9);
    assertEquals(1.0, right.getDutyCycle(), 1e-9);

    // Forward left turn
    drive.arcadeDrive(0.5, 0.5, false);
    assertEquals(0.0, left.getDutyCycle(), 1e-9);
    assertEquals(0.5, right.getDutyCycle(), 1e-9);

    // Forward right turn
    drive.arcadeDrive(0.5, -0.5, false);
    assertEquals(0.5, left.getDutyCycle(), 1e-9);
    assertEquals(0.0, right.getDutyCycle(), 1e-9);

    // Backward
    drive.arcadeDrive(-1.0, 0.0, false);
    assertEquals(-1.0, left.getDutyCycle(), 1e-9);
    assertEquals(-1.0, right.getDutyCycle(), 1e-9);

    // Backward left turn
    drive.arcadeDrive(-0.5, 0.5, false);
    assertEquals(-0.5, left.getDutyCycle(), 1e-9);
    assertEquals(0.0, right.getDutyCycle(), 1e-9);

    // Backward right turn
    drive.arcadeDrive(-0.5, -0.5, false);
    assertEquals(0.0, left.getDutyCycle(), 1e-9);
    assertEquals(-0.5, right.getDutyCycle(), 1e-9);
  }

  @Test
  void testArcadeDriveSquared() {
    var left = new MockPWMMotorController();
    var right = new MockPWMMotorController();
    var drive = new DifferentialDrive(left::setDutyCycle, right::setDutyCycle);
    drive.setDeadband(0.0);

    // Forward
    drive.arcadeDrive(1.0, 0.0, true);
    assertEquals(1.0, left.getDutyCycle(), 1e-9);
    assertEquals(1.0, right.getDutyCycle(), 1e-9);

    // Forward left turn
    drive.arcadeDrive(0.5, 0.5, true);
    assertEquals(0.0, left.getDutyCycle(), 1e-9);
    assertEquals(0.25, right.getDutyCycle(), 1e-9);

    // Forward right turn
    drive.arcadeDrive(0.5, -0.5, true);
    assertEquals(0.25, left.getDutyCycle(), 1e-9);
    assertEquals(0.0, right.getDutyCycle(), 1e-9);

    // Backward
    drive.arcadeDrive(-1.0, 0.0, true);
    assertEquals(-1.0, left.getDutyCycle(), 1e-9);
    assertEquals(-1.0, right.getDutyCycle(), 1e-9);

    // Backward left turn
    drive.arcadeDrive(-0.5, 0.5, true);
    assertEquals(-0.25, left.getDutyCycle(), 1e-9);
    assertEquals(0.0, right.getDutyCycle(), 1e-9);

    // Backward right turn
    drive.arcadeDrive(-0.5, -0.5, true);
    assertEquals(0.0, left.getDutyCycle(), 1e-9);
    assertEquals(-0.25, right.getDutyCycle(), 1e-9);
  }

  @Test
  void testCurvatureDrive() {
    var left = new MockPWMMotorController();
    var right = new MockPWMMotorController();
    var drive = new DifferentialDrive(left::setDutyCycle, right::setDutyCycle);
    drive.setDeadband(0.0);

    // Forward
    drive.curvatureDrive(1.0, 0.0, false);
    assertEquals(1.0, left.getDutyCycle(), 1e-9);
    assertEquals(1.0, right.getDutyCycle(), 1e-9);

    // Forward left turn
    drive.curvatureDrive(0.5, 0.5, false);
    assertEquals(0.25, left.getDutyCycle(), 1e-9);
    assertEquals(0.75, right.getDutyCycle(), 1e-9);

    // Forward right turn
    drive.curvatureDrive(0.5, -0.5, false);
    assertEquals(0.75, left.getDutyCycle(), 1e-9);
    assertEquals(0.25, right.getDutyCycle(), 1e-9);

    // Backward
    drive.curvatureDrive(-1.0, 0.0, false);
    assertEquals(-1.0, left.getDutyCycle(), 1e-9);
    assertEquals(-1.0, right.getDutyCycle(), 1e-9);

    // Backward left turn
    drive.curvatureDrive(-0.5, 0.5, false);
    assertEquals(-0.75, left.getDutyCycle(), 1e-9);
    assertEquals(-0.25, right.getDutyCycle(), 1e-9);

    // Backward right turn
    drive.curvatureDrive(-0.5, -0.5, false);
    assertEquals(-0.25, left.getDutyCycle(), 1e-9);
    assertEquals(-0.75, right.getDutyCycle(), 1e-9);
  }

  @Test
  void testCurvatureDriveTurnInPlace() {
    var left = new MockPWMMotorController();
    var right = new MockPWMMotorController();
    var drive = new DifferentialDrive(left::setDutyCycle, right::setDutyCycle);
    drive.setDeadband(0.0);

    // Forward
    drive.curvatureDrive(1.0, 0.0, true);
    assertEquals(1.0, left.getDutyCycle(), 1e-9);
    assertEquals(1.0, right.getDutyCycle(), 1e-9);

    // Forward left turn
    drive.curvatureDrive(0.5, 0.5, true);
    assertEquals(0.0, left.getDutyCycle(), 1e-9);
    assertEquals(1.0, right.getDutyCycle(), 1e-9);

    // Forward right turn
    drive.curvatureDrive(0.5, -0.5, true);
    assertEquals(1.0, left.getDutyCycle(), 1e-9);
    assertEquals(0.0, right.getDutyCycle(), 1e-9);

    // Backward
    drive.curvatureDrive(-1.0, 0.0, true);
    assertEquals(-1.0, left.getDutyCycle(), 1e-9);
    assertEquals(-1.0, right.getDutyCycle(), 1e-9);

    // Backward left turn
    drive.curvatureDrive(-0.5, 0.5, true);
    assertEquals(-1.0, left.getDutyCycle(), 1e-9);
    assertEquals(0.0, right.getDutyCycle(), 1e-9);

    // Backward right turn
    drive.curvatureDrive(-0.5, -0.5, true);
    assertEquals(0.0, left.getDutyCycle(), 1e-9);
    assertEquals(-1.0, right.getDutyCycle(), 1e-9);
  }

  @Test
  void testTankDrive() {
    var left = new MockPWMMotorController();
    var right = new MockPWMMotorController();
    var drive = new DifferentialDrive(left::setDutyCycle, right::setDutyCycle);
    drive.setDeadband(0.0);

    // Forward
    drive.tankDrive(1.0, 1.0, false);
    assertEquals(1.0, left.getDutyCycle(), 1e-9);
    assertEquals(1.0, right.getDutyCycle(), 1e-9);

    // Forward left turn
    drive.tankDrive(0.5, 1.0, false);
    assertEquals(0.5, left.getDutyCycle(), 1e-9);
    assertEquals(1.0, right.getDutyCycle(), 1e-9);

    // Forward right turn
    drive.tankDrive(1.0, 0.5, false);
    assertEquals(1.0, left.getDutyCycle(), 1e-9);
    assertEquals(0.5, right.getDutyCycle(), 1e-9);

    // Backward
    drive.tankDrive(-1.0, -1.0, false);
    assertEquals(-1.0, left.getDutyCycle(), 1e-9);
    assertEquals(-1.0, right.getDutyCycle(), 1e-9);

    // Backward left turn
    drive.tankDrive(-0.5, -1.0, false);
    assertEquals(-0.5, left.getDutyCycle(), 1e-9);
    assertEquals(-1.0, right.getDutyCycle(), 1e-9);

    // Backward right turn
    drive.tankDrive(-0.5, 1.0, false);
    assertEquals(-0.5, left.getDutyCycle(), 1e-9);
    assertEquals(1.0, right.getDutyCycle(), 1e-9);
  }

  @Test
  void testTankDriveSquared() {
    var left = new MockPWMMotorController();
    var right = new MockPWMMotorController();
    var drive = new DifferentialDrive(left::setDutyCycle, right::setDutyCycle);
    drive.setDeadband(0.0);

    // Forward
    drive.tankDrive(1.0, 1.0, true);
    assertEquals(1.0, left.getDutyCycle(), 1e-9);
    assertEquals(1.0, right.getDutyCycle(), 1e-9);

    // Forward left turn
    drive.tankDrive(0.5, 1.0, true);
    assertEquals(0.25, left.getDutyCycle(), 1e-9);
    assertEquals(1.0, right.getDutyCycle(), 1e-9);

    // Forward right turn
    drive.tankDrive(1.0, 0.5, true);
    assertEquals(1.0, left.getDutyCycle(), 1e-9);
    assertEquals(0.25, right.getDutyCycle(), 1e-9);

    // Backward
    drive.tankDrive(-1.0, -1.0, true);
    assertEquals(-1.0, left.getDutyCycle(), 1e-9);
    assertEquals(-1.0, right.getDutyCycle(), 1e-9);

    // Backward left turn
    drive.tankDrive(-0.5, -1.0, true);
    assertEquals(-0.25, left.getDutyCycle(), 1e-9);
    assertEquals(-1.0, right.getDutyCycle(), 1e-9);

    // Backward right turn
    drive.tankDrive(-1.0, -0.5, true);
    assertEquals(-1.0, left.getDutyCycle(), 1e-9);
    assertEquals(-0.25, right.getDutyCycle(), 1e-9);
  }
}
