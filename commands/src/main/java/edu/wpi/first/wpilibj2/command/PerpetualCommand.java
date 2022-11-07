// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.wpilibj2.command.CommandGroupBase.registerGroupedCommands;
import static edu.wpi.first.wpilibj2.command.CommandGroupBase.requireUngrouped;

/**
 * A command that runs another command in perpetuity, ignoring that command's end conditions. While
 * this class does not extend {@link CommandGroupBase}, it is still considered a CommandGroup, as it
 * allows one to compose another command within it; the command instances that are passed to it
 * cannot be added to any other groups, or scheduled individually.
 *
 * <p>As a rule, CommandGroups require the union of the requirements of their component commands.
 *
 * <p>This class is provided by the NewCommands VendorDep
 *
 * @deprecated PerpetualCommand violates the assumption that execute() doesn't get called after
 *     isFinished() returns true -- an assumption that should be valid. This was unsafe/undefined
 *     behavior from the start, and RepeatCommand provides an easy way to achieve similar end
 *     results with slightly different (and safe) semantics.
 */
@Deprecated(forRemoval = true, since = "2023")
public class PerpetualCommand extends CommandBase {
  protected final Command m_command;

  /**
   * Creates a new PerpetualCommand. Will run another command in perpetuity, ignoring that command's
   * end conditions, unless this command itself is interrupted.
   *
   * @param command the command to run perpetually
   */
  public PerpetualCommand(Command command) {
    requireUngrouped(command);
    registerGroupedCommands(command);
    m_command = command;
    m_requirements.addAll(command.getRequirements());
  }

  @Override
  public void initialize() {
    m_command.initialize();
  }

  @Override
  public void execute() {
    m_command.execute();
  }

  @Override
  public void end(boolean interrupted) {
    m_command.end(interrupted);
  }

  @Override
  public boolean runsWhenDisabled() {
    return m_command.runsWhenDisabled();
  }
}
