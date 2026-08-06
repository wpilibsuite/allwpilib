// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.wpilib.units.Units.Seconds;
import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;
import org.wpilib.command3.Scheduler.ScheduleResult;
import org.wpilib.system.Timer;
import org.wpilib.units.measure.Time;

/**
 * A coroutine object is injected into command's {@link Command#run(Coroutine)} method to allow
 * commands to yield and compositions to run other commands. Commands are considered <i>bound</i> to
 * a coroutine while they're scheduled; attempting to use a coroutine outside the command bound to
 * it will result in an {@code IllegalStateException} being thrown.
 */
public final class Coroutine {
  private final Scheduler m_scheduler;
  private final Continuation m_backingContinuation;

  private boolean m_cancelOnForkFailure = true;
  private ForkResult m_lastForkFailure = null;
  private boolean m_cancellationRequested = false;

  /**
   * Creates a new coroutine. Package-private; only the scheduler should be creating these.
   *
   * @param scheduler The scheduler running the coroutine
   * @param scope The continuation scope the coroutine's backing continuation runs in
   * @param callback The callback for the continuation to execute when mounted. Often a command
   *     function's body.
   */
  Coroutine(Scheduler scheduler, ContinuationScope scope, Consumer<Coroutine> callback) {
    m_scheduler = scheduler;
    m_backingContinuation = new Continuation(scope, () -> callback.accept(this));
  }

  /**
   * Configures the coroutine to cancel itself if a child command couldn't be forked. This setting
   * defaults to {@code true}, but can be changed to allow the coroutine to continue running after a
   * fork failure.
   *
   * <p>This setting only affects commands forked by using the coroutine methods {@code fork},
   * {@code await}, {@code awaitAll}, or {@code awaitAny}. Commands scheduled directly with {@link
   * Scheduler#schedule(Command)} or via inner trigger bindings will not be affected.
   *
   * @param cancelOnForkFailure true to make the coroutine cancel itself if a child command couldn't
   *     be forked, false to allow the coroutine to continue running after a fork failure
   * @see #fork(Command...)
   * @see #fork(Collection)
   * @see #await(Command)
   * @see #awaitAll(Command...)
   * @see #awaitAll(Collection)
   * @see #awaitAny(Command...)
   * @see #awaitAny(Collection)
   */
  public void setCancelOnForkFailure(boolean cancelOnForkFailure) {
    m_cancelOnForkFailure = cancelOnForkFailure;
  }

  /**
   * Checks if the coroutine is currently configured to cancel itself if a child command couldn't be
   * forked.
   *
   * @return true if the coroutine will cancel itself if a child command couldn't be forked, false
   *     otherwise
   * @see #setCancelOnForkFailure(boolean)
   */
  public boolean isCancelOnForkFailure() {
    return m_cancelOnForkFailure;
  }

  // Package-private. User code can never access this because setting the flag happens immediately
  // before the coroutine yields itself.
  boolean isInterruptRequested() {
    return m_cancelOnForkFailure && m_lastForkFailure != null;
  }

  /**
   * Requests early cancellation of the coroutine and the command bound to it. Any child commands
   * will be canceled as well, as commands can never outlive their parent, but the parent command
   * will not be canceled.
   *
   * <p>Team code shouldn't need to use this; instead, use a {@code return} statement in your
   * command's function body to exit early. This method is primarily to be used by internal APIs
   * that don't have access to the command's function body.
   */
  public void requestCancellation() {
    requireMounted();

    // Set the flag and immediately yield. The scheduler will check the flag after the yield call
    // and cancel the command and its descendants.
    m_cancellationRequested = true;
    this.yield();
  }

  // package-private; setting the flag will immediately yield, so user code can never access this
  // in a state where it returns anything other than `false`
  boolean isCancellationRequested() {
    return m_cancellationRequested;
  }

  ForkResult getForkResult() {
    return m_lastForkFailure;
  }

  /**
   * Yields control back to the scheduler to allow other commands to execute. This can be thought of
   * as "pausing" the currently executing command.
   *
   * @return true
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   */
  public boolean yield() {
    requireMounted();

    return m_backingContinuation.yield();
  }

  /**
   * Parks the current command. No code in a command declared after calling {@code park()} will be
   * executed. A parked command will never complete naturally and must be interrupted or canceled.
   *
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   */
  @SuppressWarnings("InfiniteLoopStatement")
  public void park() {
    requireMounted();

    while (true) {
      // 'this' is required because 'yield' is a semi-keyword and needs to be qualified
      this.yield();
    }
  }

