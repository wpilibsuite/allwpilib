// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.util.sendable.SendableBuilder;

/**
 * A command that runs another command repeatedly, restarting it when it ends, until this command is
 * interrupted. Command instances that are passed to it cannot be added to any other groups, or
 * scheduled individually.
 *
 * <p>The rules for command compositions apply: command instances that are passed to it cannot be
 * added to any other composition or scheduled individually,and the composition requires all
 * subsystems its components require.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class RepeatCommand extends Command {
  private final Command m_command;
  private boolean m_ended;

  /**
   * Creates a new RepeatCommand. Will run another command repeatedly, restarting it whenever it
   * ends, until this command is interrupted.
   *
   * @param command the command to run repeatedly
   */
  @SuppressWarnings("this-escape")
  public RepeatCommand(Command command) {
    m_command = requireNonNullParam(command, "command", "RepeatCommand");
    CommandScheduler.getInstance().registerComposedCommands(command);
    addRequirements(command.getRequirements());
    setName("Repeat(" + command.getName() + ")");
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
    // Make sure we didn't already call end() (which would happen if the command finished in the
    // last call to our execute())
    if (!m_ended) {
      m_command.end(interrupted);
      m_ended = true;
    }
  }

  @Override
  public boolean runsWhenDisabled() {
    return m_command.runsWhenDisabled();
  }

  @Override
  public InterruptionBehavior getInterruptionBehavior() {
    return m_command.getInterruptionBehavior();
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    super.initSendable(builder);
    builder.addStringProperty("command", m_command::getName, null);
  }
}
