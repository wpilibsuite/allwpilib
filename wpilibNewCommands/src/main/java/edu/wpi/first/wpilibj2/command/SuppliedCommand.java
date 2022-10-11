// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import java.util.Set;
import java.util.function.Supplier;

/**
 * Lazily initializes the command supplied. This can be useful for initializing the command
 * depending on a value known at compile-time without constructing a map for {@link SelectCommand}.
 */
public class SuppliedCommand extends CommandBase {
  private final Supplier<Command> m_supplier;
  protected Command m_selectedCommand;

  /**
   * Creates a new SuppliedCommand.
   *
   * @param supplier a supplier providing the command to run
   * @param requirements the subsystems the selected command might require.
   */
  public SuppliedCommand(Supplier<Command> supplier, Set<Subsystem> requirements) {
    m_supplier = requireNonNullParam(supplier, "supplier", "SelectCommand");

    // requirements are a set and not a vararg parameter to ensure they are passed in
    addRequirements(
        requirements.stream()
            .map(requirement -> requireNonNullParam(requirement, "requirement", "SuppliedCommand"))
            .toArray(Subsystem[]::new));
  }

  @Override
  public final void initialize() {
    m_selectedCommand = m_supplier.get();

    m_selectedCommand.initialize();
  }

  @Override
  public final void execute() {
    m_selectedCommand.execute();
  }

  @Override
  public final void end(boolean interrupted) {
    m_selectedCommand.end(interrupted);
  }

  @Override
  public final boolean isFinished() {
    return m_selectedCommand.isFinished();
  }
}
