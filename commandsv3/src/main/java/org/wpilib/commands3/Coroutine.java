// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static edu.wpi.first.units.Units.Seconds;
import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.units.measure.Time;
import edu.wpi.first.wpilibj.Timer;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;
import java.util.stream.Collectors;

/**
 * A coroutine object is injected into command's {@link Command#run(Coroutine)} method to allow
 * commands to yield and compositions to run other commands. Commands are considered <i>bound</i> to
 * a coroutine while they're scheduled; attempting to use a coroutine outside the command bound to
 * it will result in an {@code IllegalStateException} being thrown.
 */
public final class Coroutine {
  private final Scheduler m_scheduler;
  private final Continuation m_backingContinuation;

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
   * Yields control back to the scheduler to allow other commands to execute. This can be thought of
   * as "pausing" the currently executing command.
   *
   * @return true
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   */
  public boolean yield() {
    requireMounted();

    try {
      return m_backingContinuation.yield();
    } catch (IllegalStateException e) {
      if ("Pinned: MONITOR".equals(e.getMessage())) {
        // Raised when a continuation yields inside a synchronized block or method:
        // https://github.com/openjdk/jdk/blob/jdk-21%2B35/src/java.base/share/classes/jdk/internal/vm/Continuation.java#L396-L402
        // Note: Not a thing in Java 24+
        // Rethrow with an error message that's more helpful for our users
        throw new IllegalStateException(
            "Coroutine.yield() cannot be called inside a synchronized block or method. "
                + "Consider using a Lock instead of synchronized, "
                + "or rewrite your code to avoid locks and mutexes altogether.",
            e);
      } else {
        // rethrow
        throw e;
      }
    }
  }

  /**
   * Parks the current command. No code in a command declared after calling {@code park()} will be
   * executed. A parked command will never complete naturally and must be interrupted or cancelled.
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
   * Forks off a command. It will run until its natural completion, the parent command exits, or the
   * parent command cancels it. The parent command will continue executing while the forked command
   * runs, and can resync with the forked command using {@link #await(Command)}.
   *
   * <pre>{@code
   * Command example() {
   *   return Command.noRequirements().executing(coroutine -> {
   *     Command inner = ...;
   *     coroutine.fork(inner);
   *     // ... do more things
   *     // then sync back up with the inner command
   *     coroutine.await(inner);
   *   }).named("Example");
   * }
   * }</pre>
   *
   * <p>Note: forking a command that conflicts with a higher-priority command will fail. The forked
   * command will not be scheduled, and the existing command will continue to run.
   *
   * @param commands The commands to fork.
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   */
  public void fork(Command... commands) {
    requireMounted();

    requireNonNullParam(commands, "commands", "Coroutine.fork");
    for (int i = 0; i < commands.length; i++) {
      requireNonNullParam(commands[i], "commands[" + i + "]", "Coroutine.fork");
    }

    // Check for user error; there's no reason to fork conflicting commands simultaneously
    validateNoConflicts(List.of(commands));

    // Shorthand; this is handy for user-defined compositions
    for (var command : commands) {
      m_scheduler.schedule(command);
    }
  }

  /**
   * Forks off some commands. Each command will run until its natural completion, the parent command
   * exits, or the parent command cancels it. The parent command will continue executing while the
   * forked commands run, and can resync with the forked commands using {@link
   * #awaitAll(Collection)}.
   *
   * <pre>{@code
   * Command example() {
   *   return Command.noRequirements().executing(coroutine -> {
   *     Collection<Command> innerCommands = ...;
   *     coroutine.fork(innerCommands);
   *     // ... do more things
   *     // then sync back up with the inner commands
   *     coroutine.awaitAll(innerCommands);
   *   }).named("Example");
   * }
   * }</pre>
   *
   * <p>Note: forking a command that conflicts with a higher-priority command will fail. The forked
   * command will not be scheduled, and the existing command will continue to run.
   *
   * @param commands The commands to fork.
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   */
  public void forkAll(Collection<Command> commands) {
    requireMounted();

    validateNoConflicts(commands);

    requireNonNullParam(commands, "commands", "Coroutine.fork");
    int i = 0;
    for (Command command : commands) {
      requireNonNullParam(command, "commands[" + i + "]", "Coroutine.fork");
      i++;
    }

    // Check for user error; there's no reason to fork conflicting commands simultaneously
    validateNoConflicts(commands);

    // Shorthand; this is handy for user-defined compositions
    for (var command : commands) {
      m_scheduler.schedule(command);
    }
  }

