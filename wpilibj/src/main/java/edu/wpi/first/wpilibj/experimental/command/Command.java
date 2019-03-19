package edu.wpi.first.wpilibj.experimental.command;

import java.util.HashSet;
import java.util.Set;
import java.util.function.BooleanSupplier;

/**
 * A state machine representing a complete action to be performed by the robot.  Commands are
 * run by the {@link CommandScheduler}, and can be composed into CommandGroups to allow users to
 * build complicated multi-step actions without the need to roll the state machine logic themselves.
 *
 * <p>Commands are run synchronously from the main robot loop; no multithreading is used, unless
 * specified explicitly from the command implementation.
 */
@SuppressWarnings("PMD.TooManyMethods")
public interface Command {

  /**
   * The initial subroutine of a command.  Called once when the command is initially scheduled.
   */
  default void initialize() {
  }

  /**
   * The main body of a command.  Called repeatedly while the command is scheduled.
   */
  default void execute() {
  }

  /**
   * The action to take when the command is forced to end early.  Called either when the command
   * is interrupted by another command with a shared requirement, or when the command is explicitly
   * canceled.  Defaults to call the ordinary end() method.
   */
  default void interrupted() {
    end();
  }

  /**
   * The action to take when the command ends normally.  Called once after the command reports that
   * it is finished, and called by default from interrupted().
   */
  default void end() {
  }

  /**
   * Whether the command has finished.  Once a command finishes, the scheduler will call its
   * end() method and un-schedule it.
   *
   * @return whether the command has finished.
   */
  default boolean isFinished() {
    return false;
  }

  /**
   * Specifies the set of subsystems used by this command.  Two commands cannot use the same
   * subsystem at the same time.  If the command is scheduled as interruptible and another
   * command is scheduled that shares a requirement, the command will be interrupted.  Else,
   * the command will not be scheduled.  If no subsystems are required, return an empty set
   * (as is done in the default implementation).
   *
   * <p>Note: it is recommended that user implementations contain the requirements as a field,
   * and return that field here, rather than allocating a new set every time this is called.
   *
   * @return the set of subsystems that are required
   */
  default Set<Subsystem> getRequirements() {
    return new HashSet<>();
  }

  /**
   * Decorates this command with a timeout.  If the specified timeout is exceeded before the command
   * finishes normally, the command will be interrupted and un-scheduled.  Note that the
   * timeout only applies to the command returned by this method; the calling command is
   * not itself changed.
   *
   * <p>Note: This decorator works by composing this command within a CommandGroup.  The command
   * cannot be used independently after being decorated, or be re-decorated with a different
   * decorator, unless it is manually cleared from the list of grouped commands with
   * {@link CommandGroupBase#clearGroupedCommand(Command)}.  The decorated command can, however, be
   * further decorated without issue.
   *
   * @param seconds the timeout duration
   * @return the command with the timeout added
   */
  default Command withTimeout(double seconds) {
    return new ParallelRaceGroup(this, new WaitCommand(seconds));
  }

  /**
   * Decorates this command with an interrupt condition.  If the specified condition becomes true
   * before the command finishes normally, the command will be interrupted and un-scheduled.
   * Note that this only applies to the command returned by this method; the calling command
   * is not itself changed.
   *
   * <p>Note: This decorator works by composing this command within a CommandGroup.  The command
   * cannot be used independently after being decorated, or be re-decorated with a different
   * decorator, unless it is manually cleared from the list of grouped commands with
   * {@link CommandGroupBase#clearGroupedCommand(Command)}.  The decorated command can, however, be
   * further decorated without issue.
   *
   * @param condition the interrupt condition
   * @return the command with the interrupt condition added
   */
  default Command withInterruptCondition(BooleanSupplier condition) {
    return new ParallelRaceGroup(this, new EndOnConditionCommand(condition));
  }

  /**
   * Schedules this command.
   *
   * @param interruptible whether this command can be interrupted by another command that
   *                      shares one of its requirements
   */
  default void schedule(boolean interruptible) {
    CommandScheduler.getInstance().scheduleCommand(this, interruptible);
  }

  /**
   * Schedules this command, defaulting to interruptible.
   */
  default void schedule() {
    schedule(true);
  }

  /**
   * Cancels this command.  Will call the command's interrupted() method.
   * Commands will be canceled even if they are not marked as interruptible.
   */
  default void cancel() {
    CommandScheduler.getInstance().cancelCommand(this);
  }

  /**
   * Whether or not the command is currently scheduled.  Note that this does not detect whether
   * the command is being run by a CommandGroup, only whether it is directly being run by
   * the scheduler.
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
  default boolean requires(Subsystem requirement) {
    return getRequirements().contains(requirement);
  }

  /**
   * Whether the given command should run when the robot is disabled.  Override to return true
   * if the command should run when disabled.
   *
   * @return whether the command should run when the robot is disabled
   */
  default boolean runsWhenDisabled() {
    return false;
  }

  default String getName() {
    return this.getClass().getSimpleName();
  }
}
