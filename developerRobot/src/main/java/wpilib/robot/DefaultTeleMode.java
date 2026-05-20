// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package wpilib.robot;

import org.wpilib.driverstation.DefaultUserControls;
import org.wpilib.opmode.PeriodicOpMode;
import org.wpilib.opmode.Teleop;

@Teleop
public class DefaultTeleMode extends PeriodicOpMode {
  @SuppressWarnings("unused")
  private final OpRobot robot;

  @SuppressWarnings("unused")
  private final DefaultUserControls userControls;

  public DefaultTeleMode(OpRobot robot, DefaultUserControls userControls) {
    this.robot = robot;
    this.userControls = userControls;
  }

  @Override
  public void periodic() {}
}
