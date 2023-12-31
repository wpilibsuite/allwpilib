package edu.wpi.first.wpilibj2.command.async;

import edu.wpi.first.util.ErrorMessages;
import java.util.Objects;
import java.util.concurrent.Callable;
import java.util.concurrent.CancellationException;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

/**
 * @param <R> the self type, to allow for type safety on configured tasks
 */
public class Resource<R extends Resource<R>> {
  private static final Thread.UncaughtExceptionHandler defaultUeh = (t, e) -> {
    throw new RuntimeException(
        "An unhandled error was encountered while processing a task on thread " + t.getName(), e);
  };

  private final String name;
  private Task<R, ?> defaultTask;
  private final ExecutorService executor;

  private final ReadWriteLock lock = new ReentrantReadWriteLock();
  private final AtomicReference<TaskData<R, ?>> currentData =
      new AtomicReference<>(new TaskData<>(null, null, null));

  /**
   * An idle task that simply parks the owning thread. This task never returns a value.
   */
  private final Task<R, ?> idleTask;

  @SuppressWarnings({"InfiniteLoopStatement", "BusyWait"})
  private static final Callable<?> SLEEP = () -> {
    while (true) {
      Thread.sleep(Long.MAX_VALUE);
    }
  };

  public Resource(String name) {
    this.name = Objects.requireNonNull(name);
    this.executor = Executors.newSingleThreadExecutor(
        Thread.ofVirtual().name(name, 0).uncaughtExceptionHandler(defaultUeh).factory());

    var idleTask = new Task<>("[IDLE]", (R) this, Task.LOWEST_PRIORITY, (Callable<?>) SLEEP);
    this.idleTask = idleTask;
    this.defaultTask = idleTask;

    // Kick off the default task on construction so something is always running
    runTask(defaultTask);
  }

  public String getName() {
    return name;
  }

  /**
   * Sets the default task to run on the resource when no other task is scheduled. The default
   * task's priority is effectively the minimum allowable priority for any task requiring a
   * resource. For this reason, it's recommended that a default task have a priority no greater than
   * {@link Task#DEFAULT_PRIORITY} to prevent it from blocking other, non-default tasks from running.
   *
   * <p>The default task is initially an idle task that merely parks the execution thread. This task
   * has the lowest possible priority so as to allow any other task to run.
   *
   * @param defaultTask the new default task
   */
  public void setDefaultTask(Task<R, ?> defaultTask) {
    ErrorMessages.requireNonNullParam(defaultTask, "defaultTask", "setDefaultTask");

    var runningTask = getCurrentTask();
    var oldDefault = this.defaultTask;
    this.defaultTask = defaultTask;

    // Cancel the previous default task, if it's currently running, and replace it with the new
    // default task
    if (runningTask == oldDefault || runningTask == idleTask) {
      runTask(defaultTask);
    }
  }

  public Task<R, ?> getDefaultTask() {
    return defaultTask;
  }

  /**
   * Schedules a task to run, cancelling the current task if it has a lower
   * {@link Task#priority() priority} than the new task. If the task is scheduled,
   * {@link #getCurrentTask()} is guaranteed to return either the given task or any task scheduled
   * after it (depending on timing).
   *
   * @param task the task to run
   * @param <T>  the type of values returned by the task upon completion
   * @return a {@code Future} that can be used to retrieve the result of the task
   */
  public <T> Future<T> runTask(Task<R, T> task) {
    return runTask(task, true);
  }

  public <T> T runAndAwait(Task<R, T> task) throws ExecutionException {
    try {
      return (T) runTask(task).get();
    } catch (InterruptedException e) {
      return null;
    }
  }

