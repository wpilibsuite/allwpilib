// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.gyro;

import org.wpilib.drive.DifferentialDrive;
import org.wpilib.driverstation.Joystick;
import org.wpilib.hardware.imu.OnboardIMU;
import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.opmode.TimedRobot;
import org.wpilib.util.sendable.SendableRegistry;

/**
 * This is a sample program to demonstrate how to use a gyro sensor to make a robot drive straight.
 * This program uses a joystick to drive forwards and backwards while the gyro is used for direction
 * keeping.
 */
public class Robot extends TimedRobot {
  private static final double kAngleSetpoint = 0.0;
  private static final double kP = 0.005; // proportional turning constant

  private static final int kLeftMotorPort = 0;
  private static final int kRightMotorPort = 1;
  private static final OnboardIMU.MountOrientation kIMUMountOrientation =
      OnboardIMU.MountOrientation.kFlat;
  private static final int kJoystickPort = 0;

  private final PWMSparkMax m_leftDrive = new PWMSparkMax(kLeftMotorPort);
  private final PWMSparkMax m_rightDrive = new PWMSparkMax(kRightMotorPort);
  private final DifferentialDrive m_robotDrive =
      new DifferentialDrive(m_leftDrive::set, m_rightDrive::set);
  private final OnboardIMU m_imu = new OnboardIMU(kIMUMountOrientation);
  private final Joystick m_joystick = new Joystick(kJoystickPort);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    SendableRegistry.addChild(m_robotDrive, m_leftDrive);
    SendableRegistry.addChild(m_robotDrive, m_rightDrive);

    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    m_rightDrive.setInverted(true);
  }

  /**
   * The motor speed is set from the joystick while the DifferentialDrive turning value is assigned
   * from the error between the setpoint and the gyro angle.
   */
  @Override
  public void teleopPeriodic() {
    double turningValue = (kAngleSetpoint - m_imu.getRotation2d().getDegrees()) * kP;
    m_robotDrive.arcadeDrive(-m_joystick.getY(), -turningValue);
  }
}
