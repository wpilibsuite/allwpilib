package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.Time;
import java.util.Collection;
import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.TimeoutException;
import java.util.stream.Collectors;

public class ParallelGroup implements AsyncCommand {
  private final AsyncScheduler scheduler;
  private final Set<HardwareResource> requirements;
  private final Collection<AsyncCommand> commands;
  private final Set<AsyncCommand> requiredCommands;
  private final String name;
  private final Measure<Time> timeout;
  private final int priority;

  public ParallelGroup(
      AsyncScheduler scheduler,
      Collection<AsyncCommand> commands,
      Collection<AsyncCommand> requiredCommands,
      String name,
      Measure<Time> timeout) {
    this.scheduler = scheduler;
    this.commands = commands;
    this.requiredCommands = Set.copyOf(requiredCommands);
    this.name = name;
    this.requirements =
        commands.stream().flatMap(c -> c.requirements().stream()).collect(Collectors.toSet());
    this.timeout = timeout.copy();

    // Safe to get; can't build a group without any commands
    this.priority = commands.stream().mapToInt(AsyncCommand::priority).max().getAsInt();
  }

  public static Builder onScheduler(AsyncScheduler scheduler) {
    return new Builder(scheduler);
  }

  public static Builder onDefaultScheduler() {
    return onScheduler(AsyncScheduler.getInstance());
  }

  @Override
  public void run() throws Exception {
    try (var scope = new StageScope<>(scheduler, requiredCommands)) {
      for (var command : commands) {
        scope.fork(new NestedCommand(this, command));
      }

      try {
        scope.joinWithTimeout(timeout).throwIfError();
      } catch (TimeoutException e) {
        // just means the maximum execution time was reached
        // If the group is nested in another group or sequence, we need to tell the
        // scheduler to clean it up. The scheduler will only see the nested wrapper command
        scheduler.cancel(this);
      }
    } finally {
      // Cancel any still-running commands in this stage
      // This should only occur if the scope join timed out or
      // [0..N-1] commands are required for a N-sized group
      commands.forEach(scheduler::cancel);
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
  public String toString() {
    return name;
  }

  public static class Builder {
    private final AsyncScheduler scheduler;
    private final Set<AsyncCommand> commands = new HashSet<>();
    private final Set<AsyncCommand> requiredCommands = new HashSet<>();
    private final MutableMeasure<Time> timeout =
        MutableMeasure.ofBaseUnits(Double.MAX_VALUE, Seconds);

    public Builder(AsyncScheduler scheduler) {
      this.scheduler = scheduler;
    }

    public Builder all(AsyncCommand command, AsyncCommand... more) {
      commands.add(command);
      commands.addAll(Set.of(more));
      requiredCommands.addAll(commands);
      return this;
    }

    public Builder racing(AsyncCommand... commands) {
      this.commands.addAll(Set.of(commands));
      requiredCommands.clear();
      return this;
    }

    public Builder requiring(AsyncCommand... required) {
      requiredCommands.addAll(Set.of(required));
      return this;
    }

    public Builder withTimeout(Measure<Time> timeout) {
      this.timeout.mut_replace(timeout);
      return this;
    }

    public ParallelGroup named(String name) {
      // TODO: Confirm no requirement overlaps between commands
      return new ParallelGroup(scheduler, commands, requiredCommands, name, timeout);
    }
  }
}
