package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.units.Units.Microsecond;
import static edu.wpi.first.units.Units.Microseconds;
import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.Time;
import java.util.Collection;
import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.ExecutionException;
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
      Collection<AsyncCommand> commands,
      Collection<AsyncCommand> requiredCommands,
      String name,
      Measure<Time> timeout) {
    this.scheduler = new AsyncScheduler();
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
    return new Builder();
  }

  public static Builder onDefaultScheduler() {
    return new Builder();
  }

  @Override
  public void run() throws Exception {
    try (var scope = new StageScope<>(scheduler, requiredCommands)) {
      for (var command : commands) {
        scope.fork(command);
      }

      scope.joinWithTimeout(timeout).throwIfError();
    } catch (TimeoutException e) {
      // just means the maximum execution time was reached
      Logger.log("PARALLEL", name + " timed out after " + timeout.in(Microseconds) + " µs while waiting for required commands to complete");
    } catch (ExecutionException e) {
      if (e.getCause() instanceof InterruptedException) {
        // The commands in the group were interrupted by an interrupt signal to the enclosing parallel group.
        // This is totally fine
        Logger.log("PARALLEL", name + " was interrupted during execution");
      }
      throw e;
    } finally {
      // Cancel any still-running commands in this stage
      // This should only occur if the scope join timed out or
      // [0..N-1] commands are required for a N-sized group
      Logger.log("PARALLEL", name + " - Cancelling still-running commands");

      // Note: cancelAll will wait for each command to be cancelled before returning
      scheduler.cancelAll(commands);
      Logger.log("PARALLEL", name + " - All commands cancelled");
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
    private final Set<AsyncCommand> commands = new HashSet<>();
    private final Set<AsyncCommand> requiredCommands = new HashSet<>();
    private final MutableMeasure<Time> timeout =
        MutableMeasure.ofBaseUnits(Double.MAX_VALUE, Seconds);

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
      return new ParallelGroup(commands, requiredCommands, name, timeout);
    }
  }
}
