package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.units.Units.Microseconds;
import static edu.wpi.first.units.Units.Milliseconds;
import static edu.wpi.first.units.Units.Nanoseconds;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import edu.wpi.first.wpilibj.event.EventLoop;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Callable;
import java.util.concurrent.CancellationException;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

public class AsyncScheduler {
  /**
   * The default update period of async commands is 20 milliseconds by convention.
   */
  public static final Measure<Time> DEFAULT_UPDATE_PERIOD = Milliseconds.of(20);

  private final Map<AsyncCommand, Throwable> errors = new HashMap<>();

  /**
   * The top-level running commands. This would include the highest-level command groups but not any
   * of their nested commands.
   */
  private final Map<HardwareResource, AsyncCommand> runningCommands = new HashMap<>();
  private final Map<HardwareResource, AsyncCommand> defaultCommands = new HashMap<>();
  private final Map<AsyncCommand, CommandState> commandStates = new HashMap<>();
  private final EventLoop eventLoop = new EventLoop();

  /**
   * How long to wait for a command to be canceled before timing out and throwing an error. Defaults
   * to 2.5ms.
   */
  public volatile Measure<Time> cancelTimeout;
  /**
   * How long to wait for a command to start up after being scheduled before timing out and throwing
   * an error. Defaults to 2.5ms.
   */
  public volatile Measure<Time> scheduleTimeout;

  // NOTE: This requires the jdk.virtualThreadScheduler.parallelism system property to be set to 1!
  //       If it is not set, or is set to a value > 1, then commands may be scheduled - at random -
  //       to run on different threads, with all the race condition and resource contention problems
  //       that entails. BE SURE TO SET `-Djdk.virtualThreadScheduler.parallelism=1` WHEN RUNNING A
  //       ROBOT PROGRAM.
  private final ThreadFactory factory =
      Thread.ofVirtual().name("async-commands-runner-", 1).factory();
  // Use a cached thread pool so we can guarantee one virtual thread per command
  // When commands complete, their virtual threads will be reused by later commands
  private final ExecutorService service = Executors.newCachedThreadPool(factory);

  private static final AsyncScheduler defaultScheduler = new AsyncScheduler();

  private final ReadWriteLock lock = new ReentrantReadWriteLock();

  public static AsyncScheduler getInstance() {
    return defaultScheduler;
  }

  public AsyncScheduler() {
    this(Milliseconds.of(20));
  }

  public AsyncScheduler(Measure<Time> triggerPollingPeriod) {
    this(triggerPollingPeriod, Milliseconds.of(2.5), Milliseconds.of(2.5));
  }

  public AsyncScheduler(Measure<Time> triggerPollingPeriod, Measure<Time> cancelTimeout, Measure<Time> scheduleTimeout) {
    // Start polling the event loop
    // This intentionally runs on the same carrier thread as commands so the boolean checks
    // will be guaranteed to run on the same thread as the commands that manipulate state
    service.submit(() -> {
      Thread.currentThread().setName("async-scheduler-eventloop");
      pollEventLoop(triggerPollingPeriod);
    });

    this.cancelTimeout = cancelTimeout.copy();
    this.scheduleTimeout = scheduleTimeout.copy();
  }

  @SuppressWarnings({"InfiniteLoopStatement", "BusyWait"})
  private void pollEventLoop(Measure<Time> period) {
    var ms = (long) period.in(Milliseconds);
    while (true) {
      try {
        Thread.sleep(ms);
        eventLoop.poll();
      } catch (Exception e) {
        // ignore and keep running
      }
    }
  }

  public void registerResource(HardwareResource resource) {
    registerResource(resource, new IdleCommand(resource));
  }

  public void registerResource(HardwareResource resource, AsyncCommand defaultCommand) {
    defaultCommands.put(resource, defaultCommand);
    schedule(defaultCommand);
  }

  public void setDefaultCommand(HardwareResource resource, AsyncCommand defaultCommand) {
    if (!defaultCommand.requires(resource)) {
      throw new IllegalArgumentException("A resource's default command must require that resource");
    }

    if (defaultCommand.requirements().size() > 1) {
      throw new IllegalArgumentException(
          "A resource's default command cannot require other resources");
    }

    var oldDefaultCommand = defaultCommands.put(resource, defaultCommand);
    cancelAndWait(oldDefaultCommand, false);
    schedule(defaultCommand);
  }

