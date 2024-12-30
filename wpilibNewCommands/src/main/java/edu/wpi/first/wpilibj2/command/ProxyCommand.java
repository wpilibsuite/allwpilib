// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.util.sendable.SendableBuilder;
import java.util.function.Supplier;

/**
 * Schedules a given command when this command is initialized and ends when it ends, but does not
 * directly run it. Use this for including a command in a composition without adding its
 * requirements, <strong>but only if you know what you are doing. If you are unsure, see <a
 * href="https://docs.wpilib.org/en/stable/docs/software/commandbased/command-compositions.html#scheduling-other-commands">the
 * WPILib docs</a> for a complete explanation of proxy semantics.</strong> Do not proxy a command
 * from a subsystem already required by the composition, or else the composition will cancel itself
 * when the proxy is reached. If this command is interrupted, it will cancel the command.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class ProxyCommand extends Command {
  private final Supplier<Command> m_supplier;
  private Command m_command;

  /**
   * Creates a new ProxyCommand that schedules the supplied command when initialized, and ends when
   * it is no longer scheduled. Use this for lazily creating <strong>proxied</strong> commands at
   * runtime. Proxying should only be done to escape from composition requirement semantics, so if
   * only initialization time command construction is needed, use {@link DeferredCommand} instead.
   *
   * @param supplier the command supplier
   * @deprecated This constructor's similarity to {@link DeferredCommand} is confusing and opens
   *     potential footguns for users who do not fully understand the semantics and implications of
   *     proxying, but who simply want runtime construction. Users who do know what they are doing
   *     and need a supplier-constructed proxied command should instead defer a proxy command.
   * @see DeferredCommand
   */
  @Deprecated(since = "2025", forRemoval = true)
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
    Command nullCheckedCommand = requireNonNullParam(command, "command", "ProxyCommand");
    m_supplier = () -> nullCheckedCommand;
    setName("Proxy(" + nullCheckedCommand.getName() + ")");
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