  /**
   * Queues up a task to run using this resource.
   *
   * @param task          the task to run
   * @param cancelCurrent whether to cancel the current task. However, if the current task has a
   *                      higher priority than the given one, then
   * @param <T>           the type of values returned by the task upon completion
   * @return a {@code Future} that can be used to retrieve the result of the task, or {@code null}
   * if the task could not be queued
   */
  private <T> Future<T> runTask(Task<R, T> task, boolean cancelCurrent) {
    ErrorMessages.requireNonNullParam(task, "task", "runTask");

    System.out.println(
        "Requesting to run "
            + task
            + " on " + this
            + (cancelCurrent
            ? " (attempting to cancel current task)"
            : " (not attempting to cancel current task)"));

    if (task.resource() != this) {
      throw new IllegalArgumentException(
          "Cannot run a task on " + this.getName() + " that requires " + task.resource().getName());
    }

    lock.writeLock().lock();
    try {
      var data = currentData.get();
      var runningTask = data.task;
      var runningFuture = data.future;

      if (cancelCurrent && runningTask != null) {
        if (runningTask.priority() > task.priority()) {
          // Currently running a higher priority task, leave it running
          System.out.println("  Cannot cancel higher priority task " + runningTask);
          return null;
        }
      }

      if (cancelCurrent && runningFuture != null) {
        // Fire an interruption on the thread executing the currently running task
        System.out.println("  Cancelling running task " + runningTask);
        runningFuture.cancel(true);
        if (runningFuture.state() == Future.State.RUNNING) {
          try {
            // Wait a few milliseconds for the task body to hit the interruption. This may take a
            // little while
            System.out.println("  Waiting for running task to hit the interrupt flag... " + runningTask);
            runningFuture.get(5, TimeUnit.MILLISECONDS);
          } catch (InterruptedException | CancellationException e) {
            // Fine - the task was being canceled. We can safely ignore this
            System.out.println("  Gotcha");
          } catch (ExecutionException e) {
            // The task execution encountered an internal exception, re-raise it here
            throw new RuntimeException("An unhandled exception occurred while processing " + runningTask, e);
          } catch (TimeoutException e) {
            // The task didn't respond in time to the cancellation request
            // This is an error in user code and is unlikely to be able to be recovered from
            throw new IllegalStateException(
                "The current task did not respond to the terminal signal in time! " + runningTask, e);
          }
        } else {
          switch (runningFuture.state()) {
            case SUCCESS -> System.out.println("  Task completed with result: " + runningFuture.resultNow());
            case CANCELLED -> System.out.println("  Task was cancelled before it could complete: " + runningTask);
            case FAILED -> System.out.println("  Task failed with error: " + runningFuture.exceptionNow());
            case RUNNING -> System.out.println("  Task is still running??? " + task);
          }
        }
      }

      System.out.println("  Queueing up " + task);
      var completable = new CompletableFuture<T>();
      var future = executor.submit(() -> {
        try {
          var result = task.call();
          completable.complete(result);
          return result;
        } catch (Exception e) {
          completable.completeExceptionally(e);
          throw e;
        }
      });

      currentData.set(new TaskData<>(task, future, completable.whenComplete((result, error) -> {
        // TODO: Figure out why queueing a task here causes so many problems
//        Util.writing(lock, () -> {
//          System.out.println("Queueing default task after completing task: " + task);
//          // clear the current task data
//          currentData.set(new TaskData<>(null, null, null));
//          runTask(getDefaultTask());
//        });
      })));
      return future;
    } finally {
      lock.writeLock().unlock();
    }
  }

  /**
   * Waits until the currently running task has run to completion. This will block the calling
   * thread, preventing it from running anything else. This may cause issues if the task has a
   * long-running process or an infinite loop inside, as the only way to unblock would be for
   * a third, separate thread to either wake this one or to cancel the task.
   */
  public Object await() throws ExecutionException {
    var data = Util.reading(lock, currentData::get);
    if (data != null) {
      try {
        return data.completable.get();
      } catch (InterruptedException e) {
        // OK - the caller thread was woken up while we were waiting for a result
        // Reset the interruption flag before continuing
        Thread.currentThread().interrupt();
      } catch (CancellationException e) {
        // Fine - the task was cancelled while we were waiting, there's no result
      }
    }

    // No future (unlikely) or we were interrupted while waiting
    return null;
  }

  /**
   * Cancels the currently running task. The default task will be run in its place.
   */
  public void cancelCurrentTask() {
    runTask(getDefaultTask());
  }

  /**
   * Gets the currently executing task on this resource.
   */
  public Task<R, ?> getCurrentTask() {
    return Util.reading(lock, currentData::get).task;
  }

  /**
   * Gets the idle task. If a null task is scheduled with {@link #runTask(Task)}, the idle task
   * will be run instead.
   */
  @SuppressWarnings("unchecked")
  public <T> Task<R, T> getIdleTask() {
    return (Task<R, T>) idleTask;
  }

  @Override
  public String toString() {
    return name;
  }

  private record TaskData<R extends Resource<R>, T>(Task<R, T> task, Future<T> future, CompletableFuture<T> completable) {}
}
