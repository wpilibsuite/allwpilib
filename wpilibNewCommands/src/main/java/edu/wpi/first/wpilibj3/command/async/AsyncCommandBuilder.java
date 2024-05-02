package edu.wpi.first.wpilibj3.command.async;

import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

public class AsyncCommandBuilder {
  private final Set<HardwareResource> requirements = new HashSet<>();
  private ThrowingRunnable impl;
  private String name;
  private int priority = AsyncCommand.DEFAULT_PRIORITY;
  private AsyncCommand.InterruptBehavior interruptBehavior = AsyncCommand.InterruptBehavior.Cancel;
  private AsyncCommand.RobotDisabledBehavior disabledBehavior = AsyncCommand.RobotDisabledBehavior.CancelWhileDisabled;

  public AsyncCommandBuilder requiring(HardwareResource resource) {
    requirements.add(resource);
    return this;
  }

  public AsyncCommandBuilder requiring(HardwareResource... resources) {
    requirements.addAll(Arrays.asList(resources));
    return this;
  }

  public AsyncCommandBuilder requiring(Collection<HardwareResource> resources) {
    requirements.addAll(resources);
    return this;
  }

  public AsyncCommandBuilder withName(String name) {
    this.name = name;
    return this;
  }

  public AsyncCommand named(String name) {
    return withName(name).make();
  }

  public AsyncCommandBuilder withPriority(int priority) {
    this.priority = priority;
    return this;
  }

  public AsyncCommandBuilder suspendOnInterrupt() {
    return withInterruptBehavior(AsyncCommand.InterruptBehavior.Suspend);
  }

  public AsyncCommandBuilder withInterruptBehavior(AsyncCommand.InterruptBehavior behavior) {
    this.interruptBehavior = behavior;
    return this;
  }

  public AsyncCommandBuilder ignoringDisable() {
    return withDisabledBehavior(AsyncCommand.RobotDisabledBehavior.RunWhileDisabled);
  }

  public AsyncCommandBuilder withDisabledBehavior(AsyncCommand.RobotDisabledBehavior behavior) {
    this.disabledBehavior = behavior;
    return this;
  }

  public AsyncCommandBuilder executing(ThrowingRunnable impl) {
    this.impl = impl;
    return this;
  }

  public AsyncCommand make() {
    Objects.requireNonNull(name, "Name was not specified");
    Objects.requireNonNull(impl, "Command logic was not specified");

    return new AsyncCommand() {
      @Override
      public void run() throws Exception {
        impl.run();
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
      public InterruptBehavior interruptBehavior() {
        return interruptBehavior;
      }

      @Override
      public RobotDisabledBehavior robotDisabledBehavior() {
        return disabledBehavior;
      }

      @Override
      public String toString() {
        return name();
      }
    };
  }
}
