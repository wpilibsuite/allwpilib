// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.wpilibj2.command.CommandGroupBase.registerGroupedCommands;
import static edu.wpi.first.wpilibj2.command.CommandGroupBase.requireUngrouped;

/**
 * A command that runs another command repeatedly, restarting it when it ends, until this command is
 * interrupted. While this class does not extend {@link CommandGroupBase}, it is still considered a
 * CommandGroup, as it allows one to compose another command within it; the command instances that
 * are passed to it cannot be added to any other groups, or scheduled individually.
 *
 * <p>As a rule, CommandGroups require the union of the requirements of their component commands.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class RepeatCommand extends CommandBase {
  protected final Command m_command;
  private boolean m_ended;

  /**
   * Creates a new RepeatCommand. Will run another command repeatedly, restarting it whenever it
   * ends, until this command is interrupted.
   *
   * @param command the command to run repeatedly
   */
  public RepeatCommand(Command command) {
    requireUngrouped(command);
    registerGroupedCommands(command);
    m_command = command;
    m_requirements.addAll(command.getRequirements());
  }

  @Override
  public void initialize() {
    m_ended = false;
    m_command.initialize();
  }

  @Override
  public void execute() {
    if (m_ended) {
      m_ended = false;
      m_command.initialize();
    }
    m_command.execute();
    if (m_command.isFinished()) {
      // restart command
      m_command.end(false);
      m_ended = true;
    }
  }

  @Override
  public boolean isFinished() {
    return false;
  }

  @Override
  public void end(boolean interrupted) {
    m_command.end(interrupted);
  }

  @Override
  public boolean runsWhenDisabled() {
    return m_command.runsWhenDisabled();
  }

  @Override
  public InterruptionBehavior getInterruptionBehavior() {
    return m_command.getInterruptionBehavior();
  }
}
