// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.commandsv3;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.function.BooleanSupplier;
import java.util.stream.Collectors;

/**
 * A sequence of commands that run one after another. Each successive command only starts after its
 * predecessor completes execution. The priority of a sequence is equal to the highest priority of
 * any of its commands. If all commands in the sequence are able to run while the robot is disabled,
 * then the sequence itself will be allowed to run while the robot is disabled.
 *
 * <p>The sequence will <i>own</i> all resources required by all commands in the sequence for the
 * entire duration of the sequence. This means that a resource owned by one command in the sequence,
 * but not by a later one, will be <i>uncommanded</i> while that later command executes.
 */
public class Sequence implements Command {
  private final String name;
  private final List<Command> commands = new ArrayList<>();
  private final Set<RequireableResource> requirements = new HashSet<>();
  private final int priority;
  private RobotDisabledBehavior robotDisabledBehavior;

  /**
   * Creates a new command sequence.
   *
   * @param name the name of the sequence
   * @param commands the commands to execute within the sequence
   */
  public Sequence(String name, List<Command> commands) {
    this.name = name;
    this.commands.addAll(commands);

    for (var command : commands) {
      this.requirements.addAll(command.requirements());
    }

    this.priority =
        commands.stream().mapToInt(Command::priority).max().orElse(Command.DEFAULT_PRIORITY);

    this.robotDisabledBehavior = RobotDisabledBehavior.RunWhileDisabled;
    for (var command : commands) {
      if (command.robotDisabledBehavior() == RobotDisabledBehavior.CancelWhileDisabled) {
        this.robotDisabledBehavior = RobotDisabledBehavior.CancelWhileDisabled;
        break;
      }
    }
  }

  @Override
  public void run(Coroutine coroutine) {
    for (var command : commands) {
      coroutine.await(command);
    }
  }

  @Override
  public String name() {
    return name;
  }

  @Override
  public Set<RequireableResource> requirements() {
    return requirements;
  }

  @Override
  public int priority() {
    return priority;
  }

  @Override
  public RobotDisabledBehavior robotDisabledBehavior() {
    return robotDisabledBehavior;
  }

  public static Builder builder() {
    return new Builder();
  }

  public static Builder sequence(Command... commands) {
    var builder = new Builder();
    for (var command : commands) {
      builder.andThen(command);
    }
    return builder;
  }

  public static Command of(Command... commands) {
    return sequence(commands).withAutomaticName();
  }

  public static final class Builder {
    private final List<Command> steps = new ArrayList<>();
    private BooleanSupplier endCondition = null;

    /**
     * Adds a command to the sequence.
     *
     * @param next The next command in the sequence
     * @return The builder object, for chaining
     */
    public Builder andThen(Command next) {
      requireNonNullParam(next, "next", "Sequence.Builder.andThen");

      steps.add(next);
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
    public Builder until(BooleanSupplier endCondition) {
      this.endCondition = endCondition;
      return this;
    }

    /**
     * Creates the sequence command, giving it the specified name.
     *
     * @param name The name of the sequence command
     * @return The built command
     */
    public Command named(String name) {
      var seq = new Sequence(name, steps);
      if (endCondition != null) {
        // No custom end condition, return the group as is
        return seq;
      }

      // We have a custom end condition, so we need to wrap the group in a race
      return ParallelGroup.builder()
          .optional(seq, Command.waitingFor(endCondition).named("Until Condition"))
          .named(name);
    }

    /**
     * Creates the sequence command, giving it an automatically generated name based on the commands
     * in the sequence.
     *
     * @return The built command
     */
    public Command withAutomaticName() {
      return named(steps.stream().map(Command::name).collect(Collectors.joining(" -> ")));
    }
  }
}
