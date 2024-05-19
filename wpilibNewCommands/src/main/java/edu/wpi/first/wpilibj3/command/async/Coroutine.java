package edu.wpi.first.wpilibj3.command.async;

import java.util.Collection;

/**
 * A coroutine object is injected into command's {@link AsyncCommand#run(Coroutine)} method to allow
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
   */
  default void await(AsyncCommand command) {
    if (!scheduler().isScheduledOrRunning(command)) {
      scheduler().schedule(command);
    }
    scheduler().await(command);
  }

  /**
   * Awaits completion of all given commands. Unlike {@link #await(AsyncCommand)}, none of the given
   * commands will be scheduled.
   *
   * @param commands the commands to await
   * @throws IllegalStateException if any of the given commands is not already running or scheduled
   * @throws IllegalStateException if any of the given commands uses a resource not owned by the
   *  calling command
   */
  default void awaitAll(Collection<AsyncCommand> commands) {
    scheduler().awaitAll(commands);
  }

  /**
   * Awaits completion of any given commands. Unlike {@link #await(AsyncCommand)}, none of the given
   * commands will be scheduled. Once any of the givne commands completes, the rest will be
   * canceled.
   *
   * @param commands the commands to await
   * @throws IllegalStateException if any of the given commands is not already running or scheduled
   * @throws IllegalStateException if any of the given commands uses a resource not owned by the
   *  calling command
   */
  default void awaitAny(Collection<AsyncCommand> commands) {
    scheduler().awaitAny(commands);
  }

  /**
   * Advanced users only: this permits access to the backing command scheduler to run custom logic
   * not provided by the standard coroutine methods.
   *
   * @return the command scheduler backing this coroutine
   */
  AsyncScheduler scheduler();
}
