/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

/**
 * A Command that runs instantly; it will initialize, execute once, and end on the same
 * iteration of the scheduler.  Users can either pass in a Runnable and a set of requirements,
 * or else subclass this command if desired.
 */
public class InstantCommand extends CommandBase {
  private final Runnable m_toRun;

  /**
   * Creates a new InstantCommand that runs the given Runnable with the given requirements.
   *
   * @param toRun        the Runnable to run
   * @param requirements the subsystems required by this command
   */
  public InstantCommand(Runnable toRun, Subsystem... requirements) {
    m_toRun = requireNonNullParam(toRun, "toRun", "InstantCommand");

    addRequirements(requirements);
  }

  /**
   * Creates a new InstantCommand with a Runnable that does nothing.  Useful only as a no-arg
   * constructor to call implicitly from subclass constructors.
   */
  public InstantCommand() {
    m_toRun = () -> {
    };
  }

  @Override
  public void initialize() {
    m_toRun.run();
  }

  @Override
  public final boolean isFinished() {
    return true;
  }
}
