// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.expansionhubsample;

import org.wpilib.driverstation.DefaultUserControls;
import org.wpilib.opmode.PeriodicOpMode;

public class DefaultTeleMode extends PeriodicOpMode {
  private final Robot m_robot;
  private final DefaultUserControls m_userControls;

  public DefaultTeleMode(Robot robot, DefaultUserControls userControls) {
    m_robot = robot;
    m_userControls = userControls;
  }

  @Override
  public void periodic() {
    m_robot.leftMotor.setThrottle(-m_userControls.getGamepad(0).getLeftY());
    m_robot.rightMotor.setThrottle(-m_userControls.getGamepad(0).getRightY());
    m_robot.frontMotor.setThrottle(-m_userControls.getGamepad(0).getLeftX());
    m_robot.backMotor.setThrottle(-m_userControls.getGamepad(0).getRightX());
    m_robot.servo.set(m_userControls.getGamepad(0).getLeftTriggerAxis());
    m_robot.servo2.set(m_userControls.getGamepad(0).getRightTriggerAxis());
  }

}
