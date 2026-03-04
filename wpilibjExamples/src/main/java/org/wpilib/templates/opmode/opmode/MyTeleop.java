// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.templates.opmode.opmode;

import org.wpilib.opmode.OpMode;
import org.wpilib.opmode.Teleop;
import org.wpilib.templates.opmode.Robot;

@Teleop
public class MyTeleop implements OpMode {
  private final Robot m_robot;

  /** The Robot instance is passed into the opmode via the constructor. */
  public MyTeleop(Robot robot) {
    m_robot = robot;
  }

  @Override
  public void disabledPeriodic() {
    /* Called periodically (on every DS packet) while the robot is disabled. */
  }

  @Override
  public void opModeStart() {
    /* Called once when the robot is enabled. */
  }

  @Override
  public void opModePeriodic() {
    /* Called periodically (set time interval) while the robot is enabled. */
  }

  @Override
  public void opModeStop() {
    /* Called when the robot is disabled (after previously being enabled). */
  }

  @Override
  public void opModeClose() {
    /* Called when the opmode is de-selected / no additional methods will be called. */
  }
}
