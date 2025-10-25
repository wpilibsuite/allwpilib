// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot.opmode;

import edu.wpi.first.wpilibj.opmode.Autonomous;
import edu.wpi.first.wpilibj.opmode.PeriodicOpMode;
import frc.robot.Robot;

@Autonomous(name = "My Auto", group = "Group 1")
public class MyAuto extends PeriodicOpMode {
  private final Robot m_robot;

  /** Can pass the Robot instance into an opmode via the constructor. */
  public MyAuto(Robot robot) {
    m_robot = robot;
  }

  @Override
  public void periodic() {}
}
