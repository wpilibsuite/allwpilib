package edu.wpi.first.wpilibj3.command.async;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import java.util.Collections;
import java.util.Set;
import java.util.function.Consumer;

/**
 * An asynchronous command allows command logic to be written in a traditional imperative style
 * using the collaborative concurrency tools added in Java 21; namely, continuations. Continuations
 * allow commands to be executed concurrently in a collaborative manner as coroutines. Instead of
 * needing to split command behavior into distinct functions (initialize(), execute(), end(),
 * and isFinished()), async commands can be implemented with a single, imperative loop.
 *
 * <p><strong>Note:</strong> Because coroutines are <i>opt-in</i> collaborate constructs, every
 * async command implementation <strong>must</strong> call {@link Coroutine#yield()} within
 * any periodic loop. Failure to do so may result in an unrecoverable infinite loop.</p>
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
 * AsyncCommand command = requiredSubsystem.run((coroutine) -> {
 *   initialize();
 *   while (!isFinished()) {
 *     coroutine.yield();
 *     execute();
 *   }
 *   end();
 * }).named("The Command");
 *}
 */
public interface AsyncCommand {
  /** The default command priority. */
  int DEFAULT_PRIORITY = 0;
  /**
   * The lowest possible command priority. Commands with the lowest priority can be interrupted by
   * any other command, including other minimum-priority commands.
   */
  int LOWEST_PRIORITY = Integer.MIN_VALUE;

  /**
   * The highest possible command priority. Commands with the highest priority can only be
   * interrupted by other maximum-priority commands.
   */
  int HIGHEST_PRIORITY = Integer.MAX_VALUE;

  /**
   * Runs the command. Commands that need to periodically run until a goal state is reached should
   * simply run a while loop like {@code while (!atGoal() && coroutine.yield()) { ... } }.
   *
   * <p><strong>Warning:</strong> any loops in a command must call {@code coroutine.yield()}.
   * Failure to do so will prevent anything else in your robot code from running. Async commands
   * are <i>opt-in</i> collaborative constructs; don't be greedy!</p>
   *
   * @param coroutine the coroutine backing the command's execution
   */
  void run(Coroutine coroutine);

  /**
   * The name of the command.
   *
   * @return the name of the command
   */
  String name();

  /**
   * The set of resources required by the command. This is used by the scheduler to determine if
   * two commands conflict with each other. Any singular resource may only be required by a single
   * running command at a time.
   *
   * @return the set of resources required by the command
   */
  Set<HardwareResource> requirements();

  /**
   * The priority of the command. If a command is scheduled that conflicts with another running
   * or pending command, the relative priority values are compared. If the scheduled command is
   * lower priority than the running command, then it will not be scheduled and the running command
   * will continue to run. If it is the same or higher priority, then the running command will be
   * canceled and the scheduled command will start to run.
   *
   * @return the priority of the command
   */
  default int priority() {
    return DEFAULT_PRIORITY;
  }

  enum RobotDisabledBehavior {
    /**
     * Behavior that will prevent a command from running while the robot is disabled. A command
     * with this behavior will be cancelled while running if the robot is disabled, and will not
     * be able to be scheduled while disabled.
     */
    CancelWhileDisabled,
    /**
     * Behavior that will allow a command to run while the robot is disabled. This allows safe
     * commands - commands that do not try to move actuators - to still be able to run do perform
     * tasks like updating data buffers or resetting sensors and odometry. Note that even if a
     * command that <i>does</i> try to move actuators has this behavior, it will be unable to effect
     * any movement due to the inbuilt safety mechanisms in the roboRIO and vendor hardware.
     */
    RunWhileDisabled,
  }

  /**
   * The behavior of this command when the robot is disabled. Defaults to
   * {@link RobotDisabledBehavior#CancelWhileDisabled}.
   *
   * @return the command's behavior during robot disable.
   */
  default RobotDisabledBehavior robotDisabledBehavior() {
    return RobotDisabledBehavior.CancelWhileDisabled;
  }

  enum InterruptBehavior {
    /**
     * Cancels the command when interrupted by a higher priority command. The command will be
     * removed from the scheduler without being allowed to complete.
     */
    CancelOnInterrupt,
    /**
     * Suspends the command when interrupted by a higher priority command. The scheduler will keep
     * the command around and will be resumed when every conflicting higher priority command has
     * completed or been canceled.
     */
    SuspendOnInterrupt,
  }

  /**
   * The behavior of the command when it is interrupted by a higher priority command. Defaults to
   * {@link InterruptBehavior#CancelOnInterrupt}.
   *
   * @return the command's behavior when interrupted
   */
  default InterruptBehavior interruptBehavior() {
    return InterruptBehavior.CancelOnInterrupt;
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
   * @return true if the resource is a member of the required resources, false if not
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
   * like resetting odometry and sensors that you don't want to interrupt a command that's
   * controlling the resources it affects.
   *
   * @param impl the implementation of the command logic
   * @return a builder that can be used to configure the resulting command
   */
  static AsyncCommandBuilder noHardware(Consumer<Coroutine> impl) {
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

  /**
   * Creates a new command that runs this one for a maximum duration, after which it is forcibly
   * canceled. This is particularly useful for autonomous routines where you want to prevent your
   * entire autonomous period spent stuck on a single action because a mechanism doesn't quite
   * reach its setpoint (for example, spinning up a flywheel or driving to a particular location on
   * the field). The resulting command will have the same name as this one.
   *
   * @param timeout the maximum duration that the command is permitted to run. Negative or zero
   *                values will result in the command running only once before being canceled.
   * @return the timed out command.
   */
  default AsyncCommand withTimeout(Measure<Time> timeout) {
    return ParallelGroup.race(name(), this, new WaitCommand(timeout));
  }

  static AsyncCommandBuilder requiring(HardwareResource requirement, HardwareResource... rest) {
    return new AsyncCommandBuilder().requiring(requirement).requiring(rest);
  }
}
