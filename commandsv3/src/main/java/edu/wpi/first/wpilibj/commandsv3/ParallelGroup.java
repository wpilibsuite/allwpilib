package edu.wpi.first.wpilibj.commandsv3;

import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

public class ParallelGroup implements Command {
  private final Collection<Command> commands = new LinkedHashSet<>();
  private final Collection<Command> requiredCommands = new HashSet<>();
  private final Set<RequireableResource> requirements = new HashSet<>();
  private final String name;
  private final int priority;
  private RobotDisabledBehavior disabledBehavior;

  public ParallelGroup(
      String name, Collection<Command> allCommands, Collection<Command> requiredCommands) {
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
                  + c1
                  + " and "
                  + c2
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
   * @param name the name of the group
   * @param commands the commands to run in parallel
   * @return the group
   */
  public static ParallelGroup race(String name, Command... commands) {
    return new ParallelGroup(name, List.of(commands), Set.of());
  }

  /**
   * Creates a parallel group that runs all the given commands until they all finish. If a command
   * finishes early, its required resources will be idle (uncommanded) until the group completes.
   *
   * @param name the name of the group
   * @param commands the commands to run in parallel
   * @return the group
   */
  public static ParallelGroup all(String name, Command... commands) {
    return new ParallelGroup(name, List.of(commands), List.of(commands));
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
    private final Set<Command> requiredCommands = new HashSet<>();

    public Builder running(Command... commands) {
      this.commands.addAll(Arrays.asList(commands));
      return this;
    }

    public Builder requiring(Command... commands) {
      requiredCommands.addAll(Arrays.asList(commands));
      this.commands.addAll(requiredCommands);
      return this;
    }

    public Builder racing() {
      requiredCommands.clear();
      return this;
    }

    public Builder requireAll() {
      requiredCommands.clear();
      requiredCommands.addAll(commands);
      return this;
    }

    public ParallelGroup named(String name) {
      return new ParallelGroup(name, commands, requiredCommands);
    }
  }
}
