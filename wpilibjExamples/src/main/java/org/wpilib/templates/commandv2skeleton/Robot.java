// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.templates.commandv2skeleton;

import org.wpilib.command2.Command;
import org.wpilib.command2.CommandScheduler;
import org.wpilib.command2.Commands;
import org.wpilib.framework.TimedRobot;

public class Robot extends TimedRobot {
  private Command autonomousCommand;

  public Robot() {
    configureBindings();
  }

  private void configureBindings() {}

  @Override
  public void robotPeriodic() {
    CommandScheduler.getInstance().run();
  }

  @Override
  public void disabledStart() {}

  @Override
  public void disabledPeriodic() {}

  @Override
  public void disabledEnd() {}

  @Override
  public void autonomousStart() {
    autonomousCommand = Commands.print("No autonomous command configured");

    CommandScheduler.getInstance().schedule(autonomousCommand);
  }

  @Override
  public void autonomousPeriodic() {}

  @Override
  public void autonomousEnd() {
    if (autonomousCommand != null) {
      autonomousCommand.cancel();
    }
  }

  @Override
  public void teleopStart() {}

  @Override
  public void teleopPeriodic() {}

  @Override
  public void teleopEnd() {}

  @Override
  public void utilityStart() {
    CommandScheduler.getInstance().cancelAll();
  }

  @Override
  public void utilityPeriodic() {}

  @Override
  public void utilityEnd() {}
}
