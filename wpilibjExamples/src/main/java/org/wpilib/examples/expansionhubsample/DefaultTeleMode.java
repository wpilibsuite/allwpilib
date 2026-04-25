// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.expansionhubsample;

import org.wpilib.driverstation.DefaultUserControls;
import org.wpilib.opmode.PeriodicOpMode;

public class DefaultTeleMode extends PeriodicOpMode {
  private final Robot robot;
  private final DefaultUserControls userControls;

  public DefaultTeleMode(Robot robot, DefaultUserControls userControls) {
    this.robot = robot;
    this.userControls = userControls;
  }

  @Override
  public void periodic() {
    robot.motor0.setThrottle(-userControls.getGamepad(0).getLeftY());
    robot.motor1.setThrottle(-userControls.getGamepad(0).getRightY());
    robot.motor2.setThrottle(-userControls.getGamepad(0).getLeftX());
    robot.motor3.setThrottle(-userControls.getGamepad(0).getRightX());
    robot.servo0.setPosition(userControls.getGamepad(0).getLeftTriggerAxis());
    robot.servo1.setPosition(userControls.getGamepad(0).getRightTriggerAxis());
  }
}