  public AsyncCommand getDefaultCommand(HardwareResource resource) {
    return defaultCommands.get(resource);
  }

  /**
   * Schedules a command to run. The command will execute in a virtual thread; its status may be
   * checked later with {@link #isRunning(AsyncCommand)}. Scheduling will fail if the command has a
   * lower priority than a currently running command using one or more of the same resources.
   *
   * <p>This method will block until the command has started to run. This may take between 0.1 and
   * 2.0 milliseconds.
   *
   * @param command the command to schedule
   */
  public void schedule(AsyncCommand command) {
    var currentState = Util.reading(lock, () -> commandStates.get(command));
    if (currentState != null) {
      return;
    }

    // Fire cancel requests on all conflicting commands (in a lock), then wait for them all to
    // exit (outside the lock, to avoid contention with locking behavior in the cancellation cleanup)
    // Then (in a lock again), create and schedule the carrier for the command, and (outside the
    // lock), wait for the carrier to start up before finally returning from schedule().

    var canceledCommandStates = new ArrayList<CommandState>();
    boolean isSchedulable = Util.writing(
        lock,
        () -> {
          // Don't schedule if a required resource is currently running a higher priority command
          boolean shouldSchedule = true;
          for (HardwareResource resource : command.requirements()) {
            var runningCommand = runningCommands.get(resource);
            if (command.isLowerPriorityThan(runningCommand)) {
              shouldSchedule = false;
              break;
            }
          }
          if (!shouldSchedule) {
            return false;
          }

          // Cancel the currently scheduled commands for each required resource and schedule the
          // incoming command over them
          for (HardwareResource resource : command.requirements()) {
            var runningCommand = runningCommands.get(resource);

            // Add to the running commands before we've technically scheduled it
            // Cancelling a command will schedule default commands (which we don't want!) if the
            // cancelled command is still mapped to a resource
            runningCommands.put(resource, command);

            if (runningCommand != null) {
              // Cancel the current command, but don't start the default command for the resource
              // because we're about to start our own command that requires it
              // Fire the event but don't wait for completion, because that will deadlock.
              // Instead, after exiting this block, we wait for all the canceled commands to exit
              // OUTSIDE the lock
              canceledCommandStates.add(fireCancelEvent(runningCommand));
            }
          }

          return true;
        });

    if (!isSchedulable) {
      // Not schedulable, bail
      return;
    }

    awaitBulkCancellation(canceledCommandStates);

    var newCommandStates = Util.writing(
        lock,
        () -> {
          // Sanity check to make sure we're not overwriting state for the scheduled command, if it
          // was already running when rescheduled. It should have been cancelled above.
          var existingStates = commandStates.get(command);
          if (existingStates != null) {
            throw new IllegalStateException("Command state was not cleared!");
          }

          // schedule the command to run on a virtual thread, automatically scheduling the default
          // commands for all its required resources when it completes
          var scheduled = new CompletableFuture<Boolean>();
          var completion = new CompletableFuture<>();
          var states = new CommandState(command, scheduled, completion);
          Callable<?> callback = createCommandExecutorCallback(command, states);
          states.exec = service.submit(callback);
          commandStates.put(command, states);
          return states;
        });

    if (newCommandStates != null) {
      // Wait for the command to have started before returning
      // Note: this may take up to 1 or 2 milliseconds! Normally seen on the order of ~0.1ms
      try {
        newCommandStates.schedule.get((long) scheduleTimeout.in(Nanoseconds), TimeUnit.NANOSECONDS);
      } catch (TimeoutException e) {
        throw new RuntimeException("Command " + command.name() + " (run ID=" + newCommandStates.id  + ") did not start within " + scheduleTimeout.toLongString() + " of being scheduled!", e);
      } catch (InterruptedException e) {
        throw new RuntimeException("Interrupted during the " + scheduleTimeout.toLongString() + " wait for " + command.name() + " to start", e);
      } catch (ExecutionException e) {
        throw new RuntimeException("Unable to wait for command to start", e);
      }
    }
  }

