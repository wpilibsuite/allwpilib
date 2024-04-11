package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.units.Units.Milliseconds;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.concurrent.Callable;

public interface AsyncCommand extends Callable<Object> {
  int DEFAULT_PRIORITY = 0;
  int LOWEST_PRIORITY = Integer.MIN_VALUE;
  int HIGHEST_PRIORITY = Integer.MAX_VALUE;

  void run() throws Exception;

  String name();

  @Override
  default Object call() throws Exception {
    run();
    return null;
  }

  Set<Resource> requirements();

  default int priority() {
    return DEFAULT_PRIORITY;
  }

  /**
   * Checks if this command has a lower {@link #priority() priority} than another command.
   *
   * @param other the command to compare with
   * @return true if this command has a lower priority than the other one, false otherwise
   */
  default boolean isLowerPriorityThan(AsyncCommand other) {
    if (other == null) return false;

    return priority() < other.priority();
  }

  /**
   * Checks if this command requires a particular resource.
   *
   * @param resource the resource to check
   * @return
   */
  default boolean requires(Resource resource) {
    return requirements().contains(resource);
  }

  /**
   * A helper method for executing a command body in a periodic loop until some end condition is
   * met.
   *
   * <pre>{@code
   *   loop(() -> m_motor.set(m_pid.calculate(m_sensor.readValue())))
   *     .withPeriod(Milliseconds.of(20))
   *     .forDuration(Seconds.of(1.5));
   * }</pre>
   *
   * @param body the loop body
   */
  static Loop.Builder loop(ThrowingRunnable body) {
    return new Loop.Builder(body);
  }

  /**
   * Creates an async command that does not require any hardware; that is, it does not affect the
   * state of any physical objects. This is useful for commands that do some house cleaning work
   * like resetting odometry and sensors.
   *
   * @param impl the implementation of the command logic
   * @return
   */
  static AsyncCommandBuilder noHardware(ThrowingRunnable impl) {
    return new AsyncCommandBuilder().executing(impl);
  }

  default Stage alongWith(AsyncCommand other) {
    return Stage.all(this, other);
  }

  default Group andThen(AsyncCommand next) {
    return new Group(List.of(
        Stage.all(this),
        Stage.all(next)
    ));
  }

  /**
   * Schedules this command with the default async scheduler.
   */
  default void schedule() {
    AsyncScheduler.getInstance().schedule(this);
  }

  /**
   * Cancels this command, if running on the default async scheduler.
   */
  default void cancel() {
    AsyncScheduler.getInstance().cancel(this);
  }

  /**
   * Checks if this command is currently scheduled to be running on the default async scheduler.
   */
  default boolean isScheduled() {
    return AsyncScheduler.getInstance().isRunning(this);
  }

  class AsyncCommandBuilder {
    private final Set<Resource> requirements = new HashSet<>();
    private ThrowingRunnable impl;
    private String name;
    private int priority = DEFAULT_PRIORITY;

    public AsyncCommandBuilder requiring(Resource resource) {
      requirements.add(resource);
      return this;
    }

    public AsyncCommandBuilder requiring(Resource... resources) {
      requirements.addAll(Arrays.asList(resources));
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

    public AsyncCommand prioritized(int priority) {
      return withPriority(priority).make();
    }

    public AsyncCommandBuilder executing(ThrowingRunnable impl) {
      this.impl = impl;
      return this;
    }

    public AsyncCommand execute(ThrowingRunnable impl) {
      return executing(impl).make();
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
        public Set<Resource> requirements() {
          return requirements;
        }

        @Override
        public String toString() {
          return name();
        }
      };
    }
  }

  static AsyncCommandBuilder requiring(Resource requirement, Resource... rest) {
    return new AsyncCommandBuilder().requiring(requirement).requiring(rest);
  }

  static void yield(Measure<Time> duration) throws InterruptedException {
    Thread.sleep((long) duration.in(Milliseconds));
  }

  static void yield() throws InterruptedException {
    AsyncCommand.yield(AsyncScheduler.DEFAULT_UPDATE_PERIOD);
  }
}
