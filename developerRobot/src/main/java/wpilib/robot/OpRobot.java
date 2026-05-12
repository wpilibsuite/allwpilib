// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package wpilib.robot;

import org.wpilib.driverstation.DefaultUserControls;
import org.wpilib.driverstation.UserControlsInstance;
import org.wpilib.framework.OpModeRobot;

/**
 * This is a demo program showing the use of the Expansion Hub motors and servos. The motors and
 * servos are driven using the controllers in the telop opmode, and timed in the auto op mode.
 */
@UserControlsInstance(DefaultUserControls.class)
public class OpRobot extends OpModeRobot {
  /** Called once at the beginning of the robot program. */
  public OpRobot() {}
}
