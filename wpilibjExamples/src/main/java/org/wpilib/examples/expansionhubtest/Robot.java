// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.expansionhubtest;

import org.wpilib.drive.DifferentialDrive;
import org.wpilib.driverstation.Gamepad;
import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.motor.PWMSparkMax;
import org.wpilib.util.sendable.SendableRegistry;

/**
 * This is a demo program showing the use of the DifferentialDrive class. Runs the motors with split
 * arcade steering and a gamepad.
 */
public class Robot extends TimedRobot {
  private final ExpansionHubMotor m_leftMotor = new ExpansionHubMotor(0, 0);
  private final ExpansionHubMotor m_rightMotor = new ExpansionHubMotor(0, 1);
  private final ExpansionHubMotor m_frontMotor = new ExpansionHubMotor(0, 2);
  private final ExpansionHubMotor m_backMotor = new ExpansionHubMotor(0, 3);

  private final ExpansionHubServo m_servo = new ExpansionHubServo(0, 0);
  private final ExpansionHubServo m_servo2 = new ExpansionHubServo(0, 1);

  private final Gamepad m_driverController = new Gamepad(0);

  /** Called once at the beginning of the robot program. */
  public Robot() {
  }

  @Override
  public void teleopPeriodic() {
    m_leftMotor.setThrottle(-m_driverController.getLeftY());
    m_rightMotor.setThrottle(-m_driverController.getRightY());
    m_frontMotor.setThrottle(-m_driverController.getLeftX());
    m_backMotor.setThrottle(-m_driverController.getRightX());
    m_servo.set(m_driverController.getLeftTrigger());
    m_servo2.set(m_driverController.getRightTrigger());
  }
}
