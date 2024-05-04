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
 */
public class StageScope<T> extends StructuredTaskScope<T> {
  private final AsyncScheduler scheduler;
  private final Set<? extends Callable<? extends T>> remainingRequirements;
  private final Set<T> results = Collections.synchronizedSet(new HashSet<>());
  private final AtomicReference<Throwable> exception = new AtomicReference<>(null);

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

  public <U extends T> Subtask<U> fork(AsyncCommand command) {
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

  public StageScope<T> joinWithTimeout(Measure<Time> timeout)
      throws InterruptedException, TimeoutException {
    super.joinUntil(Instant.now().plusNanos((long) (timeout.in(Seconds) * 1e9)));
    return this;
  }

  public Set<T> getResults() {
    ensureOwnerAndJoined();
    return Set.copyOf(results);
  }

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
