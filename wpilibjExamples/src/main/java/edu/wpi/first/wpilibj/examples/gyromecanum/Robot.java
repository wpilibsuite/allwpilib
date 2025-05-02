// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.gyromecanum;

import edu.wpi.first.util.sendable.SendableRegistry;
import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.drive.MecanumDrive;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;

/**
 * This is a sample program that uses mecanum drive with a gyro sensor to maintain rotation vectors
 * in relation to the starting orientation of the robot (field-oriented controls).
 */
public class Robot extends TimedRobot {
  // gyro calibration constant, may need to be adjusted;
  // gyro value of 360 is set to correspond to one full revolution
  private static final double VOLTS_PER_DEGREE_PER_SECOND = 0.0128;

  private static final int FRONT_LEFT_CHANNEL = 0;
  private static final int REAR_LEFT_CHANNEL = 1;
  private static final int FRONT_RIGHT_CHANNEL = 2;
  private static final int REAR_RIGHT_CHANNEL = 3;
  private static final int GYRO_PORT = 0;
  private static final int JOYSTICK_PORT = 0;

  private final MecanumDrive m_robotDrive;
  private final AnalogGyro m_gyro = new AnalogGyro(GYRO_PORT);
  private final Joystick m_joystick = new Joystick(JOYSTICK_PORT);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    PWMSparkMax frontLeft = new PWMSparkMax(FRONT_LEFT_CHANNEL);
    PWMSparkMax rearLeft = new PWMSparkMax(REAR_LEFT_CHANNEL);
    PWMSparkMax frontRight = new PWMSparkMax(FRONT_RIGHT_CHANNEL);
    PWMSparkMax rearRight = new PWMSparkMax(REAR_RIGHT_CHANNEL);

    // Invert the right side motors.
    // You may need to change or remove this to match your robot.
    frontRight.setInverted(true);
    rearRight.setInverted(true);

    m_robotDrive = new MecanumDrive(frontLeft::set, rearLeft::set, frontRight::set, rearRight::set);

    m_gyro.setSensitivity(VOLTS_PER_DEGREE_PER_SECOND);

    SendableRegistry.addChild(m_robotDrive, frontLeft);
    SendableRegistry.addChild(m_robotDrive, rearLeft);
    SendableRegistry.addChild(m_robotDrive, frontRight);
    SendableRegistry.addChild(m_robotDrive, rearRight);
  }

  /** Mecanum drive is used with the gyro angle as an input. */
  @Override
  public void teleopPeriodic() {
    m_robotDrive.driveCartesian(
        -m_joystick.getY(), -m_joystick.getX(), -m_joystick.getZ(), m_gyro.getRotation2d());
  }
}
