// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package wpilib.robot;

import org.wpilib.opmode.Autonomous;
import org.wpilib.opmode.PeriodicOpMode;

@Autonomous
public class DefaultAutoMode extends PeriodicOpMode {
  @SuppressWarnings("unused")
  private final OpRobot robot;

  public DefaultAutoMode(OpRobot robot) {
    this.robot = robot;
  }

  @Override
  public void start() {}

  @Override
  public void periodic() {}
}
