// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.util.sendable.SendableBuilder;
import java.util.function.Supplier;

/**
 * Schedules the given command when this command is initialized, and ends when it ends. Useful for
 * forking off from CommandGroups. If this command is interrupted, it will cancel the command.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class ProxyCommand extends Command {
  private final Supplier<Command> m_supplier;
  private Command m_command;

  /**
   * Creates a new ProxyCommand that schedules the supplied command when initialized, and ends when
   * it is no longer scheduled. Useful for lazily creating commands at runtime.
   *
   * @param supplier the command supplier
   */
  public ProxyCommand(Supplier<Command> supplier) {
    m_supplier = requireNonNullParam(supplier, "supplier", "ProxyCommand");
  }

  /**
   * Creates a new ProxyCommand that schedules the given command when initialized, and ends when it
   * is no longer scheduled.
   *
   * @param command the command to run by proxy
   */
  @SuppressWarnings("this-escape")
  public ProxyCommand(Command command) {
    this(() -> command);
    setName("Proxy(" + command.getName() + ")");
  }

  @Override
  public void initialize() {
    m_command = m_supplier.get();
    m_command.schedule();
  }

  @Override
  public void end(boolean interrupted) {
    if (interrupted) {
      m_command.cancel();
    }
    m_command = null;
  }

  @Override
  public void execute() {}

  @Override
  public boolean isFinished() {
    // because we're between `initialize` and `end`, `m_command` is necessarily not null
    // but if called otherwise and m_command is null,
    // it's UB, so we can do whatever we want -- like return true.
    return m_command == null || !m_command.isScheduled();
  }

  /**
   * Whether the given command should run when the robot is disabled. Override to return true if the
   * command should run when disabled.
   *
   * @return true. Otherwise, this proxy would cancel commands that do run when disabled.
   */
  @Override
  public boolean runsWhenDisabled() {
    return true;
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    super.initSendable(builder);
    builder.addStringProperty(
        "proxied", () -> m_command == null ? "null" : m_command.getName(), null);
  }
}