  /**
   * Represents the result of a fork operation that attempts to schedule multiple commands
   * concurrently. The class provides information about the success or failure of the fork operation
   * as well as utility methods to handle the outcome.
   *
   * <p>Instances of this class are immutable and provide methods to access the details of the
   * forked and failed commands.
   */
  public final class ForkResult {
    private final List<? extends Command> m_forkedCommands;
    private final List<? extends ScheduleResult.Failure> m_failedCommands;

    // private - only the coroutine can construct these
    private ForkResult(
        Collection<? extends Command> forkedCommands,
        List<? extends ScheduleResult.Failure> failedCommands) {
      m_forkedCommands = List.copyOf(forkedCommands);
      m_failedCommands = List.copyOf(failedCommands);
    }

    /**
     * Checks if the fork request was successful.
     *
     * @return True if no commands failed to be forked, false if at least one failed.
     */
    public boolean successful() {
      return m_failedCommands.isEmpty();
    }

    /**
     * Checks if the fork request failed.
     *
     * @return True if at least one command failed to be forked, false if all commands were
     *     successfully forked.
     */
    public boolean failed() {
      return !successful();
    }

    /**
     * Checks if the fork request was partially successful. A partial success is defined as at least
     * one command being forked but at least one failing to be forked. This can happen if a child
     * immediately schedules a grandchild with a higher priority than a later sibling that shares
     * requirements with that grandchild.
     *
     * @return True if at least one command was forked but at least one failed to be forked, false
     *     if all commands were successfully forked or none were forked.
     */
    public boolean partialSuccess() {
      return !m_forkedCommands.isEmpty() && !m_failedCommands.isEmpty();
    }

    /**
     * Waits for all forked commands to complete. Unlike a fork-then-await pattern, which will
     * reschedule any forked commands that already completed, {@code awaitCompletion()} will only
     * wait for the forked commands that are still running.
     *
     * <p>This method does nothing if no commands were successfully forked.
     */
    public void awaitCompletion() {
      for (Command command : m_forkedCommands) {
        if (m_scheduler.isRunning(command)) {
          Coroutine.this.yield();
        }
      }
    }

    /**
     * Returns the list of commands that failed to be forked. This list is read-only and cannot be
     * modified.
     *
     * @return The list of failed commands.
     */
    public List<? extends ScheduleResult.Failure> getFailedCommands() {
      return m_failedCommands;
    }

    /**
     * Returns the list of commands that were successfully forked. This list is read-only and cannot
     * be modified.
     *
     * @return The list of forked commands.
     */
    public List<? extends Command> getForkedCommands() {
      return m_forkedCommands;
    }
  }

  /**
   * Checks that all the given commands can be forked.
   *
   * @param commands The commands to check
   * @return A failure-type ForkResult if the commands can't all be scheduled, or null on success.
   */
  private ForkResult checkAllForkable(Collection<? extends Command> commands) {
    // Check for user error; there's no reason to fork conflicting commands simultaneously
    // Because this is a bug in user code, throw an error instead of returning a failure result
    ConflictDetector.throwIfConflicts(commands);

    var schedulable = new ArrayList<Command>();
    var unschedulable = new ArrayList<ScheduleResult.Failure>();
    for (var command : commands) {
      var result = m_scheduler.isSchedulable(command);
      if (result instanceof ScheduleResult.Failure fail) {
        unschedulable.add(fail);
      } else {
        schedulable.add(command);
      }
    }

    var result = new ForkResult(schedulable, unschedulable);
    if (!unschedulable.isEmpty()) {
      if (m_cancelOnForkFailure) {
        // Canceling on fork failure means no coroutine or user code gets to run to handle the
        // failure result
        m_lastForkFailure = result;
        this.yield();
        // could throw an IllegalStateException, but probably shouldn't crash user code
        return result;
      } else {
        // always return the failure object if self-cancellation is disabled
        return result;
      }
    }

    return result;
  }

