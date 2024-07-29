// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.util.sendable.SendableBuilder;
import java.util.Set;
import java.util.function.Supplier;

/**
 * Defers Command construction to runtime. Runs the command returned by a supplier when this command
 * is initialized, and ends when it ends. Useful for performing runtime tasks before creating a new
 * command. If this command is interrupted, it will cancel the command.
 *
 * <p>Note that the supplier <i>must</i> create a new Command each call. For selecting one of a
 * preallocated set of commands, use {@link SelectCommand}.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class DeferredCommand extends Command {
  private final Command m_nullCommand =
      new PrintCommand("[DeferredCommand] Supplied command was null!");

  private final Supplier<Command> m_supplier;
  private Command m_command = m_nullCommand;

  /**
   * Creates a new DeferredCommand that directly runs the supplied command when initialized, and
   * ends when it ends. Useful for lazily creating commands when the DeferredCommand is initialized,
   * such as if the supplied command depends on runtime state. The {@link Supplier} will be called
   * each time this command is initialized. The Supplier <i>must</i> create a new Command each call.
   *
   * @param supplier The command supplier
   * @param requirements The command requirements. This is a {@link Set} to prevent accidental
   *     omission of command requirements. Use {@link Set#of()} to easily construct a requirement
   *     set.
   */
  @SuppressWarnings("this-escape")
  public DeferredCommand(Supplier<Command> supplier, Set<Subsystem> requirements) {
    m_supplier = requireNonNullParam(supplier, "supplier", "DeferredCommand");
    addRequirements(requirements.toArray(new Subsystem[0]));
  }

  @Override
  public void initialize() {
    var cmd = m_supplier.get();
    if (cmd != null) {
      m_command = cmd;
      CommandScheduler.getInstance().registerComposedCommands(m_command);
    }
    m_command.initialize();
  }

  @Override
  public void execute() {
    m_command.execute();
  }

  @Override
  public boolean isFinished() {
    return m_command.isFinished();
  }

  @Override
  public void end(boolean interrupted) {
    m_command.end(interrupted);
    m_command = m_nullCommand;
  }

  @Override
  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  public void initSendable(SendableBuilder builder) {
    super.initSendable(builder);
    builder.addStringProperty(
        "deferred", () -> m_command == m_nullCommand ? "null" : m_command.getName(), null);
  }
}
