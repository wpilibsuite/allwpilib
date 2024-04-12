package edu.wpi.first.wpilibj3.command.async;

import java.util.Set;

record NestedCommand(AsyncCommand parent, AsyncCommand impl) implements AsyncCommand {
  @Override
  public void run() throws Exception {
    impl.run();
  }

  @Override
  public String name() {
    return impl.name();
  }

  @Override
  public Set<Resource> requirements() {
    return impl.requirements();
  }

  @Override
  public int priority() {
    return impl.priority();
  }
}