  /**
   * Performs the actual fork operation for a collection of commands. This is used in conjunction
   * with {@link #checkAllForkable(Collection)} in case a child forks a grandchild that conflicts
   * with another entry in the set.
   *
   * <p>Given this setup, child1 and child2 are both forkable at the same time (no shared
   * requirements and no conflicts with running commands), but child1 immediately forks a higher
   * priority command than child2, which prevents child2 from being scheduled.
   *
   * <pre>{@code
   * parent:
   *   child1:
   *     fork priority(1000, mech)
   *   child2: priority(0, mech))
   * }</pre>
   *
   * @param commands The commands to fork
   * @return A failure result if at least one of the given commands was unable to be scheduled, or a
   *     success result if all commands were scheduled.
   */
  private ForkResult doFork(Collection<? extends Command> commands) {
    var successes = new ArrayList<Command>();
    var fails = new ArrayList<ScheduleResult.Failure>();
    for (var command : commands) {
      var result = m_scheduler.schedule(command);
      if (result instanceof ScheduleResult.Failure fail) {
        fails.add(fail);
      } else {
        successes.add(command);
      }
    }
    ForkResult result = new ForkResult(successes, fails);

    if (!fails.isEmpty() && m_cancelOnForkFailure) {
      // At least one of the commands couldn't be scheduled due to runtime behavior that can't be
      // checked ahead of time by `checkAllForkable`.
      //
      // We can't roll back the partial success; the commands that successfully started will have
      // already affected the state of the robot and may have interrupted previously running
      // commands, which we can't restore. The two options are to either cancel the entire command
      // composition - which is the default behavior - or to allow user code to handle the failure
      // and do something with it (eg trying something else, retrying, etc).
      m_lastForkFailure = result;
      this.yield();
      return result; // note: unreachable
    }

    return result;
  }

  /**
   * Schedules child commands and then immediately returns. The child commands will run until their
   * natural completion, the parent command exits, or the parent command cancels it.
   *
   * <p>If any child command is unable to be scheduled per the contract of {@link
   * Scheduler#isSchedulable(Command)}, then none of the commands will be scheduled and the method
   * call will return a {@link ForkResult} containing the unschedulable commands. If the coroutine
   * is configured to cancel on fork failure, then the coroutine will be canceled and user code will
   * not get a chance to handle the failure result.
   *
   * <p>This is a nonblocking operation. To fork and then wait for the child command to complete,
   * use {@link #await(Command)}.
   *
   * <p>The parent command will continue executing while the child command runs, and can resync with
   * the child command using {@link #await(Command)}.
   *
   * <pre>{@code
   * Command example() {
   *   return Command.noRequirements(coroutine -> {
   *     Command child = ...;
   *     ForkResult childTask = coroutine.fork(child);
   *     // ... do more things
   *     // then sync back up with the child command
   *     if (childTask.successful()) {
   *       childTask.awaitCompletion();
   *     }
   *   }).named("Example");
   * }
   * }</pre>
   *
   * @param commands The commands to fork.
   * @return a result indicating whether the fork was successful or if the commands could not be
   *     forked
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   * @see #await(Command)
   * @see #setCancelOnForkFailure(boolean)
   */
  public ForkResult fork(Command... commands) {
    requireMounted();

    requireNonNullParam(commands, "commands", "Coroutine.fork");
    for (int i = 0; i < commands.length; i++) {
      requireNonNullParam(commands[i], "commands[" + i + "]", "Coroutine.fork");
    }

    var forkableCheck = checkAllForkable(Arrays.asList(commands));
    if (forkableCheck.failed()) {
      return forkableCheck;
    }

    return doFork(Arrays.asList(commands));
  }

  /**
   * Forks off some commands. Each command will run until its natural completion, the parent command
   * exits, or the parent command cancels it. The parent command will continue executing while the
   * forked commands run, and can resync with the forked commands using {@link
   * #awaitAll(Collection)}.
   *
   * <p>If any child command is unable to be scheduled per the contract of {@link
   * Scheduler#isSchedulable(Command)}, then none of the commands will be scheduled and the method
   * call will return a {@link ForkResult} containing the unschedulable commands. If the coroutine
   * is configured to cancel on fork failure, then the coroutine will be canceled and user code will
   * not get a chance to handle the failure result.
   *
   * <pre>{@code
   * Command example() {
   *   return Command.noRequirements(coroutine -> {
   *     Collection<Command> innerCommands = ...;
   *     ForkResult childTask = coroutine.fork(innerCommands);
   *     // ... do more things
   *     // then sync back up with the inner commands
   *     if (childTask.successful()) {
   *       childTask.awaitCompletion();
   *     }
   *   }).named("Example");
   * }
   * }</pre>
   *
   * @param commands The commands to fork.
   * @return a result indicating whether the fork was successful or if the commands could not be
   *     forked
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   * @see #setCancelOnForkFailure(boolean)
   */
  public ForkResult fork(Collection<? extends Command> commands) {
    return fork(commands.toArray(Command[]::new));
  }

