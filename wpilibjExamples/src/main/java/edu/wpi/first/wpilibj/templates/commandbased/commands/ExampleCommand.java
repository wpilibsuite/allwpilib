/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.templates.commandbased.commands;

import edu.wpi.first.wpilibj.experimental.command.SendableCommandBase;
import edu.wpi.first.wpilibj.templates.commandbased.subsystems.ExampleSubsystem;

/**
 * An example command.  You can replace this with your own command.
 */
public class ExampleCommand extends SendableCommandBase {

  // The subsystem this command runs on.  Should be passed in ("injected") through the command's
  // constructor.
  private ExampleSubsystem m_subsystem;

  public ExampleCommand(ExampleSubsystem subsystem) {
    // Use addRequirements here to declare subsystem dependencies
    addRequirements(subsystem);
    // Inject the subsystem.
    m_subsystem = subsystem;
  }

  /**
   * This method is called once when the command is first scheduled to run.
   */
  @Override
  public void initialize() {
  }

  /**
   * This method is called repeatedly while the command is scheduled to run.
   */
  @Override
  public void execute() {
  }

  /**
   * This method should return true when the command is finished.
   *
   * @return whether the command has finished
   */
  @Override
  public boolean isFinished() {
    return false;
  }

  /**
   * Called when the command ends.
   *
   * @param interrupted whether the command was interrupted/canceled
   */
  @Override
  public void end(boolean interrupted) {
  }
}
