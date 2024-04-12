package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.units.Units.Milliseconds;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import java.util.Set;
import java.util.concurrent.Callable;

/**
 * An asynchronous command allows command logic to be written in a traditional imperative style
 * using the collaborative concurrency tools added in Java 21; namely, virtual threads. Virtual
 * threads allow commands to be executed concurrently on a single operating system-level thread
 * (called a "carrier thread") as long as the commands pause themselves at regular intervals to
 * allow for other commands to run on the same carrier thread. The class-based and function-based
 * command APIs both operate under a collaborative model where the commands have distinct logic
 * for initialization, execution, and completion stages of their lifecycle. Those can be
 *
 * {@snippet lang=java :
 * // A 2013-style class-based command definition
 * class ClassBasedCommand extends Command {
 *   public ClassBasedCommand(Subsystem requiredSubsystem) {
 *     addRequirements(requiredSubsystem);
 *   }
 *
 *   @Override
 *   public void initialize() {}
 *
 *   @Override
 *   public void execute() {}
 *
 *   @Override
 *   public void end(boolean interrupted) {}
 *
 *   @Override
 *   public void isFinished() { return true; }
 *
 *   @Override
 *   public String getName() { return "The Command"; }
 * }
 *
 * Command command = new ClassBasedCommand(requiredSubsystem);
 *
 * // Or a 2020-style function-based command
 * Command command = requiredSubsystem
 *   .runOnce(() -> initialize())
 *   .andThen(
 *     requiredSubsystem
 *       .run(() -> execute())
 *       .until(() -> isFinished())
 *       .onFinish(() -> end())
 *   ).withName("The Command");
 *
 * // Can be represented with a 2025-style async-based definition
 * AsyncCommand command = requiredSubsystem.run(() -> {
 *   initialize();
 *   while (!isFinished()) {
 *     AsyncCommand.yield();
 *     execute();
 *   }
 *   end();
 * }).named("The Command");
 *}
 */
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
   * loop(() -> m_motor.set(m_pid.calculate(m_sensor.readValue())))
   *   .withPeriod(Milliseconds.of(20))
   *   .forDuration(Seconds.of(1.5));
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

  default ParallelGroup.Builder alongWith(AsyncScheduler scheduler, AsyncCommand... other) {
    return ParallelGroup.onScheduler(scheduler).all(this, other);
  }

  default ParallelGroup.Builder alongWith(AsyncCommand... other) {
    return alongWith(AsyncScheduler.getInstance(), other);
  }

  default AsyncCommand andThen(AsyncCommand next) {
    return new Sequence(AsyncScheduler.getInstance(), this, next);
  }

  /** Schedules this command with the default async scheduler. */
  default void schedule() {
    AsyncScheduler.getInstance().schedule(this);
  }

  /** Cancels this command, if running on the default async scheduler. */
  default void cancel() {
    AsyncScheduler.getInstance().cancel(this);
  }

  /** Checks if this command is currently scheduled to be running on the default async scheduler. */
  default boolean isScheduled() {
    return AsyncScheduler.getInstance().isRunning(this);
  }

  static AsyncCommandBuilder requiring(Resource requirement, Resource... rest) {
    return new AsyncCommandBuilder().requiring(requirement).requiring(rest);
  }

  /**
   * Pauses the execution thread for the given duration. This is intended to be used by commands to
   * pause themselves and allow other commands to run.
   *
   * @param duration how long the command should pause for. Shorter durations provide higher
   *                 resolution for control loops, but incur higher CPU use and may cause contention
   *                 if too many commands with low pause times are running concurrently.
   * @throws InterruptedException if the command was interrupted by another command while paused
   */
  static void yield(Measure<Time> duration) throws InterruptedException {
    Thread.sleep((long) duration.in(Milliseconds));
  }

  /**
   * Pauses the execution thread for the {@link AsyncScheduler#DEFAULT_UPDATE_PERIOD default period}
   * to allow commands to pause themselves and allow other commands to run.
   *
   * @throws InterruptedException if the command was interrupted by another command while paused.
   */
  static void yield() throws InterruptedException {
    AsyncCommand.yield(AsyncScheduler.DEFAULT_UPDATE_PERIOD);
  }
}
