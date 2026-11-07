// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.expansionhubsample;

import org.wpilib.driverstation.DriverStation;
import org.wpilib.driverstation.Gamepad;
import org.wpilib.opmode.PeriodicOpMode;
import org.wpilib.opmode.Teleop;

@Teleop
public class DefaultTeleMode extends PeriodicOpMode {
  private final Robot robot;
  private final Gamepad gamepad;

  public DefaultTeleMode(Robot robot) {
    this.robot = robot;
    this.gamepad = DriverStation.getGamepad(0);
  }

  @Override
  public void periodic() {
    robot.motor0.setThrottle(-gamepad.getLeftY());
    robot.motor1.setThrottle(-gamepad.getRightY());
    robot.motor2.setThrottle(-gamepad.getLeftX());
    robot.motor3.setThrottle(-gamepad.getRightX());
    robot.servo0.setPosition(gamepad.getLeftTrigger());
    robot.servo1.setPosition(gamepad.getRightTrigger());
  }
}
