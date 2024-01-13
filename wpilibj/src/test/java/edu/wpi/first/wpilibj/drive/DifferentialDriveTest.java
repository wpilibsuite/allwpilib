// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.drive;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.wpilibj.motorcontrol.MockPWMMotorController;
import org.junit.jupiter.api.Test;

@SuppressWarnings("resource")
class DifferentialDriveTest {
  @Test
  void testArcadeDriveIK() {
    // Forward
    var speeds = DifferentialDrive.arcadeDriveIK(1.0, 0.0, false);
    assertEquals(1.0, speeds.left, 1e-9);
    assertEquals(1.0, speeds.right, 1e-9);

    // Forward left turn
    speeds = DifferentialDrive.arcadeDriveIK(0.5, 0.5, false);
    assertEquals(0.0, speeds.left, 1e-9);
    assertEquals(0.5, speeds.right, 1e-9);

    // Forward right turn
    speeds = DifferentialDrive.arcadeDriveIK(0.5, -0.5, false);
    assertEquals(0.5, speeds.left, 1e-9);
    assertEquals(0.0, speeds.right, 1e-9);

    // Backward
    speeds = DifferentialDrive.arcadeDriveIK(-1.0, 0.0, false);
    assertEquals(-1.0, speeds.left, 1e-9);
    assertEquals(-1.0, speeds.right, 1e-9);

    // Backward left turn
    speeds = DifferentialDrive.arcadeDriveIK(-0.5, 0.5, false);
    assertEquals(-0.5, speeds.left, 1e-9);
    assertEquals(0.0, speeds.right, 1e-9);

    // Backward right turn
    speeds = DifferentialDrive.arcadeDriveIK(-0.5, -0.5, false);
    assertEquals(0.0, speeds.left, 1e-9);
    assertEquals(-0.5, speeds.right, 1e-9);

    // Left turn (xSpeed with negative sign)
    speeds = DifferentialDrive.arcadeDriveIK(-0.0, 1.0, false);
    assertEquals(-1.0, speeds.left, 1e-9);
    assertEquals(1.0, speeds.right, 1e-9);

    // Left turn (xSpeed with positive sign)
    speeds = DifferentialDrive.arcadeDriveIK(0.0, 1.0, false);
    assertEquals(-1.0, speeds.left, 1e-9);
    assertEquals(1.0, speeds.right, 1e-9);

    // Right turn (xSpeed with negative sign)
    speeds = DifferentialDrive.arcadeDriveIK(-0.0, -1.0, false);
    assertEquals(1.0, speeds.left, 1e-9);
    assertEquals(-1.0, speeds.right, 1e-9);

    // Right turn (xSpeed with positive sign)
    speeds = DifferentialDrive.arcadeDriveIK(0.0, -1.0, false);
    assertEquals(1.0, speeds.left, 1e-9);
    assertEquals(-1.0, speeds.right, 1e-9);
  }

