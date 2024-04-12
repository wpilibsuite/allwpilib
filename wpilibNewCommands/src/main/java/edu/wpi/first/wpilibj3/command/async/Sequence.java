package edu.wpi.first.wpilibj3.command.async;

import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.StringJoiner;

public class Sequence implements AsyncCommand {
  private final Set<Resource> requirements;
  private final int priority;
  private final String name;
  private final List<AsyncCommand> commands;
  private final AsyncScheduler scheduler;

  public Sequence(AsyncScheduler scheduler, AsyncCommand... sequence) {
    var requirements = new HashSet<Resource>();
    int priority = DEFAULT_PRIORITY;
    var nameBuilder = new StringJoiner(" > ");
    for (AsyncCommand command : sequence) {
      requirements.addAll(command.requirements());
      priority = Math.max(priority, command.priority());
      nameBuilder.add(command.name());
    }

    this.scheduler = scheduler;
    this.requirements = requirements;
    this.priority = priority;
    this.name = nameBuilder.toString();
    this.commands = List.of(sequence);
  }

  @Override
  public void run() throws Exception {
    for (var command : commands) {
      NestedCommand cmd = new NestedCommand(this, command);
      scheduler.schedule(cmd);
      scheduler.await(cmd);
    }
  }

  @Override
  public String name() {
    return name;
  }

  @Override
  public int priority() {
    return priority;
  }

  @Override
  public Set<Resource> requirements() {
    return requirements;
  }

  @Override
  public String toString() {
    return name;
  }
}
