package edu.wpi.first.wpilibj3.command.async;

import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.StringJoiner;

/**
 * Executes other commands in a defined sequence. A sequence has ownership of all the resources
 * required by the commands in the sequence, with a priority equal to the highest priority of
 * all the composed commands.
 */
public class Sequence implements AsyncCommand {
  private final Set<HardwareResource> requirements;
  private final int priority;
  private final String name;
  private final List<AsyncCommand> commands;
  private final AsyncScheduler scheduler;

  public Sequence(AsyncScheduler scheduler, AsyncCommand... sequence) {
    var requirements = new HashSet<HardwareResource>();
    int priority = LOWEST_PRIORITY;
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
      command.run();
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
  public Set<HardwareResource> requirements() {
    return requirements;
  }

  @Override
  public String toString() {
    return name;
  }
}
