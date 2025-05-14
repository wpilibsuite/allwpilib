// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.gyro;

import edu.wpi.first.util.sendable.SendableRegistry;
import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.Joystick;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.drive.DifferentialDrive;
import edu.wpi.first.wpilibj.motorcontrol.PWMSparkMax;

/**
 * This is a sample program to demonstrate how to use a gyro sensor to make a robot drive straight.
 * This program uses a joystick to drive forwards and backwards while the gyro is used for direction
 * keeping.
 */
public class Robot extends TimedRobot {
  private static final double ANGLE_SETPOINT = 0.0;
  private static final double P = 0.005; // proportional turning constant

  // gyro calibration constant, may need to be adjusted;
  // gyro value of 360 is set to correspond to one full revolution
  private static final double VOLTS_PER_DEGREE_PER_SECOND = 0.0128;

  private static final int LEFT_MOTOR_PORT = 0;
  private static final int RIGHT_MOTOR_PORT = 1;
  private static final int GYRO_PORT = 0;
  private static final int JOYSTICK_PORT = 0;

  private final PWMSparkMax m_leftDrive = new PWMSparkMax(LEFT_MOTOR_PORT);
  private final PWMSparkMax m_rightDrive = new PWMSparkMax(RIGHT_MOTOR_PORT);
  private final DifferentialDrive m_robotDrive =
      new DifferentialDrive(m_leftDrive::set, m_rightDrive::set);
  private final AnalogGyro m_gyro = new AnalogGyro(GYRO_PORT);
  private final Joystick m_joystick = new Joystick(JOYSTICK_PORT);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    SendableRegistry.addChild(m_robotDrive, m_leftDrive);
    SendableRegistry.addChild(m_robotDrive, m_rightDrive);

    m_gyro.setSensitivity(VOLTS_PER_DEGREE_PER_SECOND);
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
    double turningValue = (ANGLE_SETPOINT - m_gyro.getAngle()) * P;
    m_robotDrive.arcadeDrive(-m_joystick.getY(), -turningValue);
  }
}
