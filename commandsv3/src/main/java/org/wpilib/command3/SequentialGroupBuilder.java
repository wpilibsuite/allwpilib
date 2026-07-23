// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.Set;
import java.util.function.BooleanSupplier;
import java.util.stream.Collectors;
import org.wpilib.annotation.NoDiscard;

/**
 * A builder class to configure and then create a {@link SequentialGroup}. Like {@link
 * SequentialGroupBuilder}, the final command is created by calling the terminal {@link
 * #named(String)} method, or with an automatically generated name using {@link
 * #withAutomaticName()}.
 */
@NoDiscard
public class SequentialGroupBuilder {
  private final List<Command> m_steps = new ArrayList<>();
  private final Set<Mechanism> m_additionalRequirements = new HashSet<>();
  private BooleanSupplier m_endCondition;
  private boolean m_inheritRequirements = true;

  /**
   * Creates new SequentialGroupBuilder. The builder will have no commands and have no preapplied
   * configuration options. Use {@link #andThen(Command)} to add commands to the sequence.
   */
  public SequentialGroupBuilder() {}

  /**
   * Adds a command to the sequence.
   *
   * @param next The next command in the sequence
   * @return The builder object, for chaining
   */
  public SequentialGroupBuilder andThen(Command next) {
    requireNonNullParam(next, "next", "SequentialGroupBuilder.andThen");

    m_steps.add(next);
    return this;
  }

  /**
   * Adds commands to the sequence. Commands will be added to the sequence in the order they are
   * passed to this method.
   *
   * @param nextCommands The next commands in the sequence
   * @return The builder object, for chaining
   */
  public SequentialGroupBuilder andThen(Command... nextCommands) {
    requireNonNullParam(nextCommands, "nextCommands", "SequentialGroupBuilder.andThen");
    for (int i = 0; i < nextCommands.length; i++) {
      requireNonNullParam(
          nextCommands[i], "nextCommands[" + i + "]", "SequentialGroupBuilder.andThen");
    }

    m_steps.addAll(Arrays.asList(nextCommands));
    return this;
  }

  /**
   * Adds an end condition to the command group. If this condition is met before all required
   * commands have completed, the group will exit early. If multiple end conditions are added (e.g.
   * {@code .until(() -> conditionA()).until(() -> conditionB())}), then the last end condition
   * added will be used and any previously configured condition will be overridden.
   *
   * @param endCondition The end condition for the group
   * @return The builder object, for chaining
   */
  public SequentialGroupBuilder until(BooleanSupplier endCondition) {
    m_endCondition = endCondition;
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
  public SequentialGroupBuilder inheritRequirements(boolean shouldInherit) {
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
  public SequentialGroupBuilder withAdditionalRequirements(Mechanism requirement, Mechanism... extra) {
    requireNonNullParam(requirement, "requirement", "SequentialGroupBuilder.withAdditionalRequirements");
    requireNonNullParam(extra, "extra", "SequentialGroupBuilder.withAdditionalRequirements");

    for (int i = 0; i < extra.length; i++) {
      requireNonNullParam(extra[i], "extra[" + i + "]", "SequentialGroupBuilder.withAdditionalRequirements");
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
  public SequentialGroupBuilder withAdditionalRequirements(Collection<Mechanism> requirements) {

    requireNonNullParam(requirements, "requirements", "SequentialGroupBuilder.withAdditionalRequirements");
    int i = 0;
    for (var mechanism : requirements) {
      requireNonNullParam(mechanism, "requirements[" + i + "]", "SequentialGroupBuilder.withAdditionalRequirements");
      i++;
    }

    m_additionalRequirements.addAll(requirements);
    return this;
  }

  /**
   * Creates the sequence command, giving it the specified name.
   *
   * @param name The name of the sequence command
   * @return The built command
   */
  public Command named(String name) {
    var seq = new SequentialGroup(name, m_steps, m_inheritRequirements, m_additionalRequirements);
    if (m_endCondition == null) {
      // No custom end condition, return the group as is
      return seq;
    }

    // We have a custom end condition, so we need to wrap the group in a race
    return new ParallelGroupBuilder()
        .optional(seq, Command.waitUntil(m_endCondition).named("Until Condition"))
        .named(name);
  }

  /**
   * Creates the sequence command, giving it an automatically generated name based on the commands
   * in the sequence.
   *
   * @return The built command
   */
  public Command withAutomaticName() {
    return named(m_steps.stream().map(Command::name).collect(Collectors.joining(" -> ")));
  }
}
