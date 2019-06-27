/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.logging.Logger;

import org.junit.BeforeClass;
import org.junit.Test;

import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj.drive.MecanumDrive;
import edu.wpi.first.wpilibj.test.AbstractComsSetup;

import static org.junit.Assert.assertEquals;

/**
 * Tests DifferentialDrive and MecanumDrive.
 */
public class DriveTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(DriveTest.class.getName());

  private static MockSpeedController m_rdFrontLeft = new MockSpeedController();
  private static MockSpeedController m_rdRearLeft = new MockSpeedController();
  private static MockSpeedController m_rdFrontRight = new MockSpeedController();
  private static MockSpeedController m_rdRearRight = new MockSpeedController();
  private static MockSpeedController m_frontLeft = new MockSpeedController();
  private static MockSpeedController m_rearLeft = new MockSpeedController();
  private static MockSpeedController m_frontRight = new MockSpeedController();
  private static MockSpeedController m_rearRight = new MockSpeedController();
  private static RobotDrive m_robotDrive =
      new RobotDrive(m_rdFrontLeft, m_rdRearLeft, m_rdFrontRight, m_rdRearRight);
  private static DifferentialDrive m_differentialDrive =
      new DifferentialDrive(m_frontLeft, m_frontRight);
  private static MecanumDrive m_mecanumDrive =
      new MecanumDrive(m_frontLeft, m_rearLeft, m_frontRight, m_rearRight);

  private final double[] m_testJoystickValues = {1.0, 0.9, 0.5, 0.01, 0.0, -0.01, -0.5, -0.9,
                                                 -1.0};
  private final double[] m_testGyroValues = {0, 30, 45, 90, 135, 180, 225, 270, 305, 360, 540,
                                             -45, -90, -135, -180, -225, -270, -305, -360, -540};

  @BeforeClass
  public static void before() {
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
        assertEquals("Left Motor squared didn't match. Left Joystick: " + leftJoystick
            + " Right Joystick: " + rightJoystick + " ", m_rdFrontLeft.get(), m_frontLeft.get(),
            0.01);
        assertEquals("Right Motor squared didn't match. Left Joystick: " + leftJoystick
            + " Right Joystick: " + rightJoystick + " ", m_rdFrontRight.get(), m_frontRight.get(),
            0.01);
      }
    }
  }

  @Test
  public void testTankDrive() {
    for (double leftJoystick : m_testJoystickValues) {
      for (double rightJoystick : m_testJoystickValues) {
        m_robotDrive.tankDrive(leftJoystick, rightJoystick, false);
        m_differentialDrive.tankDrive(leftJoystick, rightJoystick, false);
        assertEquals("Left Motor didn't match. Left Joystick: " + leftJoystick
            + " Right Joystick: " + rightJoystick + " ", m_rdFrontLeft.get(), m_frontLeft.get(),
            0.01);
        assertEquals("Right Motor didn't match. Left Joystick: " + leftJoystick
            + " Right Joystick: " + rightJoystick + " ", m_rdFrontRight.get(), m_frontRight.get(),
            0.01);
      }
    }
  }

  @Test
  public void testArcadeDriveSquared() {
    for (double moveJoystick : m_testJoystickValues) {
      for (double rotateJoystick : m_testJoystickValues) {
        m_robotDrive.arcadeDrive(moveJoystick, rotateJoystick);
        m_differentialDrive.arcadeDrive(moveJoystick, -rotateJoystick);
        assertEquals("Left Motor squared didn't match. Move Joystick: " + moveJoystick
            + " Rotate Joystick: " + rotateJoystick + " ", m_rdFrontLeft.get(), m_frontLeft.get(),
            0.01);
        assertEquals("Right Motor squared didn't match. Move Joystick: " + moveJoystick
            + " Rotate Joystick: " + rotateJoystick + " ", m_rdFrontRight.get(),
            m_frontRight.get(), 0.01);
      }
    }
  }

  @Test
  public void testArcadeDrive() {
    for (double moveJoystick : m_testJoystickValues) {
      for (double rotateJoystick : m_testJoystickValues) {
        m_robotDrive.arcadeDrive(moveJoystick, rotateJoystick, false);
        m_differentialDrive.arcadeDrive(moveJoystick, -rotateJoystick, false);
        assertEquals("Left Motor didn't match. Move Joystick: " + moveJoystick
            + " Rotate Joystick: " + rotateJoystick + " ", m_rdFrontLeft.get(), m_frontLeft.get(),
            0.01);
        assertEquals("Right Motor didn't match. Move Joystick: " + moveJoystick
            + " Rotate Joystick: " + rotateJoystick + " ", m_rdFrontRight.get(),
            m_frontRight.get(), 0.01);
      }
    }
  }

  @Test
  public void testMecanumPolar() {
    System.out.println("magnitudeJoystick, directionJoystick , rotationJoystick, "
        + "m_rdFrontLeft, m_frontLeft, m_rdFrontRight, m_frontRight, m_rdRearLeft, "
        + "m_rearLeft, m_rdRearRight, m_rearRight");
    for (double magnitudeJoystick : m_testJoystickValues) {
      for (double directionJoystick : m_testGyroValues) {
        for (double rotationJoystick : m_testJoystickValues) {
          m_robotDrive.mecanumDrive_Polar(magnitudeJoystick, directionJoystick, rotationJoystick);
          m_mecanumDrive.drivePolar(magnitudeJoystick, directionJoystick, rotationJoystick);
          assertEquals("Left Front Motor didn't match. Magnitude Joystick: " + magnitudeJoystick
              + " Direction Joystick: " + directionJoystick + " RotationJoystick: "
              + rotationJoystick, m_rdFrontLeft.get(), m_frontLeft.get(), 0.01);
          assertEquals("Right Front Motor didn't match. Magnitude Joystick: " + magnitudeJoystick
              + " Direction Joystick: " + directionJoystick + " RotationJoystick: "
              + rotationJoystick, m_rdFrontRight.get(), -m_frontRight.get(), 0.01);
          assertEquals("Left Rear Motor didn't match. Magnitude Joystick: " + magnitudeJoystick
              + " Direction Joystick: " + directionJoystick + " RotationJoystick: "
              + rotationJoystick, m_rdRearLeft.get(), m_rearLeft.get(), 0.01);
          assertEquals("Right Rear Motor didn't match. Magnitude Joystick: " + magnitudeJoystick
              + " Direction Joystick: " + directionJoystick + " RotationJoystick: "
              + rotationJoystick, m_rdRearRight.get(), -m_rearRight.get(), 0.01);
        }
      }
    }
  }

  @Test
  @SuppressWarnings("checkstyle:LocalVariableName")
  public void testMecanumCartesian() {
    for (double x_Joystick : m_testJoystickValues) {
      for (double y_Joystick : m_testJoystickValues) {
        for (double rotationJoystick : m_testJoystickValues) {
          for (double gyroValue : m_testGyroValues) {
            m_robotDrive.mecanumDrive_Cartesian(x_Joystick, y_Joystick, rotationJoystick,
                                                gyroValue);
            m_mecanumDrive.driveCartesian(x_Joystick, -y_Joystick, rotationJoystick, -gyroValue);
            assertEquals("Left Front Motor didn't match. X Joystick: " + x_Joystick
                + " Y Joystick: " + y_Joystick + " RotationJoystick: "
                + rotationJoystick + " Gyro: " + gyroValue, m_rdFrontLeft.get(),
                m_frontLeft.get(), 0.01);
            assertEquals("Right Front Motor didn't match. X Joystick: " + x_Joystick
                + " Y Joystick: " + y_Joystick + " RotationJoystick: "
                + rotationJoystick + " Gyro: " + gyroValue, m_rdFrontRight.get(),
                -m_frontRight.get(), 0.01);
            assertEquals("Left Rear Motor didn't match. X Joystick: " + x_Joystick
                + " Y Joystick: " + y_Joystick + " RotationJoystick: "
                + rotationJoystick + " Gyro: " + gyroValue, m_rdRearLeft.get(),
                m_rearLeft.get(), 0.01);
            assertEquals("Right Rear Motor didn't match. X Joystick: " + x_Joystick
                + " Y Joystick: " + y_Joystick + " RotationJoystick: "
                + rotationJoystick + " Gyro: " + gyroValue, m_rdRearRight.get(),
                -m_rearRight.get(), 0.01);
          }
        }
      }
    }
  }

  @Override
  protected Logger getClassLogger() {
    return logger;
  }
}