  /**
   * Awaits completion of a command. If the command is not currently scheduled or running, it will
   * be scheduled automatically. This is a blocking operation and will not return until the command
   * completes or has been interrupted by another command scheduled by the same parent.
   *
   * <p>If the child command is unable to be scheduled per the contract of {@link
   * Scheduler#isSchedulable(Command)}, then it will not be scheduled and the method call will
   * return a {@link ForkResult} containing the command. If the coroutine is configured to cancel on
   * fork failure, then the coroutine will be canceled and user code will not get a chance to handle
   * the failure result.
   *
   * @param command the command to await
   * @return a result indicating whether the await was successful or if the commands could not be
   *     awaited
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   * @see #fork(Command...)
   * @see #setCancelOnForkFailure(boolean)
   */
  public ForkResult await(Command command) {
    requireMounted();

    requireNonNullParam(command, "command", "Coroutine.await");

    var forkableCheck = checkAllForkable(List.of(command));
    if (forkableCheck.failed()) {
      return forkableCheck;
    }

    // Don't need doFork() because there's no chance of sibling conflicts
    m_scheduler.schedule(command);

    while (m_scheduler.isScheduledOrRunning(command)) {
      // If the command is a one-shot, then the schedule call will completely execute the command.
      // There would be nothing to await
      this.yield();
    }

    return new ForkResult(List.of(command), List.of());
  }

  /**
   * Awaits completion of all given commands. If any command is not currently scheduled or running,
   * it will be scheduled.
   *
   * <p>If any child command is unable to be scheduled per the contract of {@link
   * Scheduler#isSchedulable(Command)}, then none of the commands will be scheduled and the method
   * call will return a {@link ForkResult} containing the unschedulable commands. If the coroutine
   * is configured to cancel on fork failure, then the coroutine will be canceled and user code will
   * not get a chance to handle the failure result.
   *
   * @param commands the commands to await
   * @return a result indicating whether the await was successful or if the commands could not be
   *     awaited
   * @throws IllegalArgumentException if any of the commands conflict with each other
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   * @see #setCancelOnForkFailure(boolean)
   */
  public ForkResult awaitAll(Collection<? extends Command> commands) {
    requireMounted();

    requireNonNullParam(commands, "commands", "Coroutine.awaitAll");
    int i = 0;
    for (Command command : commands) {
      requireNonNullParam(command, "commands[" + i + "]", "Coroutine.awaitAll");
      i++;
    }

    var forkableCheck = checkAllForkable(commands);
    if (forkableCheck.failed()) {
      return forkableCheck;
    }

    var forkResult = doFork(commands);
    if (forkResult.failed()) {
      return forkResult;
    }

    while (commands.stream().anyMatch(m_scheduler::isScheduledOrRunning)) {
      this.yield();
    }

    return forkResult;
  }

  /**
   * Awaits completion of all given commands. If any command is not currently scheduled or running,
   * it will be scheduled.
   *
   * <p>If any child command is unable to be scheduled per the contract of {@link
   * Scheduler#isSchedulable(Command)}, then none of the commands will be scheduled and the method
   * call will return a {@link ForkResult} containing the unschedulable commands. If the coroutine
   * is configured to cancel on fork failure, then the coroutine will be canceled and user code will
   * not get a chance to handle the failure result.
   *
   * @param commands the commands to await
   * @return a result indicating whether the await was successful or if the commands could not be
   *     awaited
   * @throws IllegalArgumentException if any of the commands conflict with each other
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   * @see #setCancelOnForkFailure(boolean)
   */
  public ForkResult awaitAll(Command... commands) {
    return awaitAll(Arrays.asList(commands));
  }