  @Test
  void testArcadeDriveIKSquared() {
    // Forward
    var speeds = DifferentialDrive.arcadeDriveIK(1.0, 0.0, true);
    assertEquals(1.0, speeds.left, 1e-9);
    assertEquals(1.0, speeds.right, 1e-9);

    // Forward left turn
    speeds = DifferentialDrive.arcadeDriveIK(0.5, 0.5, true);
    assertEquals(0.0, speeds.left, 1e-9);
    assertEquals(0.25, speeds.right, 1e-9);

    // Forward right turn
    speeds = DifferentialDrive.arcadeDriveIK(0.5, -0.5, true);
    assertEquals(0.25, speeds.left, 1e-9);
    assertEquals(0.0, speeds.right, 1e-9);

    // Backward
    speeds = DifferentialDrive.arcadeDriveIK(-1.0, 0.0, true);
    assertEquals(-1.0, speeds.left, 1e-9);
    assertEquals(-1.0, speeds.right, 1e-9);

    // Backward left turn
    speeds = DifferentialDrive.arcadeDriveIK(-0.5, 0.5, true);
    assertEquals(-0.25, speeds.left, 1e-9);
    assertEquals(0.0, speeds.right, 1e-9);

    // Backward right turn
    speeds = DifferentialDrive.arcadeDriveIK(-0.5, -0.5, true);
    assertEquals(0.0, speeds.left, 1e-9);
    assertEquals(-0.25, speeds.right, 1e-9);

    // Left turn (xSpeed with negative sign)
    speeds = DifferentialDrive.arcadeDriveIK(-0.0, 1.0, false);
    assertEquals(-1.0, speeds.left, 1e-9);
    assertEquals(1.0, speeds.right, 1e-9);

    // Left turn (xSpeed with positive sign)
    speeds = DifferentialDrive.arcadeDriveIK(0.0, 1.0, false);
    assertEquals(-1.0, speeds.left, 1e-9);
    assertEquals(1.0, speeds.right, 1e-9);

    // Right turn (xSpeed with negative sign)
    speeds = DifferentialDrive.arcadeDriveIK(-0.0, -1.0, false);
    assertEquals(1.0, speeds.left, 1e-9);
    assertEquals(-1.0, speeds.right, 1e-9);

    // Right turn (xSpeed with positive sign)
    speeds = DifferentialDrive.arcadeDriveIK(0.0, -1.0, false);
    assertEquals(1.0, speeds.left, 1e-9);
    assertEquals(-1.0, speeds.right, 1e-9);
  }

  @Test
  void testCurvatureDriveIK() {
    // Forward
    var speeds = DifferentialDrive.curvatureDriveIK(1.0, 0.0, false);
    assertEquals(1.0, speeds.left, 1e-9);
    assertEquals(1.0, speeds.right, 1e-9);

    // Forward left turn
    speeds = DifferentialDrive.curvatureDriveIK(0.5, 0.5, false);
    assertEquals(0.25, speeds.left, 1e-9);
    assertEquals(0.75, speeds.right, 1e-9);

    // Forward right turn
    speeds = DifferentialDrive.curvatureDriveIK(0.5, -0.5, false);
    assertEquals(0.75, speeds.left, 1e-9);
    assertEquals(0.25, speeds.right, 1e-9);

    // Backward
    speeds = DifferentialDrive.curvatureDriveIK(-1.0, 0.0, false);
    assertEquals(-1.0, speeds.left, 1e-9);
    assertEquals(-1.0, speeds.right, 1e-9);

    // Backward left turn
    speeds = DifferentialDrive.curvatureDriveIK(-0.5, 0.5, false);
    assertEquals(-0.75, speeds.left, 1e-9);
    assertEquals(-0.25, speeds.right, 1e-9);

    // Backward right turn
    speeds = DifferentialDrive.curvatureDriveIK(-0.5, -0.5, false);
    assertEquals(-0.25, speeds.left, 1e-9);
    assertEquals(-0.75, speeds.right, 1e-9);
  }

  @Test
  void testCurvatureDriveIKTurnInPlace() {
    // Forward
    var speeds = DifferentialDrive.curvatureDriveIK(1.0, 0.0, true);
    assertEquals(1.0, speeds.left, 1e-9);
    assertEquals(1.0, speeds.right, 1e-9);

    // Forward left turn
    speeds = DifferentialDrive.curvatureDriveIK(0.5, 0.5, true);
    assertEquals(0.0, speeds.left, 1e-9);
    assertEquals(1.0, speeds.right, 1e-9);

    // Forward right turn
    speeds = DifferentialDrive.curvatureDriveIK(0.5, -0.5, true);
    assertEquals(1.0, speeds.left, 1e-9);
    assertEquals(0.0, speeds.right, 1e-9);

    // Backward
    speeds = DifferentialDrive.curvatureDriveIK(-1.0, 0.0, true);
    assertEquals(-1.0, speeds.left, 1e-9);
    assertEquals(-1.0, speeds.right, 1e-9);

    // Backward left turn
    speeds = DifferentialDrive.curvatureDriveIK(-0.5, 0.5, true);
    assertEquals(-1.0, speeds.left, 1e-9);
    assertEquals(0.0, speeds.right, 1e-9);

    // Backward right turn
    speeds = DifferentialDrive.curvatureDriveIK(-0.5, -0.5, true);
    assertEquals(0.0, speeds.left, 1e-9);
    assertEquals(-1.0, speeds.right, 1e-9);
  }

