// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import edu.wpi.first.units.measure.Time;
import java.util.Arrays;
import java.util.Collection;
import java.util.function.Consumer;

/**
 * A coroutine object is injected into command's {@link Command#run(Coroutine)} method to allow
 * commands to yield and compositions to run other commands.
 */
public final class Coroutine {
  private final Scheduler scheduler;
  private final Continuation backingContinuation;

  Coroutine(Scheduler scheduler, ContinuationScope scope, Consumer<Coroutine> callback) {
    this.scheduler = scheduler;
    this.backingContinuation =
        new Continuation(
            scope,
            () -> {
              callback.accept(this);
            });
  }

  /**
   * Yields control back to the scheduler to allow other commands to execute. This can be thought of
   * as "pausing" the currently executing command.
   *
   * @return true
   */
  public boolean yield() {
    return backingContinuation.yield();
  }

  /**
   * Parks the current command. No code in a command declared after calling {@code park()} will be
   * executed. A parked command will never complete naturally and must be interrupted or cancelled.
   */
  @SuppressWarnings("InfiniteLoopStatement")
  public void park() {
    while (true) {
      Coroutine.this.yield();
    }
  }

  /**
   * Forks off a command. It will run until its natural completion, the parent command exits,
   * or the parent command cancels it. The parent command will continue executing while the
   * forked command runs, and can resync with the forked command using {@link #await(Command)}.
   * <p>
   * {@snippet lang = java:
   * Command example() {
   *   return Command.noRequirements((coroutine) -> {
   *     Command inner = ...;
   *     coroutine.fork(inner);
   *     // ... do more things
   *     // then sync back up with the inner command
   *     coroutine.await(inner);
   *   }).named("Example");
   * }
   * }
   *
   * @param commands The commands to fork.
   */
  public void fork(Command... commands) {
    // Shorthand; this is handy for user-defined compositions
    for (var command : commands) {
      scheduler.schedule(command);
    }
  }

  /**
   * Awaits completion of a command. If the command is not currently scheduled or running, it will
   * be scheduled automatically.
   *
   * @param command the command to await
   * @throws IllegalStateException if the given command uses a resource not owned by the calling
   *     command
   */
  public void await(Command command) {
    if (!scheduler.isScheduledOrRunning(command)) {
      scheduler.schedule(command);
    }

    while (scheduler.isScheduledOrRunning(command)) {
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
   */
  public void awaitAll(Collection<Command> commands) {
    // Schedule anything that's not already queued or running
    for (var command : commands) {
      if (!scheduler.isScheduledOrRunning(command)) {
        scheduler.schedule(command);
      }
    }

    while (commands.stream().anyMatch(scheduler::isScheduledOrRunning)) {
      this.yield();
    }

    // The scheduler will clean up anything that's still running at this point
  }

  /**
   * Awaits completion of all given commands. If any command is not current scheduled or running, it
   * will be scheduled.
   *
   * @param commands the commands to await
   * @throws IllegalArgumentException if any of the commands conflict with each other
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
   */
  public void awaitAny(Collection<Command> commands) {
    // Schedule anything that's not already queued or running
    for (var command : commands) {
      if (!scheduler.isScheduledOrRunning(command)) {
        scheduler.schedule(command);
      }
    }

    while (commands.stream().allMatch(scheduler::isScheduledOrRunning)) {
      this.yield();
    }
  }

  /**
   * Awaits completion of any given commands. Any command that's not already scheduled or running
   * will be scheduled. After any of the given commands completes, the rest will be canceled.
   *
   * @param commands the commands to await
   * @throws IllegalArgumentException if any of the commands conflict with each other
   */
  public void awaitAny(Command... commands) {
    awaitAny(Arrays.asList(commands));
  }

  /**
   * Waits for some duration of time to elapse. Returns immediately if the given duration is zero or
   * negative. Call this within a command or command composition to introduce a simple delay.
   *
   * <p>For example, a basic autonomous routine that drives straight for 5 seconds:
   *
   * {@snippet lang = java :
   * Command timedDrive() {
   *   return drivebase.run((coroutine) -> {
   *     drivebase.tankDrive(1, 1);
   *     coroutine.wait(Seconds.of(5));
   *     drivebase.stop();
   *   }).named("Timed Drive");
   * }
   * }
   *
   * @param duration the duration of time to wait
   */
  public void wait(Time duration) {
    await(new WaitCommand(duration));
  }

  /**
   * Advanced users only: this permits access to the backing command scheduler to run custom logic
   * not provided by the standard coroutine methods. Any commands manually scheduled through this
   * will be cancelled when the parent command exits - it's not possible to "fork" a command that
   * lives longer than the command that scheduled it.
   *
   * @return the command scheduler backing this coroutine
   */
  public Scheduler scheduler() {
    return scheduler;
  }

  // Package-private for interaction with the scheduler.
  // These functions are not intended for team use.

  void runToYieldPoint() {
    backingContinuation.run();
  }

  void mount() {
    Continuation.mountContinuation(backingContinuation);
  }

  boolean isDone() {
    return backingContinuation.isDone();
  }
}
