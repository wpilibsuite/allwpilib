// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.units.measure.Time;
import java.util.Collection;
import java.util.Collections;
import java.util.Set;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;

/**
 * Performs some task using one or more {@link Mechanism mechanisms}. Commands are fundamentally
 * backed by a {@link Coroutine} that can be used to write imperative code that runs asynchronously.
 *
 * <p>Programmers familiar with the earlier versions of the command framework can think of a v3
 * command similar to a v1 or v2 command that executes the lifecycle methods in a single method, as
 * demonstrated in the example below. (Note, however, that more sophisticated code than this is
 * possible!
 *
 * <pre>{@code
 * coroutine -> {
 *   initialize();
 *   while (!isFinished()) {
 *     execute();
 *     coroutine.yield(); // be sure to yield at the end of the loop
 *   }
 *   end();
 * }
 * }</pre>
 *
 * <p><strong>Note:</strong> Because coroutines are <i>opt-in</i> collaborate constructs, every
 * command implementation <strong>must</strong> call {@link Coroutine#yield()} within any periodic
 * loop. Failure to do so may result in an unrecoverable infinite loop.
 *
 * <h2>Requirements</h2>
 *
 * <p>Commands require zero or more mechanisms. To prevent conflicting control requests from running
 * simultaneously (for example, commanding an elevator to both raise and lower at the same time), a
 * running command has <i>exclusive ownership</i> of all of its required mechanisms. If another
 * command with an equal or greater {@link #priority()} is scheduled that requires one or more of
 * those same mechanisms, it will interrupt and cancel the running command.
 *
 * <p>The recommended way to create a command is using {@link Mechanism#run(Consumer)} or a related
 * factory method to create commands that require a single mechanism (for example, a command that
 * drives an elevator up and down or rotates an arm). Commands may be <i>composed</i> into {@link
 * ParallelGroup parallel groups} and {@link SequentialGroup sequences} to build more complex
 * behavior out of fundamental building blocks. These built-in compositions will require every
 * mechanism used by every command in them, even if those commands aren't always running, and thus
 * can leave certain required mechanisms in an <i>uncommanded</i> state: owned, but not used, this
 * can lead to mechanisms sagging under gravity or running the previous motor control request they
 * were given.
 *
 * <h2>Advanced Usage</h2>
 *
 * <p>For example, a hypothetical drive-and-score sequence could be coded in two ways: one with a
 * sequence chain, or one that uses the lower-level coroutine API. Coroutines can be used in an
 * async/await style that you may be familiar with from languages like JavaScript, Python, or C#
 * (note that there is no {@code async} keyword; commands are inherently asynchronous). Nested
 * commands may be forked and awaited, but will not outlive the command that forked them; there is
 * no analog for something like a {@code ScheduleCommand} from the v2 commands framework.
 *
 * <pre>{@code
 * class Robot extends TimedRobot {
 *   private final Drivetrain drivetrain = new Drivetrain();
 *   private final Elevator elevator = new Elevator();
 *   private final Gripper gripper = new Gripper();
 *
 *  // Basic sequence builder - owns all 3 mechanisms for the full duration,
 *  // even when they're not being used
 *  private Command basicScoringSequence() {
 *     return drivetrain.driveToScoringLocation()
 *       .andThen(elevator.moveToScoringHeight())
 *       .andThen(gripper.release())
 *       .named("Scoring Sequence (Basic)");
 *   }
 *
 *   // Advanced sequence with async/await - only owns mechanisms while they're
 *   // being used, allowing default commands or other user-triggered commands
 *   // to run when not in use. Interrupting one of the inner commands while it's
 *   // running will cancel the entire sequence.
 *   private Command advancedScoringSequence() {
 *     return Command.noRequirements().executing(coroutine -> {
 *       coroutine.await(drivetrain.driveToScoringLocation());
 *       coroutine.await(elevator.moveToScoringHeight());
 *       coroutine.await(gripper.release());
 *     }).named("Scoring Sequence (Advanced)");
 *   }
 * }
 * }</pre>
 */
