// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.rebuiltcmdv3.opmodes.auto;

import org.wpilib.examples.rebuiltcmdv3.Robot;
import org.wpilib.opmode.Autonomous;
import org.wpilib.opmode.OpMode;

@Autonomous
public class DoNothingAuto implements OpMode {
  /**
   * Creates a new autonomous opmode that does nothing. This constructor is called automatically by
   * the OpModeRobot framework when the program starts up.
   *
   * @param robot The robot instance to control.
   */
  public DoNothingAuto(Robot robot) {
    // Robot mechanisms have idle default commands, so we don't need to do anything special
  }
}