  /**
   * Awaits completion of any given commands. Any command that's not already scheduled or running
   * will be scheduled. After any of the given commands complete, the rest will be canceled.
   *
   * <p>If any child command is unable to be scheduled per the contract of {@link
   * Scheduler#isSchedulable(Command)}, then none of the commands will be scheduled and the method
   * call will return a {@link ForkResult} containing the unschedulable commands. If the coroutine
   * is configured to cancel on fork failure, then the coroutine will be canceled and user code will
   * not get a chance to handle the failure result.
   *
   * @param commands the commands to await
   * @return a result indicating whether the await was successful or if the commands could not be
   *     awaited
   * @throws IllegalArgumentException if any of the commands conflict with each other
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   * @see #setCancelOnForkFailure(boolean)
   */
  public ForkResult awaitAny(Collection<? extends Command> commands) {
    requireMounted();

    requireNonNullParam(commands, "commands", "Coroutine.awaitAny");
    int i = 0;
    for (Command command : commands) {
      requireNonNullParam(command, "commands[" + i + "]", "Coroutine.awaitAny");
      i++;
    }

    var forkableCheck = checkAllForkable(commands);
    if (forkableCheck.failed()) {
      return forkableCheck;
    }

    var forkResult = doFork(commands);
    if (forkResult.failed()) {
      return forkResult;
    }

    while (commands.stream().allMatch(m_scheduler::isScheduledOrRunning)) {
      this.yield();
    }

    // At least one command exited; cancel the rest.
    commands.forEach(m_scheduler::cancel);

    return forkResult;
  }

  /**
   * Awaits completion of any given commands. Any command that's not already scheduled or running
   * will be scheduled. After any of the given commands completes, the rest will be canceled.
   *
   * <p>If any child command is unable to be scheduled per the contract of {@link
   * Scheduler#isSchedulable(Command)}, then none of the commands will be scheduled and the method
   * call will return a {@link ForkResult} containing the unschedulable commands. If the coroutine
   * is configured to cancel on fork failure, then the coroutine will be canceled and user code will
   * not get a chance to handle the failure result.
   *
   * @param commands the commands to await
   * @return a result indicating whether the await was successful or if the commands could not be
   *     awaited
   * @throws IllegalArgumentException if any of the commands conflict with each other
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   * @see #setCancelOnForkFailure(boolean)
   */
  public ForkResult awaitAny(Command... commands) {
    return awaitAny(Arrays.asList(commands));
  }

  /**
   * Waits for some duration of time to elapse. Returns immediately if the given duration is zero or
   * negative. Call this within a command or command composition to introduce a simple delay.
   *
   * <p>For example, a basic autonomous routine that drives straight for 5 seconds:
   *
   * <pre>{@code
   * Command timedDrive() {
   *   return drivebase.run(coroutine -> {
   *     drivebase.tankDrive(1, 1);
   *     coroutine.wait(Seconds.of(5));
   *     drivebase.stop();
   *   }).named("Timed Drive");
   * }
   * }</pre>
   *
   * <p>Note that the resolution of the wait period is equal to the period at which {@link
   * Scheduler#run()} is called by the robot program. If using a 20 millisecond update period, the
   * wait will be rounded up to the nearest 20 millisecond interval; in this scenario, calling
   * {@code wait(Milliseconds.of(1))} and {@code wait(Milliseconds.of(19))} would have identical
   * effects.
   *
   * <p>Very small loop times near the loop period are sensitive to the order in which commands are
   * executed. If a command waits for 10 ms at the end of a scheduler cycle, and then all the
   * commands that ran before it complete or exit, and then the next cycle starts immediately, the
   * wait will be evaluated at the <i>start</i> of that next cycle, which occurred less than 10 ms
   * later. Therefore, the wait will see not enough time has passed and only exit after an
   * additional cycle elapses, adding an unexpected extra 20 ms to the wait time. This becomes less
   * of a problem with smaller loop periods, as the extra 1-loop delay becomes smaller.
   *
   * @param duration the duration of time to wait
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   */
  public void wait(Time duration) {
    requireMounted();

    requireNonNullParam(duration, "duration", "Coroutine.wait");

    var timer = new Timer();
    timer.start();
    while (!timer.hasElapsed(duration.in(Seconds))) {
      this.yield();
    }
  }

  /**
   * Represents the result of a call to {@link Coroutine#waitUntil(BooleanSupplier)} or {@link
   * Coroutine#waitUntil(BooleanSupplier, Time)}.
   */
  public enum WaitResult {
    /** A call to {@code waitUntil} has met its condition. */
    CONDITION_MET {
      @Override
      public boolean timedOut() {
        return false;
      }
    },
    /** A call to {@link Coroutine#waitUntil(BooleanSupplier, Time)} has timed out. */
    TIMED_OUT {
      @Override
      public boolean timedOut() {
        return true;
      }
    },
    ;

