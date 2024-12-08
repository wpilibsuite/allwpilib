package edu.wpi.first.wpilibj.commandsv3;

import edu.wpi.first.units.measure.Time;
import java.util.Arrays;
import java.util.Collection;

/**
 * A coroutine object is injected into command's {@link Command#run(Coroutine)} method to allow
 * commands to yield and compositions to run other commands.
 */
public interface Coroutine {
  /**
   * Yields control back to the scheduler to allow other commands to execute. This can be thought of
   * as "pausing" the currently executing command.
   *
   * @return true
   */
  default boolean yield() {
    return scheduler().yield();
  }

  /**
   * Parks the current command. No code in a command declared after calling {@code park()} will be
   * executed. A parked command will never complete naturally and must be interrupted or cancelled.
   */
  @SuppressWarnings("InfiniteLoopStatement")
  default void park() {
    while (true) {
      Coroutine.this.yield();
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
  default void await(Command command) {
    if (!scheduler().isScheduledOrRunning(command)) {
      scheduler().schedule(command);
    }

    if (scheduler().isScheduledOrRunning(command)) {
      // If the command is a one-shot, then the schedule call will completely execute the command.
      // There would be nothing to await
      scheduler().await(command);
    }
  }

  /**
   * Awaits completion of all given commands. If any command is not current scheduled or running,
   * it will be scheduled.
   *
   * @param commands the commands to await
   * @throws IllegalArgumentException if any of the commands conflict with each other
   */
  default void awaitAll(Collection<Command> commands) {
    // Schedule anything that's not already queued or running
    commands.stream()
        .filter(c -> !scheduler().isScheduledOrRunning(c))
        .forEach(scheduler()::schedule);
    scheduler().awaitAll(commands);
  }

  /**
   * Awaits completion of all given commands. If any command is not current scheduled or running,
   * it will be scheduled.
   *
   * @param commands the commands to await
   * @throws IllegalArgumentException if any of the commands conflict with each other
   */
  default void awaitAll(Command... commands) {
    awaitAll(Arrays.asList(commands));
  }

  /**
   * Awaits completion of any given commands. Any command that's not already scheduled or running
   * will be scheduled. After any of the given commands completes, the rest will be canceled.
   *
   * @param commands the commands to await
   * @throws IllegalArgumentException if any of the commands conflict with each other
   */
  default void awaitAny(Collection<Command> commands) {
    // Schedule anything that's not already queued or running
    commands.stream()
        .filter(c -> !scheduler().isScheduledOrRunning(c))
        .forEach(scheduler()::schedule);
    scheduler().awaitAny(commands);
  }

  /**
   * Awaits completion of any given commands. Any command that's not already scheduled or running
   * will be scheduled. After any of the given commands completes, the rest will be canceled.
   *
   * @param commands the commands to await
   * @throws IllegalArgumentException if any of the commands conflict with each other
   */
  default void awaitAny(Command... commands) {
    awaitAny(Arrays.asList(commands));
  }

  /**
   * Waits for some duration of time to elapse. Returns immediately if the given duration is zero or
   * negative. Call this within a command or command composition to introduce a simple delay.
   *
   * <p>For example, a basic autonomous routine that drives straight for 5 seconds:
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
  default void wait(Time duration) {
    await(new WaitCommand(duration));
  }

  /**
   * Advanced users only: this permits access to the backing command scheduler to run custom logic
   * not provided by the standard coroutine methods.
   *
   * @return the command scheduler backing this coroutine
   */
  Scheduler scheduler();
}
