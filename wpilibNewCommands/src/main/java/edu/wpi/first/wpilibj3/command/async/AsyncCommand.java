package edu.wpi.first.wpilibj3.command.async;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Set;
import java.util.stream.Collectors;

/**
 * An asynchronous command allows command logic to be written in a traditional imperative style
 * using the collaborative concurrency tools added in Java 21; namely, virtual threads. Virtual
 * threads allow commands to be executed concurrently on a single operating system-level thread
 * (called a "carrier thread") as long as the commands pause themselves at regular intervals to
 * allow for other commands to run on the same carrier thread. The class-based and function-based
 * command APIs both operate under a collaborative model where the commands have distinct logic
 * for initialization, execution, and completion stages of their lifecycle. Those can be
 *
 * {@snippet lang = java:
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
public interface AsyncCommand {
  int DEFAULT_PRIORITY = 0;
  int LOWEST_PRIORITY = Integer.MIN_VALUE;
  int HIGHEST_PRIORITY = Integer.MAX_VALUE;

  void run();

  String name();

  Set<HardwareResource> requirements();

  default int priority() {
    return DEFAULT_PRIORITY;
  }

  enum RobotDisabledBehavior {
    CancelWhileDisabled,
    RunWhileDisabled,
  }

  default RobotDisabledBehavior robotDisabledBehavior() {
    return RobotDisabledBehavior.CancelWhileDisabled;
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
  default boolean requires(HardwareResource resource) {
    return requirements().contains(resource);
  }

  /**
   * Checks if this command conflicts with another command.
   *
   * @param other the commands to check against
   * @return true if both commands require at least one of the same resource, false if both commands
   *   have completely different requirements
   */
  default boolean conflictsWith(AsyncCommand other) {
    return !Collections.disjoint(requirements(), other.requirements());
  }

  /**
   * Creates an async command that does not require any hardware; that is, it does not affect the
   * state of any physical objects. This is useful for commands that do some house cleaning work
   * like resetting odometry and sensors.
   *
   * @param impl the implementation of the command logic
   * @return
   */
  static AsyncCommandBuilder noHardware(Runnable impl) {
    return new AsyncCommandBuilder().executing(impl);
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

  default AsyncCommand withTimeout(Measure<Time> timeout) {
    return ParallelGroup.race(name(), this, new WaitCommand(timeout));
  }

  static AsyncCommandBuilder requiring(HardwareResource requirement, HardwareResource... rest) {
    return new AsyncCommandBuilder().requiring(requirement).requiring(rest);
  }

  static void yield() {
    AsyncScheduler.getInstance().yield();
  }

  static void park() {
    while (true) {
      AsyncCommand.yield();
    }
  }
}