  private void awaitBulkCancellation(Collection<CommandState> canceledCommandStates) {
    try {
      CompletableFuture
          .allOf(canceledCommandStates.stream().map(s -> s.completion).toArray(CompletableFuture[]::new))
          .get((long) cancelTimeout.in(Nanoseconds), TimeUnit.NANOSECONDS);
    } catch (InterruptedException e) {
      throw new IllegalStateException("Interrupted while waiting for all commands to cancel", e);
    } catch (ExecutionException e) {
      throw new IllegalStateException("One or more canceled commands encountered an internal exception", e.getCause());
    } catch (TimeoutException e) {
      var stillRunning = canceledCommandStates.stream().filter(s -> !s.completion.isDone()).map(s -> s.command.name()).toList();
      throw new IllegalStateException("Timed out while waiting for all canceled commands to complete. Still running: " + stillRunning, e);
    }
  }

  /**
   * Checks for errors that have been thrown by the asynchronous commands. This will clear the
   * cached errors, and any errors were present, those errors will immediately be rethrown
   */
  public void checkForErrors() {
    var errors =
        Util.writing(
            lock,
            () -> {
              var copy = Map.copyOf(this.errors);
              this.errors.clear();
              return copy;
            });
    if (errors.isEmpty()) {
      // No errors, all good
      return;
    }
    if (errors.size() == 1) {
      // Just a single error, wrap and rethrow that on its own
      var e = errors.entrySet().iterator().next();
      throw new CommandExecutionException(e.getKey(), e.getValue());
    }

    // Multiple errors were thrown since the last check, wrap them all in a MultiException and throw
    // that
    var multi = new MultiException();
    errors.forEach((command, error) -> multi.add(new CommandExecutionException(command, error)));
    throw multi;
  }

  private static final Object COMPLETED_VALUE = new Object();

  /**
   * Creates a wrapper callback that manages execution of an async command and the associated
   * cleanup after it completes or is cancelled.
   *
   * @param command the command to wrap
   * @return a callback that can be scheduled on an executor service
   */
  private Callable<?> createCommandExecutorCallback(AsyncCommand command, CommandState state) {
    return () -> {
      long runId = state.id;
      commandOnCurrentThread.set(command);

      boolean wasInterrupted = false;
      Object result = null;
      Throwable exception = null;
      try {
        // Put this inside the try-catch to handle the case where cancellation occurs directly after
        // scheduling
        state.schedule.complete(true);

        command.run();
        result = COMPLETED_VALUE;
        return null;
      } catch (Throwable e) {
        if (e instanceof InterruptedException) {
          // Cancelled during execution. This is expected and totally fine.
          wasInterrupted = true;
          result = COMPLETED_VALUE;
        } else {
          // Command execution raised an error.
          // Cache it in the errors map for `checkForErrors` to pick up.
          Util.writing(lock, () -> errors.put(command, e));
          exception = e;
        }
        // Rethrow the exception. This will make the future complete with an error state.
        // If the caller uses `await` on the command or `get` on its associated future,
        // then this exception will be raised at the call site. Otherwise, it will have
        // to be picked up by calling `checkForErrors` (presumably in a periodic loop)
        throw e;
      } finally {
        // Not using Util.writing() because `wasInterrupted` can't be captured
        lock.writeLock().lock();
        try {
          if (commandStates.get(command) instanceof CommandState s && s.id == runId) {
            // Clean up if still running. Schedule the default command if we weren't interrupted
            // Interruption means a new command was explicitly scheduled, so the default command
            // would not be desired.
            cleanupAfterCompletion(command, !wasInterrupted);
          }
        } finally {
          if (exception != null) {
            state.completion.completeExceptionally(exception);
          } else if (result != null) {
            state.completion.complete(result);
          } else {
            // note: result shouldn't be null, but just in case...
            state.completion.complete(COMPLETED_VALUE);
          }
          lock.writeLock().unlock();
        }
      }
    };
  }

