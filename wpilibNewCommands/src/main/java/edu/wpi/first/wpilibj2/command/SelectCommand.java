// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import java.util.Map;
import java.util.function.Supplier;

/**
 * A command composition that runs one of a selection of commands, either using a selector and a key
 * to command mapping, or a supplier that returns the command directly at runtime.
 *
 * <p>The rules for command compositions apply: command instances that are passed to it cannot be
 * added to any other composition or scheduled individually, and the composition requires all
 * subsystems its components require.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class SelectCommand extends CommandBase {
  private final Map<Object, Command> m_commands;
  private final Supplier<Object> m_selector;
  private final Supplier<Command> m_toRun;
  private Command m_selectedCommand;
  private boolean m_runsWhenDisabled = true;
  private InterruptionBehavior m_interruptBehavior = InterruptionBehavior.kCancelIncoming;

  /**
   * Creates a new selectcommand.
   *
   * @param commands the map of commands to choose from
   * @param selector the selector to determine which command to run
   */
  public SelectCommand(Map<Object, Command> commands, Supplier<Object> selector) {
    m_commands = requireNonNullParam(commands, "commands", "SelectCommand");
    m_selector = requireNonNullParam(selector, "selector", "SelectCommand");

    CommandScheduler.getInstance()
        .registerComposedCommands(commands.values().toArray(new Command[] {}));

    m_toRun = null;

    for (Command command : m_commands.values()) {
      m_requirements.addAll(command.getRequirements());
      m_runsWhenDisabled &= command.runsWhenDisabled();
      if (command.getInterruptionBehavior() == InterruptionBehavior.kCancelSelf) {
        m_interruptBehavior = InterruptionBehavior.kCancelSelf;
      }
    }
  }

  /**
   * Creates a new selectcommand.
   *
   * @param toRun a supplier providing the command to run
   * @deprecated Replace with {@link ProxyCommand}
   */
  @Deprecated
  public SelectCommand(Supplier<Command> toRun) {
    m_commands = null;
    m_selector = null;
    m_toRun = requireNonNullParam(toRun, "toRun", "SelectCommand");

    // we have no way of checking the underlying command, so default.
    m_runsWhenDisabled = false;
    m_interruptBehavior = InterruptionBehavior.kCancelSelf;
  }

  @Override
  public void initialize() {
    if (m_selector != null) {
      if (!m_commands.keySet().contains(m_selector.get())) {
        m_selectedCommand =
            new PrintCommand(
                "SelectCommand selector value does not correspond to" + " any command!");
        return;
      }
      m_selectedCommand = m_commands.get(m_selector.get());
    } else {
      m_selectedCommand = m_toRun.get();
    }
    m_selectedCommand.initialize();
  }

  @Override
  public void execute() {
    m_selectedCommand.execute();
  }

  @Override
  public void end(boolean interrupted) {
    m_selectedCommand.end(interrupted);
  }

  @Override
  public boolean isFinished() {
    return m_selectedCommand.isFinished();
  }

  @Override
  public boolean runsWhenDisabled() {
    return m_runsWhenDisabled;
  }

  @Override
  public InterruptionBehavior getInterruptionBehavior() {
    return m_interruptBehavior;
  }
}
