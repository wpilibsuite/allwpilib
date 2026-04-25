// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.expansionhubsample;

import org.wpilib.opmode.PeriodicOpMode;
import org.wpilib.system.Timer;

public class DefaultAutoMode extends PeriodicOpMode {
  private final Robot m_robot;
  private final Timer m_timer = new Timer();

  public DefaultAutoMode(Robot robot) {
    m_robot = robot;
  }

  @Override
  public void start() {
    m_timer.reset();
    m_timer.start();
  }

  @Override
  public void periodic() {
    if (m_timer.get() < 2.0) {
      m_robot.m_motor0.setThrottle(0.5);
      m_robot.m_motor1.setThrottle(0.5);
    } else if (m_timer.get() < 4.0) {
      m_robot.m_motor0.setThrottle(0.9);
      m_robot.m_motor1.setThrottle(0.9);
    } else {
      m_robot.m_motor0.setThrottle(0.0);
      m_robot.m_motor1.setThrottle(0.0);
    }
  }
}
