package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.units.Units.Milliseconds;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import java.util.Objects;
import java.util.Set;

/**
 * An idle command that only parks the executing thread and does nothing else.
 *
 * @param resource the resource to idle.
 */
public record IdleCommand(HardwareResource resource) implements AsyncCommand {
  @Override
  public void run() throws InterruptedException {
    while (true) {
      AsyncScheduler.getInstance().yield();
    }
  }

  @Override
  public Set<HardwareResource> requirements() {
    return Set.of(resource);
  }

  @Override
  public String name() {
    return resource.getName() + "[IDLE]";
  }

  @Override
  public int priority() {
    // lowest priority - an idle command can be interrupted by anything else
    return LOWEST_PRIORITY;
  }

  @Override
  public String toString() {
    return name();
  }
}