  /**
   * Waits for a command to complete. This will block until the command has completed or been
   * interrupted or cancelled by another command. Does nothing if the given command is not
   * scheduled.
   *
   * @param command the command to wait for.
   * @throws ExecutionException if the command encountered an error while it was executing
   */
  public void await(AsyncCommand command) throws ExecutionException {
    var states = Util.reading(lock, () -> commandStates.get(command));

    if (states == null) {
      // not currently running, nothing to await
      return;
    }

    try {
      states.completion.get();
    } catch (InterruptedException e) {
      // Interrupted while waiting, reset the interrupt flag but do not raise
      Thread.currentThread().interrupt();
    } catch (ExecutionException e) {
      // Swallow execution exceptions caused by interrupts
      var interuptionCause = e.getCause();
      while (interuptionCause != null && !(interuptionCause instanceof InterruptedException)) {
        interuptionCause = interuptionCause.getCause();
      }
      if (interuptionCause == null) {
        throw e;
      }
    } catch (CancellationException e) {
      // Command was cancelled, nothing wrong here
    }
  }

  private CommandState fireCancelEvent(AsyncCommand command) {
    if (command == null) {
      // Nothing to do
      return null;
    }

    return Util.writing(
        lock,
        () -> {
          var state = commandStates.get(command);
          if (state == null) {
            // Not running, nothing to cancel
            return null;
          }

          state.exec.cancel(true);
          return state;
        });
  }

  public void cancel(AsyncCommand command) {
    cancelAndWait(command, true);
  }

  /**
   * Cancels a running command and schedules the default commands for all of its required resources.
   * Has no effect if the given command is not currently running.
   *
   * <p>This method will wait until the command has fully exited before returning. A timeout for the
   * wait can be configured by setting {@link #cancelTimeout}.
   *
   * @param command the command to cancel
   */
  public void cancelAndWait(AsyncCommand command, boolean thenRunDefault) {
    if (command == null) {
      // Nothing to do
      return;
    }

    var states = fireCancelEvent(command);
    if (states == null) {
      return;
    }

    long runId = states.id;

    try {
      // Wait for the command to exit. A cancellation request will interrupt the vthread and
      // immediately return, but the command could continue running until it reaches a point where it
      // calls an interruptible function like Thread.sleep.


      // TODO: This seems to break. The caller thread sometimes waits the full duration without
      //       yielding control to the execution thread, so the wait times out and throws an
      //       exception, only /after/ which the execution thread is picked up and is able to process
      //       the cancellation request
      states.completion.get((long) cancelTimeout.in(Nanoseconds), TimeUnit.NANOSECONDS);

      // Clean up only after the command has completed
      cleanupAfterCompletion(command, thenRunDefault);
    } catch (TimeoutException e) {
      String message = "Command " + command.name() + " (run ID=" + runId + ") did not stop within " + cancelTimeout.in(Microseconds) + " µs of the cancellation request!";
      throw new IllegalStateException(message);
    } catch (InterruptedException e) {
      // Why would the carrier thread be interrupted?
      throw new RuntimeException("Carrier thread interrupted while waiting for command cancellation", e);
    } catch (ExecutionException e) {
      // I think we'd want to rethrow the original exception instead of rewrapping it...
      throw new RuntimeException(e);
    }
  }

  /**
   * Cleans up state after a command completes or is cancelled. This method is not inherently
   * thread-safe and must be wrapped in calls to the write lock.
   *
   * @param command the command to clean up after
   */
  private void cleanupAfterCompletion(AsyncCommand command, boolean thenRunDefault) {
    // TODO: This may be buggy at the boundary where a command completes and is immediately rescheduled
    var states = commandStates.get(command);
    if (states == null) {
      return;
    }

    commandStates.remove(command);

    // Clear current thread owner
    if (commandOnCurrentThread.get() == command) {
      commandOnCurrentThread.remove();
    }

    for (var resource : command.requirements()) {
      // If the currently running command for this resource is the one we just
      // cancelled, we can schedule its default command
      if (runningCommands.remove(resource, command) && thenRunDefault) {
        scheduleDefault(resource);
      }
    }
  }

  /**
   * Checks if a command is currently running.
   *
   * @param command the command to check
   * @return true if the command is running, false if not
   */
  public boolean isRunning(AsyncCommand command) {
    return Util.reading(lock, () -> commandStates.containsKey(command));
  }

  /**
   * @return
   */
  public Map<HardwareResource, AsyncCommand> getRunningCommands() {
    return Util.reading(lock, () -> Map.copyOf(runningCommands));
  }