  @Test
  void testTankDriveIK() {
    // Forward
    var speeds = DifferentialDrive.tankDriveIK(1.0, 1.0, false);
    assertEquals(1.0, speeds.left, 1e-9);
    assertEquals(1.0, speeds.right, 1e-9);

    // Forward left turn
    speeds = DifferentialDrive.tankDriveIK(0.5, 1.0, false);
    assertEquals(0.5, speeds.left, 1e-9);
    assertEquals(1.0, speeds.right, 1e-9);

    // Forward right turn
    speeds = DifferentialDrive.tankDriveIK(1.0, 0.5, false);
    assertEquals(1.0, speeds.left, 1e-9);
    assertEquals(0.5, speeds.right, 1e-9);

    // Backward
    speeds = DifferentialDrive.tankDriveIK(-1.0, -1.0, false);
    assertEquals(-1.0, speeds.left, 1e-9);
    assertEquals(-1.0, speeds.right, 1e-9);

    // Backward left turn
    speeds = DifferentialDrive.tankDriveIK(-0.5, -1.0, false);
    assertEquals(-0.5, speeds.left, 1e-9);
    assertEquals(-1.0, speeds.right, 1e-9);

    // Backward right turn
    speeds = DifferentialDrive.tankDriveIK(-0.5, 1.0, false);
    assertEquals(-0.5, speeds.left, 1e-9);
    assertEquals(1.0, speeds.right, 1e-9);
  }

  @Test
  void testTankDriveIKSquared() {
    // Forward
    var speeds = DifferentialDrive.tankDriveIK(1.0, 1.0, true);
    assertEquals(1.0, speeds.left, 1e-9);
    assertEquals(1.0, speeds.right, 1e-9);

    // Forward left turn
    speeds = DifferentialDrive.tankDriveIK(0.5, 1.0, true);
    assertEquals(0.25, speeds.left, 1e-9);
    assertEquals(1.0, speeds.right, 1e-9);

    // Forward right turn
    speeds = DifferentialDrive.tankDriveIK(1.0, 0.5, true);
    assertEquals(1.0, speeds.left, 1e-9);
    assertEquals(0.25, speeds.right, 1e-9);

    // Backward
    speeds = DifferentialDrive.tankDriveIK(-1.0, -1.0, true);
    assertEquals(-1.0, speeds.left, 1e-9);
    assertEquals(-1.0, speeds.right, 1e-9);

    // Backward left turn
    speeds = DifferentialDrive.tankDriveIK(-0.5, -1.0, true);
    assertEquals(-0.25, speeds.left, 1e-9);
    assertEquals(-1.0, speeds.right, 1e-9);

    // Backward right turn
    speeds = DifferentialDrive.tankDriveIK(-1.0, -0.5, true);
    assertEquals(-1.0, speeds.left, 1e-9);
    assertEquals(-0.25, speeds.right, 1e-9);
  }

