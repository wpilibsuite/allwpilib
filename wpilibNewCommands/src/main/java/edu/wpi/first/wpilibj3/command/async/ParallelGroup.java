package edu.wpi.first.wpilibj3.command.async;

import java.util.Collection;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

public class ParallelGroup implements AsyncCommand {
  private final Collection<AsyncCommand> commands = new LinkedHashSet<>();
  private final Collection<AsyncCommand> requiredCommands = new HashSet<>();
  private final Set<HardwareResource> requirements = new HashSet<>();
  private final AsyncScheduler scheduler;
  private final String name;
  private final int priority;
  private RobotDisabledBehavior disabledBehavior;

  public ParallelGroup(
      String name,
      AsyncScheduler scheduler,
      Collection<AsyncCommand> allCommands,
      Collection<AsyncCommand> requiredCommands) {
    if (!allCommands.containsAll(requiredCommands)) {
      throw new IllegalArgumentException(
          "Required commands must also be composed");
    }

    for (AsyncCommand c1 : allCommands) {
      for (AsyncCommand c2 : allCommands) {
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
    this.scheduler = scheduler;
    this.commands.addAll(allCommands);
    this.requiredCommands.addAll(requiredCommands);

    for (var command : allCommands) {
      requirements.addAll(command.requirements());
    }

    this.priority =
        allCommands
            .stream()
            .mapToInt(AsyncCommand::priority)
            .max()
            .orElse(AsyncCommand.DEFAULT_PRIORITY);

    this.disabledBehavior = RobotDisabledBehavior.RunWhileDisabled;
    for (AsyncCommand command : allCommands) {
      if (command.robotDisabledBehavior() == RobotDisabledBehavior.CancelWhileDisabled) {
        this.disabledBehavior = RobotDisabledBehavior.CancelWhileDisabled;
        break;
      }
    }
  }

  /**
   * Creates a parallel group that runs all the given commands until any of them finish.
   * @param name the name of the group
   * @param commands the commands to run in parallel
   * @return the group
   */
  public static ParallelGroup race(String name, AsyncCommand... commands) {
    return new ParallelGroup(
        name,
        AsyncScheduler.getInstance(),
        List.of(commands),
        Set.of()
    );
  }

  /**
   * Creates a parallel group that runs all the given commands until they all finish. If a command
   * finishes early, its required resources will be idle (uncommanded) until the group completes.
   *
   * @param name the name of the group
   * @param commands the commands to run in parallel
   * @return the group
   */
  public static ParallelGroup all(String name, AsyncCommand... commands) {
    return new ParallelGroup(
        name,
        AsyncScheduler.getInstance(),
        List.of(commands),
        List.of(commands)
    );
  }

  @Override
  public void run() {
    Logger.log(name, "Starting");
    for (AsyncCommand command : commands) {
      Logger.log(name, "Scheduling child command " + command);
      scheduler.scheduleComposed(command, this);
    }
    scheduler.yield();

    while (true) {
      Logger.log(name, "Checking for group completion");
      if (requiredCommands.isEmpty()) {
        // No command is required, so we run until at least one command completes
        Logger.log(name, "Checking if any command has completed yet");

        boolean completed = false;
        for (AsyncCommand c : commands) {
          Logger.log(name, "Child command " + c + " running: " + scheduler.isRunning(c));
          if (!scheduler.isRunning(c)) {
            Logger.log(name, "Command " + c + " completed!");
            completed = true;
            break;
          }
        }
        if (completed) {
          Logger.log(name, "Completed");
          break;
        }
      } else {
        // We run all commands until every required command has completed
        Logger.log(name, "Checking if all required commands have completed yet");
        if (requiredCommands.stream().noneMatch(scheduler::isRunning)) {
          Logger.log(name, "Completed");
          break;
        }
      }

      Logger.log(name, "Yielding to the scheduler");
      scheduler.yield();
    }

    Logger.log(name, "Cancelling composed commands after completion");
    for (AsyncCommand command : commands) {
      scheduler.cancel(command);
    }
  }

  @Override
  public String name() {
    return name;
  }

  @Override
  public Set<HardwareResource> requirements() {
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
}
