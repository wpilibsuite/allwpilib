// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.drive;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.wpilibj.MockSpeedController;
import edu.wpi.first.wpilibj.RobotDrive;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

/** Tests DifferentialDrive and MecanumDrive. */
public class DriveTest {
  private final MockSpeedController m_rdFrontLeft = new MockSpeedController();
  private final MockSpeedController m_rdRearLeft = new MockSpeedController();
  private final MockSpeedController m_rdFrontRight = new MockSpeedController();
  private final MockSpeedController m_rdRearRight = new MockSpeedController();
  private final MockSpeedController m_frontLeft = new MockSpeedController();
  private final MockSpeedController m_rearLeft = new MockSpeedController();
  private final MockSpeedController m_frontRight = new MockSpeedController();
  private final MockSpeedController m_rearRight = new MockSpeedController();
  private final RobotDrive m_robotDrive =
      new RobotDrive(m_rdFrontLeft, m_rdRearLeft, m_rdFrontRight, m_rdRearRight);
  private final DifferentialDrive m_differentialDrive =
      new DifferentialDrive(m_frontLeft, m_frontRight);
  private final MecanumDrive m_mecanumDrive =
      new MecanumDrive(m_frontLeft, m_rearLeft, m_frontRight, m_rearRight);

  private final double[] m_testJoystickValues = {1.0, 0.9, 0.5, 0.01, 0.0, -0.01, -0.5, -0.9, -1.0};
  private final double[] m_testGyroValues = {
    0, 30, 45, 90, 135, 180, 225, 270, 305, 360, 540, -45, -90, -135, -180, -225, -270, -305, -360,
    -540
  };

  @BeforeEach
  void setUp() {
    m_differentialDrive.setDeadband(0.0);
    m_differentialDrive.setSafetyEnabled(false);
    m_mecanumDrive.setDeadband(0.0);
    m_mecanumDrive.setSafetyEnabled(false);
    m_robotDrive.setSafetyEnabled(false);
  }

  @Test
  public void testTankDriveSquared() {
    for (double leftJoystick : m_testJoystickValues) {
      for (double rightJoystick : m_testJoystickValues) {
        m_robotDrive.tankDrive(leftJoystick, rightJoystick);
        m_differentialDrive.tankDrive(leftJoystick, rightJoystick);
        assertEquals(
            m_rdFrontLeft.get(),
            m_frontLeft.get(),
            0.01,
            "Left Motor squared didn't match. Left Joystick: "
                + leftJoystick
                + " Right Joystick: "
                + rightJoystick);
        assertEquals(
            m_rdFrontRight.get(),
            m_frontRight.get(),
            0.01,
            "Right Motor squared didn't match. Left Joystick: "
                + leftJoystick
                + " Right Joystick: "
                + rightJoystick);
      }
    }
  }

  @Test
  void testTankDrive() {
    for (double leftJoystick : m_testJoystickValues) {
      for (double rightJoystick : m_testJoystickValues) {
        m_robotDrive.tankDrive(leftJoystick, rightJoystick, false);
        m_differentialDrive.tankDrive(leftJoystick, rightJoystick, false);
        assertEquals(
            m_rdFrontLeft.get(),
            m_frontLeft.get(),
            0.01,
            "Left Motor didn't match. Left Joystick: "
                + leftJoystick
                + " Right Joystick: "
                + rightJoystick);
        assertEquals(
            m_rdFrontRight.get(),
            m_frontRight.get(),
            0.01,
            "Right Motor didn't match. Left Joystick: "
                + leftJoystick
                + " Right Joystick: "
                + rightJoystick);
      }
    }
  }

  @Test
  void testArcadeDriveSquared() {
    for (double moveJoystick : m_testJoystickValues) {
      for (double rotateJoystick : m_testJoystickValues) {
        m_robotDrive.arcadeDrive(moveJoystick, rotateJoystick);
        m_differentialDrive.arcadeDrive(moveJoystick, -rotateJoystick);
        assertEquals(
            m_rdFrontLeft.get(),
            m_frontLeft.get(),
            0.01,
            "Left Motor squared didn't match. Move Joystick: "
                + moveJoystick
                + " Rotate Joystick: "
                + rotateJoystick);
        assertEquals(
            m_rdFrontRight.get(),
            m_frontRight.get(),
            0.01,
            "Right Motor squared didn't match. Move Joystick: "
                + moveJoystick
                + " Rotate Joystick: "
                + rotateJoystick);
      }
    }
  }

