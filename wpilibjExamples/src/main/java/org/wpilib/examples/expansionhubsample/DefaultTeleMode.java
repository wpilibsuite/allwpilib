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
    m_robot.m_motor0.setThrottle(-m_userControls.getGamepad(0).getLeftY());
    m_robot.m_motor1.setThrottle(-m_userControls.getGamepad(0).getRightY());
    m_robot.m_motor2.setThrottle(-m_userControls.getGamepad(0).getLeftX());
    m_robot.m_motor3.setThrottle(-m_userControls.getGamepad(0).getRightX());
    m_robot.m_servo0.set(m_userControls.getGamepad(0).getLeftTriggerAxis());
    m_robot.m_servo1.set(m_userControls.getGamepad(0).getRightTriggerAxis());
  }

}