  /**
   * Awaits completion of a command. If the command is not currently scheduled or running, it will
   * be scheduled automatically.
   *
   * @param command the command to await
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   */
  public void await(Command command) {
    requireMounted();

    requireNonNullParam(command, "command", "Coroutine.await");

    m_scheduler.schedule(command);

    while (m_scheduler.isScheduledOrRunning(command)) {
      // If the command is a one-shot, then the schedule call will completely execute the command.
      // There would be nothing to await
      this.yield();
    }
  }

  /**
   * Awaits completion of all given commands. If any command is not current scheduled or running, it
   * will be scheduled.
   *
   * @param commands the commands to await
   * @throws IllegalArgumentException if any of the commands conflict with each other
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   */
  public void awaitAll(Collection<Command> commands) {
    requireMounted();

    requireNonNullParam(commands, "commands", "Coroutine.awaitAll");
    int i = 0;
    for (Command command : commands) {
      requireNonNullParam(command, "commands[" + i + "]", "Coroutine.awaitAll");
      i++;
    }

    validateNoConflicts(commands);

    for (var command : commands) {
      m_scheduler.schedule(command);
    }

    while (commands.stream().anyMatch(m_scheduler::isScheduledOrRunning)) {
      this.yield();
    }
  }

  /**
   * Awaits completion of all given commands. If any command is not current scheduled or running, it
   * will be scheduled.
   *
   * @param commands the commands to await
   * @throws IllegalArgumentException if any of the commands conflict with each other
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   */
  public void awaitAll(Command... commands) {
    awaitAll(Arrays.asList(commands));
  }

  /**
   * Awaits completion of any given commands. Any command that's not already scheduled or running
   * will be scheduled. After any of the given commands completes, the rest will be canceled.
   *
   * @param commands the commands to await
   * @throws IllegalArgumentException if any of the commands conflict with each other
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   */
  public void awaitAny(Collection<Command> commands) {
    requireMounted();

    requireNonNullParam(commands, "commands", "Coroutine.awaitAny");
    int i = 0;
    for (Command command : commands) {
      requireNonNullParam(command, "commands[" + i + "]", "Coroutine.awaitAny");
      i++;
    }

    validateNoConflicts(commands);

    // Schedule anything that's not already queued or running
    for (var command : commands) {
      m_scheduler.schedule(command);
    }

    while (commands.stream().allMatch(m_scheduler::isScheduledOrRunning)) {
      this.yield();
    }

    // At least one command exited; cancel the rest.
    commands.forEach(m_scheduler::cancel);
  }

  /**
   * Awaits completion of any given commands. Any command that's not already scheduled or running
   * will be scheduled. After any of the given commands completes, the rest will be canceled.
   *
   * @param commands the commands to await
   * @throws IllegalArgumentException if any of the commands conflict with each other
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   */
  public void awaitAny(Command... commands) {
    awaitAny(Arrays.asList(commands));
  }

  /**
   * Validates that a set of commands have no internal requirement conflicts. An error is thrown if
   * a conflict is detected.
   *
   * @param commands The commands to validate
   * @throws IllegalArgumentException If at least one pair of commands is found in the input where
   *     both commands have at least one required mechanism in common
   */
  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  private static void validateNoConflicts(Collection<Command> commands) {
    for (var c1 : commands) {
      for (var c2 : commands) {
        if (c1 == c2) {
          // Commands can't conflict with themselves
          continue;
        }

        // TODO: Report all pairs of conflicting commands? Instead of just the first one we find
        if (c1.conflictsWith(c2)) {
          var conflictNames =
              c1.requirements().stream()
                  .filter(c2::requires)
                  .map(Mechanism::getName)
                  .collect(Collectors.joining(", "));

          throw new IllegalArgumentException(
              "Command %s requires mechanisms that are already used by %s. Both require %s"
                  .formatted(c2.name(), c1.name(), conflictNames));
        }
      }
    }
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
   * Yields until a condition is met.
   *
   * @param condition The condition to wait for
   * @throws IllegalStateException if called anywhere other than the coroutine's running command
   */
  public void waitUntil(BooleanSupplier condition) {
    requireMounted();

    requireNonNullParam(condition, "condition", "Coroutine.waitUntil");

    while (!condition.getAsBoolean()) {
      this.yield();
    }
  }

  /**
   * Advanced users only: this permits access to the backing command scheduler to run custom logic
   * not provided by the standard coroutine methods. Any commands manually scheduled through this
   * will be cancelled when the parent command exits - it's not possible to "fork" a command that
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
