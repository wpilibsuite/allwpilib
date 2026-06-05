// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.templates.commandv3skeleton;

import org.wpilib.command3.Scheduler;
import org.wpilib.epilogue.Epilogue;
import org.wpilib.epilogue.Logged;
import org.wpilib.framework.OpModeRobot;

@Logged
public class Robot extends OpModeRobot {
  public Robot() {}

  @Override
  public void robotPeriodic() {
    Scheduler.getDefault().run();
    Epilogue.update(this);
  }
}
