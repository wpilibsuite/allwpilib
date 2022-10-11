// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;
import static edu.wpi.first.wpilibj2.command.CommandGroupBase.requireUngrouped;

import java.util.Map;
import java.util.function.Supplier;
import java.util.stream.Collectors;

/**
 * Runs one of a selection of commands, either using a selector and a key to command mapping, or a
 * supplier that returns the command directly at runtime. Does not actually schedule the selected
 * command - rather, the command is run through this command; this ensures that the command will
 * behave as expected if used as part of a CommandGroup. Requires the requirements of all included
 * commands, again to ensure proper functioning when used in a CommandGroup. If this is undesired,
 * consider using {@link ScheduleCommand}.
 *
 * <p>As this command contains multiple component commands within it, it is technically a command
 * group; the command instances that are passed to it cannot be added to any other groups, or
 * scheduled individually.
 *
 * <p>As a rule, CommandGroups require the union of the requirements of their component commands.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class SelectCommand extends SuppliedCommand {
  private final Map<?, Command> m_commands;

  /**
   * Creates a new SelectCommand.
   *
   * @param <T> the type of keys
   * @param commands the map of commands to choose from
   * @param selector the selector to determine which command to run
   */
  public <T> SelectCommand(Map<T, Command> commands, Supplier<T> selector) {
    super(
        () ->
            commands.getOrDefault(
                selector.get(),
                new PrintCommand(
                    "SelectCommand selector value does not correspond to any command!")),
        commands.values().stream()
            .flatMap(command -> command.getRequirements().stream())
            .collect(Collectors.toSet()));

    m_commands = requireNonNullParam(commands, "commands", "SelectCommand");
    requireNonNullParam(selector, "selector", "SelectCommand");

    requireUngrouped(commands.values());
    CommandGroupBase.registerGroupedCommands(commands.values().toArray(Command[]::new));
  }

  @Override
  public boolean runsWhenDisabled() {
    // command runs when disabled only if all options run when disabled
    return m_commands.values().stream().allMatch(Command::runsWhenDisabled);
  }

  @Override
  public InterruptionBehavior getInterruptionBehavior() {
    // command is non-interruptible iff all commands are non-interruptible
    return m_commands.values().stream()
            .map(Command::getInterruptionBehavior)
            .allMatch(InterruptionBehavior.kCancelIncoming::equals)
        ? InterruptionBehavior.kCancelIncoming
        : InterruptionBehavior.kCancelSelf;
  }
}
