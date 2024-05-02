package edu.wpi.first.wpilibj3.command.async;

import java.util.Set;

/**
 * An idle command that only parks the executing thread and does nothing else.
 *
 * @param resource the resource to idle.
 */
public record IdleCommand(HardwareResource resource) implements AsyncCommand {
  @Override
  @SuppressWarnings({"InfiniteLoopStatement", "BusyWait"})
  public void run() throws Exception {
    while (true) {
      Thread.sleep(Long.MAX_VALUE);
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