  @Test
  void testArcadeDrive() {
    for (double moveJoystick : m_testJoystickValues) {
      for (double rotateJoystick : m_testJoystickValues) {
        m_robotDrive.arcadeDrive(moveJoystick, rotateJoystick, false);
        m_differentialDrive.arcadeDrive(moveJoystick, -rotateJoystick, false);
        assertEquals(
            m_rdFrontLeft.get(),
            m_frontLeft.get(),
            0.01,
            "Left Motor didn't match. Move Joystick: "
                + moveJoystick
                + " Rotate Joystick: "
                + rotateJoystick);
        assertEquals(
            m_rdFrontRight.get(),
            m_frontRight.get(),
            0.01,
            "Right Motor didn't match. Move Joystick: "
                + moveJoystick
                + " Rotate Joystick: "
                + rotateJoystick);
      }
    }
  }

  @Test
  void testMecanumPolar() {
    for (double magnitudeJoystick : m_testJoystickValues) {
      for (double directionJoystick : m_testGyroValues) {
        for (double rotationJoystick : m_testJoystickValues) {
          m_robotDrive.mecanumDrive_Polar(magnitudeJoystick, directionJoystick, rotationJoystick);
          m_mecanumDrive.drivePolar(magnitudeJoystick, directionJoystick, rotationJoystick);
          assertEquals(
              m_rdFrontLeft.get(),
              m_frontLeft.get(),
              0.01,
              "Left Front Motor didn't match. Magnitude Joystick: "
                  + magnitudeJoystick
                  + " Direction Joystick: "
                  + directionJoystick
                  + " RotationJoystick: "
                  + rotationJoystick);
          assertEquals(
              m_rdFrontRight.get(),
              -m_frontRight.get(),
              0.01,
              "Right Front Motor didn't match. Magnitude Joystick: "
                  + magnitudeJoystick
                  + " Direction Joystick: "
                  + directionJoystick
                  + " RotationJoystick: "
                  + rotationJoystick);
          assertEquals(
              m_rdRearLeft.get(),
              m_rearLeft.get(),
              0.01,
              "Left Rear Motor didn't match. Magnitude Joystick: "
                  + magnitudeJoystick
                  + " Direction Joystick: "
                  + directionJoystick
                  + " RotationJoystick: "
                  + rotationJoystick);
          assertEquals(
              m_rdRearRight.get(),
              -m_rearRight.get(),
              0.01,
              "Right Rear Motor didn't match. Magnitude Joystick: "
                  + magnitudeJoystick
                  + " Direction Joystick: "
                  + directionJoystick
                  + " RotationJoystick: "
                  + rotationJoystick);
        }
      }
    }
  }

  @Test
  @SuppressWarnings("checkstyle:LocalVariableName")
  void testMecanumCartesian() {
    for (double x_Joystick : m_testJoystickValues) {
      for (double y_Joystick : m_testJoystickValues) {
        for (double rotationJoystick : m_testJoystickValues) {
          for (double gyroValue : m_testGyroValues) {
            m_robotDrive.mecanumDrive_Cartesian(
                x_Joystick, y_Joystick, rotationJoystick, gyroValue);
            m_mecanumDrive.driveCartesian(x_Joystick, -y_Joystick, rotationJoystick, -gyroValue);
            assertEquals(
                m_rdFrontLeft.get(),
                m_frontLeft.get(),
                0.01,
                "Left Front Motor didn't match. X Joystick: "
                    + x_Joystick
                    + " Y Joystick: "
                    + y_Joystick
                    + " RotationJoystick: "
                    + rotationJoystick
                    + " Gyro: "
                    + gyroValue);
            assertEquals(
                m_rdFrontRight.get(),
                -m_frontRight.get(),
                0.01,
                "Right Front Motor didn't match. X Joystick: "
                    + x_Joystick
                    + " Y Joystick: "
                    + y_Joystick
                    + " RotationJoystick: "
                    + rotationJoystick
                    + " Gyro: "
                    + gyroValue);
            assertEquals(
                m_rdRearLeft.get(),
                m_rearLeft.get(),
                0.01,
                "Left Rear Motor didn't match. X Joystick: "
                    + x_Joystick
                    + " Y Joystick: "
                    + y_Joystick
                    + " RotationJoystick: "
                    + rotationJoystick
                    + " Gyro: "
                    + gyroValue);
            assertEquals(
                m_rdRearRight.get(),
                -m_rearRight.get(),
                0.01,
                "Right Rear Motor didn't match. X Joystick: "
                    + x_Joystick
                    + " Y Joystick: "
                    + y_Joystick
                    + " RotationJoystick: "
                    + rotationJoystick
                    + " Gyro: "
                    + gyroValue);
          }
        }
      }
    }
  }
}
