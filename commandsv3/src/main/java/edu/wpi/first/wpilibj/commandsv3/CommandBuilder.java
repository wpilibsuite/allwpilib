package edu.wpi.first.wpilibj.commandsv3;

import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;
import java.util.function.Consumer;

public class CommandBuilder {
  private final Set<RequireableResource> requirements = new HashSet<>();
  private Consumer<Coroutine> impl;
  private Runnable onCancel = () -> {};
  private String name;
  private int priority = Command.DEFAULT_PRIORITY;
  private Command.RobotDisabledBehavior disabledBehavior =
      Command.RobotDisabledBehavior.CancelWhileDisabled;
  private Command.InterruptBehavior interruptBehavior = Command.InterruptBehavior.CancelOnInterrupt;

  public CommandBuilder requiring(RequireableResource resource) {
    requirements.add(resource);
    return this;
  }

  public CommandBuilder requiring(RequireableResource... resources) {
    requirements.addAll(Arrays.asList(resources));
    return this;
  }

  public CommandBuilder requiring(Collection<RequireableResource> resources) {
    requirements.addAll(resources);
    return this;
  }

  public CommandBuilder withName(String name) {
    this.name = name;
    return this;
  }

  public Command named(String name) {
    return withName(name).make();
  }

  public CommandBuilder withPriority(int priority) {
    this.priority = priority;
    return this;
  }

  public CommandBuilder ignoringDisable() {
    return withDisabledBehavior(Command.RobotDisabledBehavior.RunWhileDisabled);
  }

  public CommandBuilder withDisabledBehavior(Command.RobotDisabledBehavior behavior) {
    this.disabledBehavior = behavior;
    return this;
  }

  public CommandBuilder suspendingOnInterrupt() {
    return withInterruptBehavior(Command.InterruptBehavior.SuspendOnInterrupt);
  }

  public CommandBuilder withInterruptBehavior(Command.InterruptBehavior behavior) {
    this.interruptBehavior = behavior;
    return this;
  }

  public CommandBuilder executing(Consumer<Coroutine> impl) {
    this.impl = impl;
    return this;
  }

  public CommandBuilder whenCanceled(Runnable onCancel) {
    this.onCancel = onCancel;
    return this;
  }

  public Command make() {
    Objects.requireNonNull(name, "Name was not specified");
    Objects.requireNonNull(impl, "Command logic was not specified");

    return new Command() {
      @Override
      public void run(Coroutine coroutine) {
        impl.accept(coroutine);
      }

      @Override
      public void onCancel() {
        onCancel.run();
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
      public InterruptBehavior interruptBehavior() {
        return interruptBehavior;
      }

      @Override
      public String toString() {
        return name();
      }
    };
  }
}