  @Test
  void testArcadeDrive() {
    var left = new MockPWMMotorController();
    var right = new MockPWMMotorController();
    var drive = new DifferentialDrive(left::set, right::set);
    drive.setDeadband(0.0);

    // Forward
    drive.arcadeDrive(1.0, 0.0, false);
    assertEquals(1.0, left.get(), 1e-9);
    assertEquals(1.0, right.get(), 1e-9);

    // Forward left turn
    drive.arcadeDrive(0.5, 0.5, false);
    assertEquals(0.0, left.get(), 1e-9);
    assertEquals(0.5, right.get(), 1e-9);

    // Forward right turn
    drive.arcadeDrive(0.5, -0.5, false);
    assertEquals(0.5, left.get(), 1e-9);
    assertEquals(0.0, right.get(), 1e-9);

    // Backward
    drive.arcadeDrive(-1.0, 0.0, false);
    assertEquals(-1.0, left.get(), 1e-9);
    assertEquals(-1.0, right.get(), 1e-9);

    // Backward left turn
    drive.arcadeDrive(-0.5, 0.5, false);
    assertEquals(-0.5, left.get(), 1e-9);
    assertEquals(0.0, right.get(), 1e-9);

    // Backward right turn
    drive.arcadeDrive(-0.5, -0.5, false);
    assertEquals(0.0, left.get(), 1e-9);
    assertEquals(-0.5, right.get(), 1e-9);
  }

  @Test
  void testArcadeDriveSquared() {
    var left = new MockPWMMotorController();
    var right = new MockPWMMotorController();
    var drive = new DifferentialDrive(left::set, right::set);
    drive.setDeadband(0.0);

    // Forward
    drive.arcadeDrive(1.0, 0.0, true);
    assertEquals(1.0, left.get(), 1e-9);
    assertEquals(1.0, right.get(), 1e-9);

    // Forward left turn
    drive.arcadeDrive(0.5, 0.5, true);
    assertEquals(0.0, left.get(), 1e-9);
    assertEquals(0.25, right.get(), 1e-9);

    // Forward right turn
    drive.arcadeDrive(0.5, -0.5, true);
    assertEquals(0.25, left.get(), 1e-9);
    assertEquals(0.0, right.get(), 1e-9);

    // Backward
    drive.arcadeDrive(-1.0, 0.0, true);
    assertEquals(-1.0, left.get(), 1e-9);
    assertEquals(-1.0, right.get(), 1e-9);

    // Backward left turn
    drive.arcadeDrive(-0.5, 0.5, true);
    assertEquals(-0.25, left.get(), 1e-9);
    assertEquals(0.0, right.get(), 1e-9);

    // Backward right turn
    drive.arcadeDrive(-0.5, -0.5, true);
    assertEquals(0.0, left.get(), 1e-9);
    assertEquals(-0.25, right.get(), 1e-9);
  }

  @Test
  void testCurvatureDrive() {
    var left = new MockPWMMotorController();
    var right = new MockPWMMotorController();
    var drive = new DifferentialDrive(left::set, right::set);
    drive.setDeadband(0.0);

    // Forward
    drive.curvatureDrive(1.0, 0.0, false);
    assertEquals(1.0, left.get(), 1e-9);
    assertEquals(1.0, right.get(), 1e-9);

    // Forward left turn
    drive.curvatureDrive(0.5, 0.5, false);
    assertEquals(0.25, left.get(), 1e-9);
    assertEquals(0.75, right.get(), 1e-9);

    // Forward right turn
    drive.curvatureDrive(0.5, -0.5, false);
    assertEquals(0.75, left.get(), 1e-9);
    assertEquals(0.25, right.get(), 1e-9);

    // Backward
    drive.curvatureDrive(-1.0, 0.0, false);
    assertEquals(-1.0, left.get(), 1e-9);
    assertEquals(-1.0, right.get(), 1e-9);

    // Backward left turn
    drive.curvatureDrive(-0.5, 0.5, false);
    assertEquals(-0.75, left.get(), 1e-9);
    assertEquals(-0.25, right.get(), 1e-9);

    // Backward right turn
    drive.curvatureDrive(-0.5, -0.5, false);
    assertEquals(-0.25, left.get(), 1e-9);
    assertEquals(-0.75, right.get(), 1e-9);
  }