public interface Command {
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
   * simply run a while loop like {@code while (!atGoal()) { ... } } and call {@link
   * Coroutine#yield()} at the end of the loop.
   *
   * <p><strong>Warning:</strong> any loops in a command must call {@code coroutine.yield()}.
   * Failure to do so will prevent anything else in your robot code from running. Commands are
   * <i>opt-in</i> collaborative constructs; don't be greedy!
   *
   * @param coroutine the coroutine backing the command's execution
   */
  void run(Coroutine coroutine);

  /**
   * An optional lifecycle hook that can be implemented to execute some code whenever this command
   * is canceled before it naturally completes. Commands should be careful to do a single-shot
   * cleanup (for example, setting a motor to zero volts) and not do any complex looping logic here.
   */
  default void onCancel() {
    // NOP by default
  }

  /**
   * The name of the command.
   *
   * @return the name of the command
   */
  String name();

  /**
   * The mechanisms required by the command. This is used by the scheduler to determine if two
   * commands conflict with each other. No mechanism may be required by more than one running
   * command at a time.
   *
   * @return the set of mechanisms required by the command
   */
  Set<Mechanism> requirements();

  /**
   * The priority of the command. If a command is scheduled that conflicts with another running or
   * pending command, their priority values are compared to determine which should run. If the
   * scheduled command is lower priority than the running command, then it will not be scheduled and
   * the running command will continue to run. If it is the same or higher priority, then the
   * running command will be canceled and the scheduled command will start to run.
   *
   * @return the priority of the command
   */
  default int priority() {
    return DEFAULT_PRIORITY;
  }

  /**
   * Checks if this command has a lower {@link #priority() priority} than another command.
   *
   * @param other the command to compare with
   * @return true if this command has a lower priority than the other one, false otherwise
   */
  default boolean isLowerPriorityThan(Command other) {
    requireNonNullParam(other, "other", "Command.isLowerPriorityThan");

    return priority() < other.priority();
  }

  /**
   * Checks if this command requires a particular mechanism.
   *
   * @param mechanism the mechanism to check
   * @return true if the mechanism is required, false if not
   */
  default boolean requires(Mechanism mechanism) {
    return requirements().contains(mechanism);
  }

  /**
   * Checks if this command conflicts with another command.
   *
   * @param other the commands to check against
   * @return true if both commands require at least one of the same mechanism, false if both
   *     commands have completely different requirements
   */
  default boolean conflictsWith(Command other) {
    requireNonNullParam(other, "other", "Command.conflictsWith");

    return !Collections.disjoint(requirements(), other.requirements());
  }

  /**
   * Creates a new command that runs this one for a maximum duration, after which it is forcibly
   * canceled. This is particularly useful for autonomous routines where you want to prevent your
   * entire autonomous period spent stuck on a single action because a mechanism doesn't quite reach
   * its setpoint (for example, spinning up a flywheel or driving to a particular location on the
   * field). The resulting command will have the same name as this one, with the timeout period
   * appended.
   *
   * @param timeout the maximum duration that the command is permitted to run. Negative or zero
   *     values will result in the command running only once before being canceled.
   * @return the timed out command.
   */
  default Command withTimeout(Time timeout) {
    requireNonNullParam(timeout, "timeout", "Command.withTimeout");

    return race(this, waitFor(timeout).named("Timeout: " + timeout.toLongString()))
        .named(name() + " [" + timeout.toLongString() + " timeout]");
  }

  /**
   * Creates a command that does not require any hardware; that is, it does not affect the state of
   * any physical objects. This is useful for commands that do some cleanup or state management,
   * such as resetting odometry or sensors, that you don't want to interrupt a command that's
   * controlling the mechanisms it affects.
   *
   * <p>More configuration options are needed after calling this function before the command can be
   * created. See {@link StagedCommandBuilder} for details.
   *
   * @return a builder that can be used to configure the resulting command
   */
  static NeedsExecutionBuilderStage noRequirements() {
    return new StagedCommandBuilder().noRequirements();
  }

  /**
   * Creates a command that requires one or more mechanisms.
   *
   * <p>More configuration options are needed after calling this function before the command can be
   * created. See {@link StagedCommandBuilder} for details.
   *
   * @param requirement The first required mechanism
   * @param rest Any other required mechanisms
   * @return A command builder
   */
  static NeedsExecutionBuilderStage requiring(Mechanism requirement, Mechanism... rest) {
    // parameters will be null checked by the builder
    return new StagedCommandBuilder().requiring(requirement, rest);
  }

  /**
   * Creates command that requires some number of mechanisms.
   *
   * <p>More configuration options are needed after calling this function before the command can be
   * created. See {@link StagedCommandBuilder} for details.
   *
   * @param requirements The required mechanisms. May be empty, but cannot contain null values.
   * @return A command builder
   */
  static NeedsExecutionBuilderStage requiring(Collection<Mechanism> requirements) {
    // parameters will be null checked by the builder
    return new StagedCommandBuilder().requiring(requirements);
  }

  /**
   * Starts creating a command that runs a group of multiple commands in parallel. The command will
   * complete when every command in the group has completed naturally.
   *
   * <p>More configuration options are needed after calling this function before the command can be
   * created. See {@link ParallelGroupBuilder} for details.
   *
   * @param commands The commands to run in parallel
   * @return A command builder
   */
  static ParallelGroupBuilder parallel(Command... commands) {
    // parameters will be null checked by the builder
    return new ParallelGroupBuilder().requiring(commands);
  }

  /**
   * Starts creating a command that runs a group of multiple commands in parallel. The command will
   * complete when any command in the group has completed naturally; all other commands in the group
   * will be canceled.
   *
   * <p>More configuration options are needed after calling this function before the command can be
   * created. See {@link ParallelGroupBuilder} for details.
   *
   * @param commands The commands to run in parallel
   * @return A command builder
   */
  static ParallelGroupBuilder race(Command... commands) {
    // parameters will be null checked by the builder
    return new ParallelGroupBuilder().optional(commands);
  }

  /**
   * Starts creating a command that runs a group of multiple commands in sequence. The command will
   * complete when the last command in the group has completed naturally. Commands in the group will
   * run in the order they're passed to this method.
   *
   * <p>More configuration options are needed after calling this function before the command can be
   * created. See {@link SequenceBuilder} for details.
   *
   * @param commands The commands to run in sequence.
   * @return A command builder
   */
  static SequenceBuilder sequence(Command... commands) {
    // parameters will be null checked by the builder
    return new SequenceBuilder().andThen(commands);
  }

  /**
   * Starts creating a command that simply waits for some condition to be met. The command will not
   * require any mechanisms.
   *
   * <p>More configuration options are needed after calling this function before the command can be
   * created. See {@link StagedCommandBuilder} for details.
   *
   * @param condition The condition to wait for
   * @return A command builder
   */
  static NeedsNameBuilderStage waitUntil(BooleanSupplier condition) {
    requireNonNullParam(condition, "condition", "Command.waitUntil");

    return noRequirements().executing(coroutine -> coroutine.waitUntil(condition));
  }

  /**
   * Creates a command that simply waits for a given duration. The command will not require any
   * mechanisms.
   *
   * @param duration How long to wait
   * @return A command builder
   */
  static NeedsNameBuilderStage waitFor(Time duration) {
    requireNonNullParam(duration, "duration", "Command.waitFor");

    return noRequirements().executing(coroutine -> coroutine.wait(duration));
  }

  /**
   * Creates a command that runs this one and ends when the end condition is met (if this command
   * has not already exited by then).
   *
   * <p>More configuration options are needed after calling this function before the command can be
   * created. See {@link ParallelGroupBuilder} for details.
   *
   * @param endCondition The end condition to wait for.
   * @return The waiting command
   */
  default ParallelGroupBuilder until(BooleanSupplier endCondition) {
    requireNonNullParam(endCondition, "endCondition", "Command.until");

    return new ParallelGroupBuilder()
        .optional(this, Command.waitUntil(endCondition).named("Until Condition"));
  }

  /**
   * Creates a command sequence, starting from this command and then running the next one. More
   * commands can be added with the builder before naming and creating the sequence.
   *
   * <pre>{@code
   * Sequence aThenBThenC =
   *   commandA()
   *     .andThen(commandB())
   *     .andThen(commandC())
   *     .withAutomaticName();
   * }</pre>
   *
   * @param next The command to run after this one in the sequence
   * @return A sequence builder
   */
  default SequenceBuilder andThen(Command next) {
    // parameter will be null checked by the builder
    return new SequenceBuilder().andThen(this).andThen(next);
  }

  /**
   * Creates a parallel command group, running this command alongside one or more other commands.
   * The group will exit once every command has finished.
   *
   * <p>More configuration options are needed after calling this function before the command can be
   * created. See {@link ParallelGroupBuilder} for details.
   *
   * <pre>{@code
   * ParallelGroup abc =
   *   commandA()
   *     .alongWith(commandB(), commandC())
   *     .withAutomaticName();
   * }</pre>
   *
   * @param parallel The commands to run in parallel with this one
   * @return A parallel group builder
   */
  default ParallelGroupBuilder alongWith(Command... parallel) {
    return new ParallelGroupBuilder().requiring(this).requiring(parallel);
  }

  /**
   * Creates a parallel command group, running this command alongside one or more other commands.
   * The group will exit after any command finishes.
   *
   * <p>More configuration options are needed after calling this function before the command can be
   * created. See {@link ParallelGroupBuilder} for details.
   *
   * @param parallel The commands to run in parallel with this one
   * @return A parallel group builder
   */
  default ParallelGroupBuilder raceWith(Command... parallel) {
    return new ParallelGroupBuilder().optional(this).optional(parallel);
  }
}
