// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.commandsv3;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.Set;
import java.util.function.BooleanSupplier;
import java.util.stream.Collectors;

/**
 * A type of command that runs multiple other commands in parallel. The group will end after all
 * required commands have completed; if no command is explicitly required for completion, then the
 * group will end after any command in the group finishes. Any commands still running when the group
 * has reached its completion condition will be cancelled.
 */
public class ParallelGroup implements Command {
  private final Collection<Command> commands = new LinkedHashSet<>();
  private final Collection<Command> requiredCommands = new HashSet<>();
  private final Set<RequireableResource> requirements = new HashSet<>();
  private final String name;
  private final int priority;
  private RobotDisabledBehavior disabledBehavior;

  /**
   * Creates a new parallel group.
   *
   * @param name The name of the group.
   * @param allCommands All the commands to run in parallel.
   * @param requiredCommands The commands that are required to completed for the group to finish. If
   *     this is empty, then the group will finish when <i>any</i> command in it has completed.
   */
  public ParallelGroup(
      String name, Collection<Command> allCommands, Collection<Command> requiredCommands) {
    requireNonNullParam(name, "name", "ParallelGroup");
    requireNonNullParam(allCommands, "allCommands", "ParallelGroup");
    requireNonNullParam(requiredCommands, "requiredCommands", "ParallelGroup");

    allCommands.forEach(
        c -> {
          requireNonNullParam(c, "allCommands[x]", "ParallelGroup");
        });
    requiredCommands.forEach(
        c -> {
          requireNonNullParam(c, "requiredCommands[x]", "ParallelGroup");
        });

    if (!allCommands.containsAll(requiredCommands)) {
      throw new IllegalArgumentException("Required commands must also be composed");
    }

    for (Command c1 : allCommands) {
      for (Command c2 : allCommands) {
        if (c1 == c2) {
          // Commands can't conflict with themselves
          continue;
        }
        if (c1.conflictsWith(c2)) {
          throw new IllegalArgumentException(
              "Commands in a parallel composition cannot require the same resources. "
                  + c1.name()
                  + " and "
                  + c2.name()
                  + " conflict.");
        }
      }
    }

    this.name = name;
    this.commands.addAll(allCommands);
    this.requiredCommands.addAll(requiredCommands);

    for (var command : allCommands) {
      requirements.addAll(command.requirements());
    }

    this.priority =
        allCommands.stream().mapToInt(Command::priority).max().orElse(Command.DEFAULT_PRIORITY);

    this.disabledBehavior = RobotDisabledBehavior.RunWhileDisabled;
    for (Command command : allCommands) {
      if (command.robotDisabledBehavior() == RobotDisabledBehavior.CancelWhileDisabled) {
        this.disabledBehavior = RobotDisabledBehavior.CancelWhileDisabled;
        break;
      }
    }
  }

  /**
   * Creates a parallel group that runs all the given commands until any of them finish.
   *
   * @param commands the commands to run in parallel
   * @return the group
   */
  public static Builder race(Command... commands) {
    return builder().optional(commands);
  }

  /**
   * Creates a parallel group that runs all the given commands until they all finish. If a command
   * finishes early, its required resources will be idle (uncommanded) until the group completes.
   *
   * @param commands the commands to run in parallel
   * @return the group
   */
  public static Builder all(Command... commands) {
    return builder().requiring(commands);
  }

  @Override
  public void run(Coroutine coroutine) {
    if (requiredCommands.isEmpty()) {
      // No command is explicitly required to complete
      // Schedule everything and wait for the first one to complete
      coroutine.awaitAny(commands);
    } else {
      // At least one command is required to complete
      // Schedule all the non-required commands (the scheduler will automatically cancel them
      // when this group completes), and await completion of all the required commands
      commands.forEach(coroutine.scheduler()::schedule);
      coroutine.awaitAll(requiredCommands);
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
    return disabledBehavior;
  }

  @Override
  public String toString() {
    return "ParallelGroup[name=" + name + "]";
  }

  public static Builder builder() {
    return new Builder();
  }

  public static class Builder {
    private final Set<Command> commands = new LinkedHashSet<>();
    private final Set<Command> requiredCommands = new LinkedHashSet<>();
    private BooleanSupplier endCondition = null;

    /**
     * Adds one or more optional commands to the group. They will not be required to complete for
     * the parallel group to exit, and will be canceled once all required commands have finished.
     *
     * @param commands The optional commands to add to the group
     * @return The builder object, for chaining
     */
    public Builder optional(Command... commands) {
      this.commands.addAll(Arrays.asList(commands));
      return this;
    }

    /**
     * Adds one or more required commands to the group. All required commands will need to complete
     * for the group to exit.
     *
     * @param commands The required commands to add to the group
     * @return The builder object, for chaining
     */
    public Builder requiring(Command... commands) {
      requiredCommands.addAll(Arrays.asList(commands));
      this.commands.addAll(requiredCommands);
      return this;
    }

    /**
     * Forces the group to be a pure race, where the group will finish after the first command in
     * the group completes. All other commands in the group will be canceled.
     *
     * @return The builder object, for chaining
     */
    public Builder racing() {
      requiredCommands.clear();
      return this;
    }

    /**
     * Forces the group to require all its commands to complete, overriding any configured race or
     * deadline behaviors. The group will only exit once every command has completed.
     *
     * @return The builder object, for chaining
     */
    public Builder requireAll() {
      requiredCommands.clear();
      requiredCommands.addAll(commands);
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
    public Builder until(BooleanSupplier condition) {
      endCondition = condition;
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
      var group = new ParallelGroup(name, commands, requiredCommands);
      if (endCondition == null) {
        // No custom end condition, return the group as is
        return group;
      }

      // We have a custom end condition, so we need to wrap the group in a race
      return builder()
          .optional(group, Command.waitingFor(endCondition).named("Until Condition"))
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
          requiredCommands.stream().map(Command::name).collect(Collectors.joining(" & ", "(", ")"));

      var optionalCommands = new LinkedHashSet<>(commands);
      optionalCommands.removeAll(requiredCommands);
      // eg "(CommandA | CommandB | CommandC)"
      String optional =
          optionalCommands.stream().map(Command::name).collect(Collectors.joining(" | ", "(", ")"));

      if (requiredCommands.isEmpty()) {
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
}