  @Test
  void testCurvatureDriveTurnInPlace() {
    var left = new MockPWMMotorController();
    var right = new MockPWMMotorController();
    var drive = new DifferentialDrive(left::set, right::set);
    drive.setDeadband(0.0);

    // Forward
    drive.curvatureDrive(1.0, 0.0, true);
    assertEquals(1.0, left.get(), 1e-9);
    assertEquals(1.0, right.get(), 1e-9);

    // Forward left turn
    drive.curvatureDrive(0.5, 0.5, true);
    assertEquals(0.0, left.get(), 1e-9);
    assertEquals(1.0, right.get(), 1e-9);

    // Forward right turn
    drive.curvatureDrive(0.5, -0.5, true);
    assertEquals(1.0, left.get(), 1e-9);
    assertEquals(0.0, right.get(), 1e-9);

    // Backward
    drive.curvatureDrive(-1.0, 0.0, true);
    assertEquals(-1.0, left.get(), 1e-9);
    assertEquals(-1.0, right.get(), 1e-9);

    // Backward left turn
    drive.curvatureDrive(-0.5, 0.5, true);
    assertEquals(-1.0, left.get(), 1e-9);
    assertEquals(0.0, right.get(), 1e-9);

    // Backward right turn
    drive.curvatureDrive(-0.5, -0.5, true);
    assertEquals(0.0, left.get(), 1e-9);
    assertEquals(-1.0, right.get(), 1e-9);
  }

  @Test
  void testTankDrive() {
    var left = new MockPWMMotorController();
    var right = new MockPWMMotorController();
    var drive = new DifferentialDrive(left::set, right::set);
    drive.setDeadband(0.0);

    // Forward
    drive.tankDrive(1.0, 1.0, false);
    assertEquals(1.0, left.get(), 1e-9);
    assertEquals(1.0, right.get(), 1e-9);

    // Forward left turn
    drive.tankDrive(0.5, 1.0, false);
    assertEquals(0.5, left.get(), 1e-9);
    assertEquals(1.0, right.get(), 1e-9);

    // Forward right turn
    drive.tankDrive(1.0, 0.5, false);
    assertEquals(1.0, left.get(), 1e-9);
    assertEquals(0.5, right.get(), 1e-9);

    // Backward
    drive.tankDrive(-1.0, -1.0, false);
    assertEquals(-1.0, left.get(), 1e-9);
    assertEquals(-1.0, right.get(), 1e-9);

    // Backward left turn
    drive.tankDrive(-0.5, -1.0, false);
    assertEquals(-0.5, left.get(), 1e-9);
    assertEquals(-1.0, right.get(), 1e-9);

    // Backward right turn
    drive.tankDrive(-0.5, 1.0, false);
    assertEquals(-0.5, left.get(), 1e-9);
    assertEquals(1.0, right.get(), 1e-9);
  }

  @Test
  void testTankDriveSquared() {
    var left = new MockPWMMotorController();
    var right = new MockPWMMotorController();
    var drive = new DifferentialDrive(left::set, right::set);
    drive.setDeadband(0.0);

    // Forward
    drive.tankDrive(1.0, 1.0, true);
    assertEquals(1.0, left.get(), 1e-9);
    assertEquals(1.0, right.get(), 1e-9);

    // Forward left turn
    drive.tankDrive(0.5, 1.0, true);
    assertEquals(0.25, left.get(), 1e-9);
    assertEquals(1.0, right.get(), 1e-9);

    // Forward right turn
    drive.tankDrive(1.0, 0.5, true);
    assertEquals(1.0, left.get(), 1e-9);
    assertEquals(0.25, right.get(), 1e-9);

    // Backward
    drive.tankDrive(-1.0, -1.0, true);
    assertEquals(-1.0, left.get(), 1e-9);
    assertEquals(-1.0, right.get(), 1e-9);

    // Backward left turn
    drive.tankDrive(-0.5, -1.0, true);
    assertEquals(-0.25, left.get(), 1e-9);
    assertEquals(-1.0, right.get(), 1e-9);

    // Backward right turn
    drive.tankDrive(-1.0, -0.5, true);
    assertEquals(-1.0, left.get(), 1e-9);
    assertEquals(-0.25, right.get(), 1e-9);
  }
}