    /**
     * Checks if the wait has timed out.
     *
     * @return true if this result was a timeout, false if the condition was met without timing out.
     */
    public abstract boolean timedOut();
  }

  /**
   * Yields until a condition is met. This method will <b>only</b> return once the condition is met;
   * if the condition never becomes true or is delayed longer than you expect, this method will
   * block indefinitely. {@link #waitUntil(BooleanSupplier, Time)} is an alternative that will only
   * wait up until a maximum duration before exiting.
   *
   * @param condition The condition to wait for
   * @return {@link WaitResult#CONDITION_MET}
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   * @see #waitUntil(BooleanSupplier, Time)
   */
  public WaitResult waitUntil(BooleanSupplier condition) {
    requireMounted();

    requireNonNullParam(condition, "condition", "Coroutine.waitUntil");

    while (!condition.getAsBoolean()) {
      this.yield();
    }

    return WaitResult.CONDITION_MET;
  }

  /**
   * Yields until a condition is met, waiting for up to a specified duration. If the condition is
   * not met before the timeout duration elapses, this method stops waiting and returns {@link
   * WaitResult#TIMED_OUT}.
   *
   * <p>Teams may use this method for waiting for conditions in autonomous or automated routines to
   * be resilient to hardware faults (e.g., a mechanism not reaching a target state or a faulty
   * sensor).
   *
   * <pre>{@code
   * Command.noRequirements(coroutine -> {
   *   coroutine.fork(elevator.up());
   *   Coroutine.WaitResult upResult = coroutine.waitUntil(elevator::atTop, Seconds.of(1.25));
   *   if (upResult.timedOut()) {
   *     // We've waited 1.25 seconds and the elevator still has not reached the top. It may have
   *     // jammed or there may be a fault with the sensors. In this example, we set a driverstation
   *     // alert and exit early - team code may want to take other approaches like retrying the
   *     // command or falling back to a secondary behavior that doesn't need the elevator to be up.
   *     elevator.setJamAlert();
   *     return;
   *   }
   *
   *   // The elevator reached the top within 1.25 seconds. Clear the alert and continue.
   *   elevator.clearJamAlert();
   *
   *   // ... more commands ...
   * })
   * }</pre>
   *
   * @param condition The condition to wait for
   * @param timeout The maximum duration to wait
   * @return {@link WaitResult#CONDITION_MET} if the condition was met within the specified timeout,
   *     or {@link WaitResult#TIMED_OUT} if the timeout duration elapsed before the condition was
   *     met.
   * @see #waitUntil(BooleanSupplier)
   */
  public WaitResult waitUntil(BooleanSupplier condition, Time timeout) {
    requireMounted();

    requireNonNullParam(condition, "condition", "Coroutine.waitUntil");
    requireNonNullParam(timeout, "timeout", "Coroutine.waitUntil");

    Timer timer = new Timer();
    timer.start();

    while (!condition.getAsBoolean()) {
      if (timer.hasElapsed(timeout)) {
        return WaitResult.TIMED_OUT;
      } else {
        this.yield();
      }
    }

    return WaitResult.CONDITION_MET;
  }

  /**
   * Advanced users only: this permits access to the backing command scheduler to run custom logic
   * not provided by the standard coroutine methods. Any commands manually scheduled through this
   * will be canceled when the parent command exits - it's not possible to "fork" a command that
   * lives longer than the command that scheduled it.
   *
   * @return the command scheduler backing this coroutine
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   */
  public Scheduler scheduler() {
    requireMounted();

    return m_scheduler;
  }

  private boolean isMounted() {
    return m_backingContinuation.isMounted();
  }

  private void requireMounted() {
    // Note: attempting to yield() outside a command will already throw an error due to the
    // continuation being unmounted, but other actions like forking and awaiting should also
    // throw errors. For consistent messaging, we use this helper in all places, not just the
    // ones that interact with the backing continuation.

    if (isMounted()) {
      return;
    }

    throw new IllegalStateException("Coroutines can only be used by the command bound to them");
  }

  // Package-private for interaction with the scheduler.
  // These functions are not intended for team use.

  void runToYieldPoint() {
    m_backingContinuation.run();
  }

  void mount() {
    Continuation.mountContinuation(m_backingContinuation);
  }

  boolean isDone() {
    return m_backingContinuation.isDone();
  }
}
