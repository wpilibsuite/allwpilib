// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.expansionhubsample;

import org.wpilib.opmode.PeriodicOpMode;
import org.wpilib.system.Timer;

public class DefaultAutoMode extends PeriodicOpMode {
  private final Robot robot;
  private final Timer timer = new Timer();

  public DefaultAutoMode(Robot robot) {
    this.robot = robot;
  }

  @Override
  public void start() {
    timer.reset();
    timer.start();
  }

  @Override
  public void periodic() {
    if (timer.get() < 2.0) {
      robot.motor0.setThrottle(0.5);
      robot.motor1.setThrottle(0.5);
    } else if (timer.get() < 4.0) {
      robot.motor0.setThrottle(0.9);
      robot.motor1.setThrottle(0.9);
    } else {
      robot.motor0.setThrottle(0.0);
      robot.motor1.setThrottle(0.0);
    }
  }
}
