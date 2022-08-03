// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;
import static edu.wpi.first.wpilibj2.command.CommandGroupBase.requireUngrouped;

import java.util.function.BooleanSupplier;

/**
 * Runs one of two commands, depending on the value of the given condition when this command is
 * initialized. Does not actually schedule the selected command - rather, the command is run through
 * this command; this ensures that the command will behave as expected if used as part of a
 * CommandGroup. Requires the requirements of both commands, again to ensure proper functioning when
 * used in a CommandGroup. If this is undesired, consider using {@link ScheduleCommand}.
 *
 * <p>As this command contains multiple component commands within it, it is technically a command
 * group; the command instances that are passed to it cannot be added to any other groups, or
 * scheduled individually.
 *
 * <p>As a rule, CommandGroups require the union of the requirements of their component commands.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class ConditionalCommand extends CommandBase {
  private final Command m_onTrue;
  private final Command m_onFalse;
  private final BooleanSupplier m_condition;
  private Command m_selectedCommand;

  /**
   * Creates a new ConditionalCommand.
   *
   * @param onTrue the command to run if the condition is true
   * @param onFalse the command to run if the condition is false
   * @param condition the condition to determine which command to run
   */
  public ConditionalCommand(Command onTrue, Command onFalse, BooleanSupplier condition) {
    requireUngrouped(onTrue, onFalse);

    CommandGroupBase.registerGroupedCommands(onTrue, onFalse);

    m_onTrue = onTrue;
    m_onFalse = onFalse;
    m_condition = requireNonNullParam(condition, "condition", "ConditionalCommand");
    m_requirements.addAll(m_onTrue.getRequirements());
    m_requirements.addAll(m_onFalse.getRequirements());
  }

  @Override
  public void initialize() {
    if (m_condition.getAsBoolean()) {
      m_selectedCommand = m_onTrue;
    } else {
      m_selectedCommand = m_onFalse;
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
    return m_onTrue.runsWhenDisabled() && m_onFalse.runsWhenDisabled();
  }
}
