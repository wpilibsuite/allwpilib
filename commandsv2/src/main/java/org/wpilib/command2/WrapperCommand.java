// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import java.util.Set;

/**
 * A class used internally to wrap commands while overriding a specific method; all other methods
 * will call through to the wrapped command.
 *
 * <p>The rules for command compositions apply: command instances that are passed to it cannot be
 * added to any other composition or scheduled individually, and the composition requires all
 * subsystems its components require.
 */
public abstract class WrapperCommand extends Command {
  /** Command being wrapped. */
  protected final Command m_command;

  /**
   * Wrap a command.
   *
   * @param command the command being wrapped. Trying to directly schedule this command or add it to
   *     a composition will throw an exception.
   */
  @SuppressWarnings("this-escape")
  protected WrapperCommand(Command command) {
    CommandScheduler.getInstance().registerComposedCommands(command);
    m_command = command;
    // copy the wrapped command's name
    setName(command.getName());
  }

  /** The initial subroutine of a command. Called once when the command is initially scheduled. */
  @Override
  public void initialize() {
    m_command.initialize();
  }

  /** The main body of a command. Called repeatedly while the command is scheduled. */
  @Override
  public void execute() {
    m_command.execute();
  }

  /**
   * The action to take when the command ends. Called when either the command finishes normally, or
   * when it interrupted/canceled.
   *
   * <p>Do not schedule commands here that share requirements with this command. Use {@link
   * #andThen(Command...)} instead.
   *
   * @param interrupted whether the command was interrupted/canceled
   */
  @Override
  public void end(boolean interrupted) {
    m_command.end(interrupted);
  }

  /**
   * Whether the command has finished. Once a command finishes, the scheduler will call its end()
   * method and un-schedule it.
   *
   * @return whether the command has finished.
   */
  @Override
  public boolean isFinished() {
    return m_command.isFinished();
  }

  /**
   * Specifies the set of subsystems used by this command. Two commands cannot use the same
   * subsystem at the same time. If the command is scheduled as interruptible and another command is
   * scheduled that shares a requirement, the command will be interrupted. Else, the command will
   * not be scheduled. If no subsystems are required, return an empty set.
   *
   * <p>Note: it is recommended that user implementations contain the requirements as a field, and
   * return that field here, rather than allocating a new set every time this is called.
   *
   * @return the set of subsystems that are required
   */
  @Override
  public Set<Subsystem> getRequirements() {
    return m_command.getRequirements();
  }

  /**
   * Whether the given command should run when the robot is disabled. Override to return true if the
   * command should run when disabled.
   *
   * @return whether the command should run when the robot is disabled
   */
  @Override
  public boolean runsWhenDisabled() {
    return m_command.runsWhenDisabled();
  }

  @Override
  public InterruptionBehavior getInterruptionBehavior() {
    return m_command.getInterruptionBehavior();
  }
}
