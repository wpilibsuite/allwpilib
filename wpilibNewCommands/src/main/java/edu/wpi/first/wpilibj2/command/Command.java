// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.util.function.BooleanConsumer;
import java.util.Set;
import java.util.function.BooleanSupplier;

/**
 * A state machine representing a complete action to be performed by the robot. Commands are run by
 * the {@link CommandScheduler}, and can be composed into CommandGroups to allow users to build
 * complicated multi-step actions without the need to roll the state machine logic themselves.
 *
 * <p>Commands are run synchronously from the main robot loop; no multithreading is used, unless
 * specified explicitly from the command implementation.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public interface Command {
  /** The initial subroutine of a command. Called once when the command is initially scheduled. */
  default void initialize() {}

  /** The main body of a command. Called repeatedly while the command is scheduled. */
  default void execute() {}

  /**
   * The action to take when the command ends. Called when either the command finishes normally, or
   * when it interrupted/canceled.
   *
   * <p>Do not schedule commands here that share requirements with this command. Use {@link
   * #andThen(Command...)} instead.
   *
   * @param interrupted whether the command was interrupted/canceled
   */
  default void end(boolean interrupted) {}

  /**
   * Whether the command has finished. Once a command finishes, the scheduler will call its end()
   * method and un-schedule it.
   *
   * @return whether the command has finished.
   */
  default boolean isFinished() {
    return false;
  }

  /**
   * Specifies the set of subsystems used by this command. Two commands cannot use the same
   * subsystem at the same time. If another command is scheduled that shares a requirement, {@link
   * #getInterruptionBehavior()} will be checked and followed. If no subsystems are required, return
   * an empty set.
   *
   * <p>Note: it is recommended that user implementations contain the requirements as a field, and
   * return that field here, rather than allocating a new set every time this is called.
   *
   * @return the set of subsystems that are required
   * @see InterruptionBehavior
   */
  Set<Subsystem> getRequirements();

  /**
   * Decorates this command with a timeout. If the specified timeout is exceeded before the command
   * finishes normally, the command will be interrupted and un-scheduled. Note that the timeout only
   * applies to the command returned by this method; the calling command is not itself changed.
   *
   * <p>Note: This decorator works by composing this command within a CommandGroup. The command
   * cannot be used independently after being decorated, or be re-decorated with a different
   * decorator, unless it is manually cleared from the list of grouped commands with {@link
   * CommandGroupBase#clearGroupedCommand(Command)}. The decorated command can, however, be further
   * decorated without issue.
   *
   * @param seconds the timeout duration
   * @return the command with the timeout added
   */
  default ParallelRaceGroup withTimeout(double seconds) {
    return raceWith(new WaitCommand(seconds));
  }

  /**
   * Decorates this command with an interrupt condition. If the specified condition becomes true
   * before the command finishes normally, the command will be interrupted and un-scheduled. Note
   * that this only applies to the command returned by this method; the calling command is not
   * itself changed.
   *
   * <p>Note: This decorator works by composing this command within a CommandGroup. The command
   * cannot be used independently after being decorated, or be re-decorated with a different
   * decorator, unless it is manually cleared from the list of grouped commands with {@link
   * CommandGroupBase#clearGroupedCommand(Command)}. The decorated command can, however, be further
   * decorated without issue.
   *
   * @param condition the interrupt condition
   * @return the command with the interrupt condition added
   */
  default ParallelRaceGroup until(BooleanSupplier condition) {
    return raceWith(new WaitUntilCommand(condition));
  }

  /**
   * Decorates this command with an interrupt condition. If the specified condition becomes true
   * before the command finishes normally, the command will be interrupted and un-scheduled. Note
   * that this only applies to the command returned by this method; the calling command is not
   * itself changed.
   *
   * <p>Note: This decorator works by composing this command within a CommandGroup. The command
   * cannot be used independently after being decorated, or be re-decorated with a different
   * decorator, unless it is manually cleared from the list of grouped commands with {@link
   * CommandGroupBase#clearGroupedCommand(Command)}. The decorated command can, however, be further
   * decorated without issue.
   *
   * @param condition the interrupt condition
   * @return the command with the interrupt condition added
   * @deprecated Replace with {@link #until(BooleanSupplier)}
   */
  @Deprecated(since = "2023")
  default ParallelRaceGroup withInterrupt(BooleanSupplier condition) {
    return until(condition);
  }

  /**
   * Decorates this command with a runnable to run before this command starts.
   *
   * <p>Note: This decorator works by composing this command within a CommandGroup. The command
   * cannot be used independently after being decorated, or be re-decorated with a different
   * decorator, unless it is manually cleared from the list of grouped commands with {@link
   * CommandGroupBase#clearGroupedCommand(Command)}. The decorated command can, however, be further
   * decorated without issue.
   *
   * @param toRun the Runnable to run
   * @param requirements the required subsystems
   * @return the decorated command
   */
  default SequentialCommandGroup beforeStarting(Runnable toRun, Subsystem... requirements) {
    return beforeStarting(new InstantCommand(toRun, requirements));
  }

  /**
   * Decorates this command with another command to run before this command starts.
   *
   * <p>Note: This decorator works by composing this command within a CommandGroup. The command
   * cannot be used independently after being decorated, or be re-decorated with a different
   * decorator, unless it is manually cleared from the list of grouped commands with {@link
   * CommandGroupBase#clearGroupedCommand(Command)}. The decorated command can, however, be further
   * decorated without issue.
   *
   * @param before the command to run before this one
   * @return the decorated command
   */
  default SequentialCommandGroup beforeStarting(Command before) {
    return new SequentialCommandGroup(before, this);
  }

  /**
   * Decorates this command with a runnable to run after the command finishes.
   *
   * <p>Note: This decorator works by composing this command within a CommandGroup. The command
   * cannot be used independently after being decorated, or be re-decorated with a different
   * decorator, unless it is manually cleared from the list of grouped commands with {@link
   * CommandGroupBase#clearGroupedCommand(Command)}. The decorated command can, however, be further
   * decorated without issue.
   *
   * @param toRun the Runnable to run
   * @param requirements the required subsystems
   * @return the decorated command
   */
  default SequentialCommandGroup andThen(Runnable toRun, Subsystem... requirements) {
    return andThen(new InstantCommand(toRun, requirements));
  }

  /**
   * Decorates this command with a set of commands to run after it in sequence. Often more
   * convenient/less-verbose than constructing a new {@link SequentialCommandGroup} explicitly.
   *
   * <p>Note: This decorator works by composing this command within a CommandGroup. The command
   * cannot be used independently after being decorated, or be re-decorated with a different
   * decorator, unless it is manually cleared from the list of grouped commands with {@link
   * CommandGroupBase#clearGroupedCommand(Command)}. The decorated command can, however, be further
   * decorated without issue.
   *
   * @param next the commands to run next
   * @return the decorated command
   */
  default SequentialCommandGroup andThen(Command... next) {
    SequentialCommandGroup group = new SequentialCommandGroup(this);
    group.addCommands(next);
    return group;
  }

  /**
   * Decorates this command with a set of commands to run parallel to it, ending when the calling
   * command ends and interrupting all the others. Often more convenient/less-verbose than
   * constructing a new {@link ParallelDeadlineGroup} explicitly.
   *
   * <p>Note: This decorator works by composing this command within a CommandGroup. The command
   * cannot be used independently after being decorated, or be re-decorated with a different
   * decorator, unless it is manually cleared from the list of grouped commands with {@link
   * CommandGroupBase#clearGroupedCommand(Command)}. The decorated command can, however, be further
   * decorated without issue.
   *
   * @param parallel the commands to run in parallel
   * @return the decorated command
   */
  default ParallelDeadlineGroup deadlineWith(Command... parallel) {
    return new ParallelDeadlineGroup(this, parallel);
  }

  /**
   * Decorates this command with a set of commands to run parallel to it, ending when the last
   * command ends. Often more convenient/less-verbose than constructing a new {@link
   * ParallelCommandGroup} explicitly.
   *
   * <p>Note: This decorator works by composing this command within a CommandGroup. The command
   * cannot be used independently after being decorated, or be re-decorated with a different
   * decorator, unless it is manually cleared from the list of grouped commands with {@link
   * CommandGroupBase#clearGroupedCommand(Command)}. The decorated command can, however, be further
   * decorated without issue.
   *
   * @param parallel the commands to run in parallel
   * @return the decorated command
   */
  default ParallelCommandGroup alongWith(Command... parallel) {
    ParallelCommandGroup group = new ParallelCommandGroup(this);
    group.addCommands(parallel);
    return group;
  }

  /**
   * Decorates this command with a set of commands to run parallel to it, ending when the first
   * command ends. Often more convenient/less-verbose than constructing a new {@link
   * ParallelRaceGroup} explicitly.
   *
   * <p>Note: This decorator works by composing this command within a CommandGroup. The command
   * cannot be used independently after being decorated, or be re-decorated with a different
   * decorator, unless it is manually cleared from the list of grouped commands with {@link
   * CommandGroupBase#clearGroupedCommand(Command)}. The decorated command can, however, be further
   * decorated without issue.
   *
   * @param parallel the commands to run in parallel
   * @return the decorated command
   */
  default ParallelRaceGroup raceWith(Command... parallel) {
    ParallelRaceGroup group = new ParallelRaceGroup(this);
    group.addCommands(parallel);
    return group;
  }

  /**
   * Decorates this command to run perpetually, ignoring its ordinary end conditions. The decorated
   * command can still be interrupted or canceled.
   *
   * <p>Note: This decorator works by composing this command within a CommandGroup. The command
   * cannot be used independently after being decorated, or be re-decorated with a different
   * decorator, unless it is manually cleared from the list of grouped commands with {@link
   * CommandGroupBase#clearGroupedCommand(Command)}. The decorated command can, however, be further
   * decorated without issue.
   *
   * @return the decorated command
   * @deprecated PerpetualCommand violates the assumption that execute() doesn't get called after
   *     isFinished() returns true -- an assumption that should be valid. This was unsafe/undefined
   *     behavior from the start, and RepeatCommand provides an easy way to achieve similar end
   *     results with slightly different (and safe) semantics.
   */
  @SuppressWarnings("removal") // PerpetualCommand
  @Deprecated(forRemoval = true, since = "2023")
  default PerpetualCommand perpetually() {
    return new PerpetualCommand(this);
  }

  /**
   * Decorates this command to run repeatedly, restarting it when it ends, until this command is
   * interrupted. The decorated command can still be canceled.
   *
   * <p>Note: This decorator works by composing this command within a CommandGroup. The command
   * cannot be used independently after being decorated, or be re-decorated with a different
   * decorator, unless it is manually cleared from the list of grouped commands with {@link
   * CommandGroupBase#clearGroupedCommand(Command)}. The decorated command can, however, be further
   * decorated without issue.
   *
   * @return the decorated command
   */
  default RepeatCommand repeatedly() {
    return new RepeatCommand(this);
  }

  /**
   * Decorates this command to run "by proxy" by wrapping it in a {@link ProxyCommand}. This is
   * useful for "forking off" from command groups when the user does not wish to extend the
   * command's requirements to the entire command group.
   *
   * @return the decorated command
   */
  default ProxyCommand asProxy() {
    return new ProxyCommand(this);
  }

  /**
   * Decorates this command to only run if this condition is not met. If the command is already
   * running and the condition changes to true, the command will not stop running. The requirements
   * of this command will be kept for the new conditonal command.
   *
   * @param condition the condition that will prevent the command from running
   * @return the decorated command
   */
  default ConditionalCommand unless(BooleanSupplier condition) {
    return new ConditionalCommand(new InstantCommand(), this, condition);
  }

  /**
   * Decorates this command to run or stop when disabled.
   *
   * @param doesRunWhenDisabled true to run when disabled.
   * @return the decorated command
   */
  default WrapperCommand ignoringDisable(boolean doesRunWhenDisabled) {
    return new WrapperCommand(this) {
      @Override
      public boolean runsWhenDisabled() {
        return doesRunWhenDisabled;
      }
    };
  }

  /**
   * Decorates this command to have a different {@link InterruptionBehavior interruption behavior}.
   *
   * @param interruptBehavior the desired interrupt behavior
   * @return the decorated command
   */
  default WrapperCommand withInterruptBehavior(InterruptionBehavior interruptBehavior) {
    return new WrapperCommand(this) {
      @Override
      public InterruptionBehavior getInterruptionBehavior() {
        return interruptBehavior;
      }
    };
  }

  /**
   * Decorates this command with a lambda to call on interrupt or end, following the command's
   * inherent {@link #end(boolean)} method.
   *
   * @param end a lambda accepting a boolean parameter specifying whether the command was
   *     interrupted.
   * @return the decorated command
   */
  default WrapperCommand finallyDo(BooleanConsumer end) {
    requireNonNullParam(end, "end", "Command.finallyDo()");
    return new WrapperCommand(this) {
      @Override
      public void end(boolean interrupted) {
        super.end(interrupted);
        end.accept(interrupted);
      }
    };
  }

  /**
   * Decorates this command with a lambda to call on interrupt, following the command's inherent
   * {@link #end(boolean)} method.
   *
   * @param handler a lambda to run when the command is interrupted
   * @return the decorated command
   */
  default WrapperCommand handleInterrupt(Runnable handler) {
    requireNonNullParam(handler, "handler", "Command.handleInterrupt()");
    return finallyDo(
        interrupted -> {
          if (interrupted) {
            handler.run();
          }
        });
  }

  /** Schedules this command. */
  default void schedule() {
    CommandScheduler.getInstance().schedule(this);
  }

  /**
   * Cancels this command. Will call {@link #end(boolean) end(true)}. Commands will be canceled
   * regardless of {@link InterruptionBehavior interruption behavior}.
   *
   * @see CommandScheduler#cancel(Command...)
   */
  default void cancel() {
    CommandScheduler.getInstance().cancel(this);
  }

  /**
   * Whether or not the command is currently scheduled. Note that this does not detect whether the
   * command is being run by a CommandGroup, only whether it is directly being run by the scheduler.
   *
   * @return Whether the command is scheduled.
   */
  default boolean isScheduled() {
    return CommandScheduler.getInstance().isScheduled(this);
  }

  /**
   * Whether the command requires a given subsystem.
   *
   * @param requirement the subsystem to inquire about
   * @return whether the subsystem is required
   */
  default boolean hasRequirement(Subsystem requirement) {
    return getRequirements().contains(requirement);
  }

  /**
   * How the command behaves when another command with a shared requirement is scheduled.
   *
   * @return a variant of {@link InterruptionBehavior}, defaulting to {@link
   *     InterruptionBehavior#kCancelSelf kCancelSelf}.
   */
  default InterruptionBehavior getInterruptionBehavior() {
    return InterruptionBehavior.kCancelSelf;
  }

  /**
   * Whether the given command should run when the robot is disabled. Override to return true if the
   * command should run when disabled.
   *
   * @return whether the command should run when the robot is disabled
   */
  default boolean runsWhenDisabled() {
    return false;
  }

  /**
   * Gets the name of this Command. Defaults to the simple class name if not overridden.
   *
   * @return The display name of the Command
   */
  default String getName() {
    return this.getClass().getSimpleName();
  }

  /**
   * Sets the name of this Command. Nullop if not overridden.
   *
   * @param name The display name of the Command.
   */
  default void setName(String name) {}

  /**
   * Decorates this Command with a name. Is an inline function for #setName(String);
   *
   * @param name name
   * @return the decorated Command
   */
  default Command withName(String name) {
    this.setName(name);
    return this;
  }

  /**
   * An enum describing the command's behavior when another command with a shared requirement is
   * scheduled.
   */
  enum InterruptionBehavior {
    /**
     * This command ends, {@link #end(boolean) end(true)} is called, and the incoming command is
     * scheduled normally.
     *
     * <p>This is the default behavior.
     */
    kCancelSelf,
    /** This command continues, and the incoming command is not scheduled. */
    kCancelIncoming
  }
}
