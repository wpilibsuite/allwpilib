/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

/**
 * This command will only finish if whatever {@link CommandGroup} it is in has
 * no active children. If it is not a part of a {@link CommandGroup}, then it
 * will finish immediately. If it is itself an active child, then the
 * {@link CommandGroup} will never end.
 *
 * <p>
 * This class is useful for the situation where you want to allow anything
 * running in parallel to finish, before continuing in the main
 * {@link CommandGroup} sequence.
 * </p>
 *$
 * @author Joe Grinstead
 */
public class WaitForChildren extends Command {

  protected void initialize() {}

  protected void execute() {}

  protected void end() {}

  protected void interrupted() {}

  protected boolean isFinished() {
    return getGroup() == null || getGroup().m_children.isEmpty();
  }
}
