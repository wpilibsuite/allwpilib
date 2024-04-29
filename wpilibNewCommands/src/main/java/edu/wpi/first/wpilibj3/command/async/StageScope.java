package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import java.time.Instant;
import java.util.Collections;
import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.StructuredTaskScope;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicReference;

/**
 * A task scope that automatically shuts down when a specific set of required tasks have completed.
 * @param <T> the type of values that may be returned by tasks in the scope
 */
@SuppressWarnings("preview")
public class StageScope<T> extends StructuredTaskScope<T> {
  /** The scheduler that actually manages concurrent tasks. */
  private final AsyncScheduler scheduler;
  /** The remaining required tasks in the scope that need to complete. */
  private final Set<? extends Callable<? extends T>> remainingRequirements;
  /** The results of the completed tasks. */
  private final Set<T> results = Collections.synchronizedSet(new HashSet<>());
  /** The exception thrown by the first failing task. */
  private final AtomicReference<Throwable> exception = new AtomicReference<>(null);

  /**
   * Creates a new task scope for concurrent commands.
   * @param scheduler the command scheduler to handle the commands
   * @param requirements the set of tasks required to complete before the scope can shut down
   */
  public StageScope(AsyncScheduler scheduler, Set<? extends Callable<? extends T>> requirements) {
    this.scheduler = scheduler;
    this.remainingRequirements = new HashSet<>(requirements);
  }

  @Override
  protected void handleComplete(Subtask<? extends T> subtask) {
    switch (subtask.state()) {
      case SUCCESS -> results.add(subtask.get());
      case FAILED -> exception.compareAndSet(null, subtask.exception());
      case UNAVAILABLE ->
          throw new IllegalStateException("Subtask is unavailable in handleComplete!");
    }

    Callable<?> task = subtask.task();

    // Unwrap a command task, if necessary
    if (task instanceof CommandTask<?>(var _scheduler, var command)) {
      task = command;
    }
    remainingRequirements.remove(task);

    if (remainingRequirements.isEmpty()) {
      shutdown();
    }
  }

  /**
   * Forks an async command in this scope.
   * @param command the command to work
   * @return the forked subtask
   */
  public Subtask<?> fork(AsyncCommand command) {
    return fork(new CommandTask<>(scheduler, command));
  }

  @Override
  public StageScope<T> join() throws InterruptedException {
    super.join();
    return this;
  }

  @Override
  public StageScope<T> joinUntil(Instant deadline) throws InterruptedException, TimeoutException {
    super.joinUntil(deadline);
    return this;
  }

  /**
   * Wait for all required subtasks started in this task scope to finish or the task scope to
   * shut down, waiting for up to the given timeout period to elapse.
   *
   * <p> This method waits for all subtasks by waiting for all threads {@linkplain
   * #fork(Callable) started} in this task scope to finish execution. It stops waiting
   * when all threads finish, the task scope is {@linkplain #shutdown() shut down}, the
   * deadline is reached, or the current thread is {@linkplain Thread#interrupt()
   * interrupted}.
   *
   * <p> This method may only be invoked by the task scope owner.
   *
   * @param timeout the timeout
   * @return this task scope
   * @throws IllegalStateException if this task scope is closed
   * @throws WrongThreadException if the current thread is not the task scope owner
   * @throws InterruptedException if interrupted while waiting
   * @throws TimeoutException if the deadline is reached while waiting
   */
  public StageScope<T> joinWithTimeout(Measure<Time> timeout)
      throws InterruptedException, TimeoutException {
    super.joinUntil(Instant.now().plusNanos((long) (timeout.in(Seconds) * 1e9)));
    return this;
  }

  /**
   * Gets the results of the computations performed by the completed tasks.
   * @return the results of the completed tasks
   */
  public Set<T> getResults() {
    ensureOwnerAndJoined();
    return Set.copyOf(results);
  }

  /**
   * If a scheduled task failed with an exception, that exception is rethrown here.
   *
   * @throws ExecutionException the
   */
  public void throwIfError() throws ExecutionException {
    ensureOwnerAndJoined();
    Throwable throwable = exception.get();
    if (throwable != null) {
      throw new ExecutionException(throwable);
    }
  }

  private record CommandTask<T>(AsyncScheduler scheduler, AsyncCommand command)
      implements Callable<T> {
    @Override
    public T call() throws Exception {
      scheduler.schedule(command);
      scheduler.await(command); // may throw if the command is cancelled
      return null;
    }
  }
}
