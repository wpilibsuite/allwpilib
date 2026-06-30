// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.Set;
import java.util.function.BooleanSupplier;
import java.util.stream.Collectors;
import org.wpilib.annotation.NoDiscard;

/**
 * A builder class to configure and then create a {@link ParallelGroup}. Like {@link
 * StagedCommandBuilder}, the final command is created by calling the terminal {@link
 * #named(String)} method, or with an automatically generated name using {@link
 * #withAutomaticName()}.
 */
@NoDiscard
public class ParallelGroupBuilder {
  private final Set<Command> m_optionalCommands = new LinkedHashSet<>();
  private final Set<Command> m_requiredCommands = new LinkedHashSet<>();
  private final Set<Mechanism> m_additionalRequirements = new HashSet<>();
  private BooleanSupplier m_endCondition;
  private boolean m_inheritRequirements = true;

  /**
   * Creates a new parallel group builder. The builder will have no commands and have no preapplied
   * configuration options.
   */
  public ParallelGroupBuilder() {}

  /**
   * Adds one or more optional commands to the group. They will not be required to complete for the
   * parallel group to exit, and will be canceled once all required commands have finished.
   *
   * @param commands The optional commands to add to the group
   * @return The builder object, for chaining
   */
  public ParallelGroupBuilder optional(Command... commands) {
    requireNonNullParam(commands, "commands", "ParallelGroupBuilder.optional");
    for (int i = 0; i < commands.length; i++) {
      requireNonNullParam(commands[i], "commands[" + i + "]", "ParallelGroupBuilder.optional");
    }

    m_optionalCommands.addAll(Arrays.asList(commands));
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
    requireNonNullParam(commands, "commands", "ParallelGroupBuilder.requiring");
    for (int i = 0; i < commands.length; i++) {
      requireNonNullParam(commands[i], "commands[" + i + "]", "ParallelGroupBuilder.requiring");
    }

    m_requiredCommands.addAll(Arrays.asList(commands));
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
   * Adds an end condition to the command group. If this condition is met before all required
   * commands have completed, the group will exit early. If multiple end conditions are added (e.g.
   * {@code .until(() -> conditionA()).until(() -> conditionB())}), then the last end condition
   * added will be used and any previously configured condition will be overridden.
   *
   * @param condition The end condition for the group. May be null.
   * @return The builder object, for chaining
   */
  public ParallelGroupBuilder until(BooleanSupplier condition) {
    m_endCondition = condition;
    return this;
  }

  /**
   * Specifies whether the command group should inherit requirements from the subcommands.
   * By default, the command group inherits the requirements from its subcommands, and will require
   * any mechanism used in the group until the entire group finishes. If inheriting is disabled,
   * it allows for mechanisms to return back to their default command, or be used by other commands.
   * 
   * @param shouldInherit Whether the group should inherit the requirements of the subcommands
   * @return The builder object, for chaining
   */
  public ParallelGroupBuilder inheritRequirements(boolean shouldInherit) {
    m_inheritRequirements = shouldInherit;
    return this;
  }

  /**
   * Specifies whether the command group has any additional mechanism requirements. By default,
   * the group will automatically inherit any mechanisms from its subcommands. However, if requirement
   * inheriting is disabled ({@link #inheritRequirements(boolean)}), it can be useful to add
   * requirements for specific mechanisms.
   * 
   * @param requirement The first required mechanism. Cannot be null.
   * @param extra Any optional extra required mechanisms. May be empty, but cannot be null or
   *     contain null values.
   * @return The builder object, for chaining
   */
  public ParallelGroupBuilder withAdditionalRequirements(Mechanism requirement, Mechanism... extra) {
    requireNonNullParam(requirement, "requirement", "ParallelGroupBuilder.withAdditionalRequirements");
    requireNonNullParam(extra, "extra", "ParallelGroupBuilder.withAdditionalRequirements");

    for (int i = 0; i < extra.length; i++) {
      requireNonNullParam(extra[i], "extra[" + i + "]", "ParallelGroupBuilder.withAdditionalRequirements");
    }

    m_additionalRequirements.add(requirement);
    m_additionalRequirements.addAll(Arrays.asList(extra));

    return this;
  }

  /**
   * Specifies whether the command group has any additional mechanism requirements. By default,
   * the group will automatically inherit any mechanisms from its subcommands. However, if requirement
   * inheriting is disabled ({@link #inheritRequirements(boolean)}), it can be useful to add
   * requirements for specific mechanisms.
   * 
   * @param requirements A collection of required mechanisms. May be empty, but cannot be null or
   *     contain null values.
   * @return The builder object, for chaining
   */
  public ParallelGroupBuilder withAdditionalRequirements(Collection<Mechanism> requirements) {
    requireNonNullParam(requirements, "requirements", "ParallelGroupBuilder.withAdditionalRequirements");
    int i = 0;
    for (var mechanism : requirements) {
      requireNonNullParam(mechanism, "requirements[" + i + "]", "ParallelGroupBuilder.withAdditionalRequirements");
      i++;
    }

    m_additionalRequirements.addAll(requirements);
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
    requireNonNullParam(name, "name", "ParallelGroupBuilder.named");

    var group = new ParallelGroup(
      name, m_requiredCommands, m_optionalCommands, m_inheritRequirements, m_additionalRequirements);
    if (m_endCondition == null) {
      // No custom end condition, return the group as is
      return group;
    }

    // We have a custom end condition, so we need to wrap the group in a race
    return new ParallelGroupBuilder()
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

    // eg "(CommandA | CommandB | CommandC)"
    String optional =
        m_optionalCommands.stream().map(Command::name).collect(Collectors.joining(" | ", "(", ")"));

    if (m_requiredCommands.isEmpty()) {
      // No required commands, pure race
      return named(optional);
    } else if (m_optionalCommands.isEmpty()) {
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