  /**
   * Cancels all currently running commands, then starts the default commands for any resources
   * that had canceled commands. A default command that is currently running will not be canceled.
   */
  public void cancelAll() {
    var canceledStates = Util.writing(
        lock,
        () -> {
          var states = new ArrayList<CommandState>();
          for (AsyncCommand command : commandStates.keySet()) {
            if (defaultCommands.containsValue(command)) {
              // Don't bother cancelling a default command.
              // It'd be reinitialized immediately afterward
              continue;
            }
            states.add(fireCancelEvent(command));
          }
          return states;
        });
    awaitBulkCancellation(canceledStates);
    Util.writing(lock, () -> {
      defaultCommands.forEach((_res, cmd) -> schedule(cmd));
    });
  }

  /**
   * Schedules the default command for the given resource. If no default command exists, then a new
   * idle command will be created and assigned as the default.
   *
   * @param resource the resource to schedule the default command for
   */
  public void scheduleDefault(HardwareResource resource) {
    var defaultCommand = defaultCommands.computeIfAbsent(resource, IdleCommand::new);
    schedule(defaultCommand);
  }

  /**
   * Gets the currently running command that uses the given resource.
   *
   * @param resource the resource to query for
   * @return the running command using a particular resource, or null if no command is currently
   * using it
   */
  public AsyncCommand getCommandUsing(HardwareResource resource) {
    return Util.reading(lock, () -> runningCommands.get(resource));
  }

  private static final ThreadLocal<AsyncCommand> commandOnCurrentThread = new ThreadLocal<>();

  /**
   * Pauses the current command for 20 milliseconds. If a higher priority command on
   * any of the current command's required resources is scheduled while paused, the higher
   * priority command will interrupt the command while it is paused.
   *
   * @throws InterruptedException if the current command was canceled or interrupted by a higher
   *   priority command
   */
  public void pauseCurrentCommand() throws InterruptedException {
    pauseCurrentCommand(DEFAULT_UPDATE_PERIOD);
  }

  /**
   * Pauses the current command for the specified period of time.  If a higher priority command on
   * any of the current command's required resources is scheduled while paused, the higher
   * priority command will interrupt the command while it is paused.
   *
   * @throws InterruptedException if the current command was canceled or interrupted by a higher
   *   priority command
   */
  public void pauseCurrentCommand(Measure<Time> time) throws InterruptedException {
    var command = commandOnCurrentThread.get();
    if (command == null) {
      throw new IllegalStateException(
          "pauseCurrentCommand() may only be called by a running command!");
    }

    var states = Util.reading(lock, () -> commandStates.get(command));

    if (Thread.currentThread().isInterrupted()) {
      // Thread interrupt flag was set prior to entering pause
      // This can happen if command code uses Timer.delay or something similar where an interrupted
      // exception is caught and the interrupt flag is set
      throw new InterruptedException();
    }

    long ms = (long) time.in(Milliseconds);
    int ns = (int) (time.in(Nanoseconds) % 1e6);

    Thread.sleep(ms, ns);

    if (Thread.currentThread().isInterrupted()) {
      // Interrupted while paused, but without an interrupted exception being thrown?
      throw new InterruptedException();
    }

    // TODO: How can a command be removed from the futures list without being interrupted?
    var statesAfterPause = Util.reading(lock, () -> commandStates.get(command));
    if (statesAfterPause != states) {
      String message = "Command " + command.name() + " (run ID=" + states.id + ")" + " was removed from the scheduler while paused without being interrupted!";
      if (statesAfterPause != null) {
        message += " (current run ID=" + statesAfterPause.id + " did not match!)";
      }
      throw new IllegalStateException(message);
    }
  }

  private static final AtomicLong invocationId = new AtomicLong(0);

  static final class CommandState {
    /**
     * A unique ID that identifies the command state. Every run of a command gets a new state,
     * even for the same command object. The ID helps to distinguish the separate runs.
     */
    final long id = invocationId.incrementAndGet();

    final AsyncCommand command;

    // assigned after construction
    Future<?> exec;
    final CompletableFuture<Boolean> schedule;
    final CompletableFuture<Object> completion;

    CommandState(AsyncCommand command, CompletableFuture<Boolean> schedule, CompletableFuture<Object> completion) {
      this.command = command;
      this.schedule = schedule;
      this.completion = completion;
    }
  }

  public EventLoop getDefaultButtonLoop() {
    return eventLoop;
  }
}
