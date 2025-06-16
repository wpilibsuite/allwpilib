// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import java.util.Arrays;
import java.util.LinkedHashSet;
import java.util.Set;
import java.util.function.BooleanSupplier;
import java.util.stream.Collectors;

/**
 * A builder class to configure and then create a {@link ParallelGroup}. Like
 * {@link CommandBuilder}, the final command is created by calling the terminal
 * {@link #named(String)} method, or with an automatically generated name using
 * {@link #withAutomaticName()}.
 */
public class ParallelGroupBuilder {
  private final Set<Command> m_commands = new LinkedHashSet<>();
  private final Set<Command> m_requiredCommands = new LinkedHashSet<>();
  private BooleanSupplier m_endCondition;

  /**
   * Adds one or more optional commands to the group. They will not be required to complete for
   * the parallel group to exit, and will be canceled once all required commands have finished.
   *
   * @param commands The optional commands to add to the group
   * @return The builder object, for chaining
   */
  public ParallelGroupBuilder optional(Command... commands) {
    this.m_commands.addAll(Arrays.asList(commands));
    return this;
  }

  /**
   * Adds one or more required commands to the group. All required commands will need to complete
   * for the group to exit.
   *
   * @param commands The required commands to add to the group
   * @return The builder object, for chaining
   */
  public ParallelGroupBuilder requiring(Command... commands) {
    m_requiredCommands.addAll(Arrays.asList(commands));
    this.m_commands.addAll(m_requiredCommands);
    return this;
  }

  /**
   * Adds a command to the group. The command must complete for the group to exit.
   *
   * @param command The command to add to the group
   * @return The builder object, for chaining
   */
  // Note: this primarily exists so users can cleanly chain .alongWith calls to build a
  //       parallel group, eg `fooCommand().alongWith(barCommand()).alongWith(bazCommand())`
  public ParallelGroupBuilder alongWith(Command command) {
    return requiring(command);
  }

  /**
   * Forces the group to be a pure race, where the group will finish after the first command in
   * the group completes. All other commands in the group will be canceled.
   *
   * @return The builder object, for chaining
   */
  public ParallelGroupBuilder racing() {
    m_requiredCommands.clear();
    return this;
  }

  /**
   * Forces the group to require all its commands to complete, overriding any configured race or
   * deadline behaviors. The group will only exit once every command has completed.
   *
   * @return The builder object, for chaining
   */
  public ParallelGroupBuilder requireAll() {
    m_requiredCommands.clear();
    m_requiredCommands.addAll(m_commands);
    return this;
  }

  /**
   * Adds an end condition to the command group. If this condition is met before all required
   * commands have completed, the group will exit early. If multiple end conditions are added
   * (e.g. {@code .until(() -> conditionA()).until(() -> conditionB())}), then the last end
   * condition added will be used and any previously configured condition will be overridden.
   *
   * @param condition The end condition for the group
   * @return The builder object, for chaining
   */
  public ParallelGroupBuilder until(BooleanSupplier condition) {
    m_endCondition = condition;
    return this;
  }

  /**
   * Creates the group, using the provided named. The group will require everything that the
   * commands in the group require, and will have a priority level equal to the highest priority
   * among those commands.
   *
   * @param name The name of the parallel group
   * @return The built group
   */
  public ParallelGroup named(String name) {
    var group = new ParallelGroup(name, m_commands, m_requiredCommands);
    if (m_endCondition == null) {
      // No custom end condition, return the group as is
      return group;
    }

    // We have a custom end condition, so we need to wrap the group in a race
    return ParallelGroup.builder()
               .optional(group, Command.waitUntil(m_endCondition).named("Until Condition"))
               .named(name);
  }

  /**
   * Creates the group, giving it a name based on the commands within the group.
   *
   * @return The built group
   */
  public ParallelGroup withAutomaticName() {
    // eg "(CommandA & CommandB & CommandC)"
    String required =
        m_requiredCommands.stream().map(Command::name).collect(Collectors.joining(" & ", "(", ")"));

    var optionalCommands = new LinkedHashSet<>(m_commands);
    optionalCommands.removeAll(m_requiredCommands);
    // eg "(CommandA | CommandB | CommandC)"
    String optional =
        optionalCommands.stream().map(Command::name).collect(Collectors.joining(" | ", "(", ")"));

    if (m_requiredCommands.isEmpty()) {
      // No required commands, pure race
      return named(optional);
    } else if (optionalCommands.isEmpty()) {
      // Everything required
      return named(required);
    } else {
      // Some form of deadline
      // eg "[(CommandA & CommandB) * (CommandX | CommandY | ...)]"
      String name = "[%s * %s]".formatted(required, optional);
      return named(name);
    }
  }
}
