package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.units.Units.Microseconds;
import static edu.wpi.first.units.Units.Milliseconds;
import static edu.wpi.first.units.Units.Nanoseconds;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import edu.wpi.first.wpilibj.event.EventLoop;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.OptionalInt;
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
  private final Map<AsyncCommand, CommandState> futures = new HashMap<>();
  private final EventLoop eventLoop = new EventLoop();

  /**
   * How long to wait for a command to be canceled before timing out and throwing an error. Defaults
   * to 2.5ms.
   */
  public volatile Measure<Time> cancelTimeout = Microseconds.of(2500);
  /**
   * How long to wait for a command to start up after being scheduled before timing out and throwing
   * an error. Defaults to 2ms.
   */
  public volatile Measure<Time> scheduleTimeout = Microseconds.of(2000);

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
    // Start polling the event loop
    // This intentionally runs on the same carrier thread as commands so the boolean checks
    // will be guaranteed to run on the same thread as the commands that manipulate state
    service.submit(() -> {
      Thread.currentThread().setName("async-scheduler-eventloop");
      pollEventLoop(triggerPollingPeriod);
    });
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
    Logger.log("SCHEDULE", "Scheduling " + command.name());
    final long start = System.nanoTime();

    var currentState = Util.reading(lock, () -> futures.get(command));
    if (currentState != null) {
      Logger.log("SCHEDULE", "Command " + command.name() + " is already running (run ID=" + currentState.id + "), not rescheduling");
      return;
    }

    var canceledCommandStates = new ArrayList<CommandState>();

    var newCommandStates = Util.writing(
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
            Logger.log("SCHEDULE", "Lower priority than running commands, not scheduling");
            return null;
          }

          // Cancel the currently scheduled commands for each required resource and schedule the
          // incoming command over them
          for (HardwareResource resource : command.requirements()) {
            var runningCommand = runningCommands.get(resource);

            // Add to the running commands before we've technically scheduled it
            // Cancelling a command will schedule default commands (which we don't want!) if the
            // cancelled command is still mapped to a resource
            runningCommands.put(resource, command);

            if (runningCommand != null
                    && runningCommand.interruptBehavior() == AsyncCommand.InterruptBehavior.Cancel) {
              // Cancel the current command, but don't start the default command for the resource
              // because we're about to start our own command that requires it
              // Fire the event but don't wait for completion, because that will deadlock.
              // Instead, after exiting this block, we wait for all the canceled commands to exit
              // OUTSIDE the lock
              canceledCommandStates.add(fireCancelEvent(runningCommand));
            }
          }

          // Sanity check to make sure we're not overwriting state for the scheduled command, if it
          // was already running when rescheduled. It should have been cancelled above.
          var existingStates = futures.get(command);
          if (existingStates != null) {
            throw new IllegalStateException("Command state was not cleared!");
          }

          // schedule the command to run on a virtual thread, automatically scheduling the default
          // commands for all its required resources when it completes
          var scheduled = new CompletableFuture<Boolean>();
          var completion = new CompletableFuture<Object>();
          var states = new CommandState(scheduled, completion);
          Callable<?> callback = createCommandExecutorCallback(command, states);
          states.exec = service.submit(callback);
          futures.put(command, states);
          return states;
        });

    try {
      CompletableFuture
          .allOf(canceledCommandStates.stream().map(s -> s.completion).toArray(CompletableFuture[]::new))
          .get((long) cancelTimeout.in(Nanoseconds), TimeUnit.NANOSECONDS);
    } catch (InterruptedException e) {
      throw new IllegalStateException("Interrupted while waiting for all commands to cancel", e);
    } catch (ExecutionException e) {
      throw new IllegalStateException("One or more canceled commands encountered an internal exception", e.getCause());
    } catch (TimeoutException e) {
      throw new IllegalStateException("Timed out while waiting for all canceled commands to complete", e);
    }

    if (newCommandStates != null) {
      // Wait for the command to have started before returning
      // Note: this may take up to 1 or 2 milliseconds! Normally seen on the order of ~0.1ms
      try {
        newCommandStates.schedule.get((long) scheduleTimeout.in(Nanoseconds), TimeUnit.NANOSECONDS);
      } catch (TimeoutException e) {
        throw new RuntimeException("Command did not start within " + scheduleTimeout.toLongString() + " of being scheduled!", e);
      } catch (InterruptedException e) {
        throw new RuntimeException("Interrupted during the " + scheduleTimeout.toLongString() + " wait for " + command.name() + " to start", e);
      } catch (ExecutionException e) {
        throw new RuntimeException("Unable to wait for command to start", e);
      } finally {
        long end = System.nanoTime();
        Logger.log("SCHEDULE", "Took " + (end - start) / 1e3 + " µs to schedule " + command.name() + " (assigned run ID=" + newCommandStates.id + ")");
      }
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
      Logger.log("COMMAND", "Starting up vthread for command " + command.name() + " (run ID=" + runId + ")");
      Logger.log("COMMAND", "Setting thread local command owner to " + command.name() + " (run ID=" + runId + ")");
      commandOnCurrentThread.set(command);

      boolean wasInterrupted = false;
      Object result = null;
      Throwable exception = null;
      try {
        // Put this inside the try-catch to handle the case where cancellation occurs directly after
        // scheduling
        Logger.log("COMMAND", "Completing the schedule future for " + command.name() + " (run ID=" + runId + ")");
        state.schedule.complete(true);

        Logger.log("COMMAND", "Starting execution for " + command.name() + " (run ID=" + runId + ")");
        long start = System.nanoTime();
        command.run();
        long end = System.nanoTime();
        Logger.log("COMMAND", command + " completed naturally in " + (end - start) / 1e3 + " µs" + " (run ID=" + runId + ")");
        result = COMPLETED_VALUE;
        return null;
      } catch (Throwable e) {
        if (e instanceof InterruptedException) {
          // Cancelled during execution. This is expected and totally fine.
          wasInterrupted = true;
          Logger.log("COMMAND", command.name()  + " (run ID=" + runId + ")" + " was interrupted with Future.cancel() while parked");
          result = COMPLETED_VALUE;
        } else {
          // Command execution raised an error.
          // Cache it in the errors map for `checkForErrors` to pick up.
          Logger.log("COMMAND ERROR", e.getMessage());
          Util.writing(lock, () -> errors.put(command, e));
          exception = e;
        }
        // Rethrow the exception. This will make the future complete with an error state.
        // If the caller uses `await` on the command or `get` on its associated future,
        // then this exception will be raised at the call site. Otherwise, it will have
        // to be picked up by calling `checkForErrors` (presumably in a periodic loop)
        throw e;
      } finally {
        Logger.log("COMMAND", "Execution completed for " + command.name() + " (run ID=" + runId + ")" + ", starting cleanup");
        Logger.log("COMMAND", "Acquiring lock..." + " (run ID=" + runId + ")");
        // Not using Util.writing() because `wasInterrupted` can't be captured
        lock.writeLock().lock();
        Logger.log("COMMAND", "Lock acquired" + " (run ID=" + runId + ")");
        try {
          if (futures.get(command) instanceof CommandState s && s.id == runId) {
            Logger.log("COMMAND", "Cleaning up after " + command.name() + " (run ID=" + runId + ")");
            // Clean up if still running. Schedule the default command if we weren't interrupted
            // Interruption means a new command was explicitly scheduled, so the default command
            // would not be desired.
            cleanupAfterCompletion(command, !wasInterrupted);
          } else {
            Logger.log("COMMAND", "Command " + command.name() + " (run ID=" + runId + ")" + " was already cleaned up");
          }
        } finally {
          Logger.log("COMMAND", "Releasing lock" + " (run ID=" + runId + ")");
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
    Logger.log("AWAIT", "Awaiting " + command.name());
    var states = Util.reading(lock, () -> futures.get(command));

    if (states == null) {
      // not currently running, nothing to await
      Logger.log("AWAIT", "Command " + command.name() + " is not currently running, nothing to await");
      return;
    }

    try {
      Logger.log("AWAIT", "Parking until " + command.name() + " (run ID=" + states.id + ") completes or is canceled...");
      long start = System.nanoTime();
      states.completion.get();
      long end = System.nanoTime();
      Logger.log("AWAIT", "Command " + command.name() + " (run ID=" + states.id + ") exited after waiting " + (end - start) / 1e3 + " µs");
    } catch (InterruptedException e) {
      // Interrupted while waiting, reset the interrupt flag but do not raise
      Logger.log("AWAIT", "Interrupted while awaiting " + command.name());
      Thread.currentThread().interrupt();
    } catch (ExecutionException e) {
      // Swallow execution exceptions caused by interrupts
      var interuptionCause = e.getCause();
      while (interuptionCause != null && !(interuptionCause instanceof InterruptedException)) {
        interuptionCause = interuptionCause.getCause();
      }
      if (interuptionCause == null) {
        Logger.log("AWAIT", "Command " + command.name() + " errored while awaiting its completion: " + e.getMessage());
        throw e;
      }
    } catch (CancellationException e) {
      // Command was cancelled, nothing wrong here
      Logger.log("AWAIT", "Command " + command.name() + " (run ID=" + states.id + ") was cancelled while awaiting its completion");
    }
  }

  private CommandState fireCancelEvent(AsyncCommand command) {
    if (command == null) {
      // Nothing to do
      return null;
    }

    Logger.log("CANCEL", "Cancelling command " + command.name());;
    return Util.writing(
        lock,
        () -> {
          var value = futures.get(command);
          if (value == null) {
            // Not running, nothing to cancel
            return null;
          }

          Logger.log("CANCEL", "Triggering interrupt for command " + command.name() + " (run ID=" + value.id + ")");
          value.exec.cancel(true);
          return value;
        });
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

      Logger.log("CANCEL", "Waiting for command " + command.name() + " to exit (run ID=" + runId + ")");

      // TODO: This seems to break. The caller thread sometimes waits the full duration without
      //       yielding control to the execution thread, so the wait times out and throws an
      //       exception, only /after/ which the execution thread is picked up and is able to process
      //       the cancellation request
      states.completion.get((long) cancelTimeout.in(Nanoseconds), TimeUnit.NANOSECONDS);
      Logger.log("CANCEL", "Command " + command.name() + " (run ID=" + runId + ") exited within " + cancelTimeout.in(Microseconds) + " µs of the cancellation request");

      // Clean up only after the command has completed
      cleanupAfterCompletion(command, thenRunDefault);
    } catch (TimeoutException e) {
      String message = "Command " + command.name() + " (run ID=" + runId + ") did not stop within " + cancelTimeout.in(Microseconds) + " µs of the cancellation request!";
      Logger.log("CANCEL", message);
      Logger.log("CANCEL", "Current lock owner: " + Util.currentLockOwner.get());
      try {
        Files.writeString(Path.of("foo.xsv"), Logger.formattedLogTable());
      } catch (Exception ignore) {}
      throw new IllegalStateException(message);
    } catch (InterruptedException e) {
      // Why would the carrier thread be interrupted?
      throw new RuntimeException("Carrier thread interrupted while waiting for command cancellation", e);
    } catch (ExecutionException e) {
      // I think we'd want to rethrow the original exception instead of rewrapping it...
      throw new RuntimeException(e);
    }
    Logger.log("CANCEL", "Cancelled command " + command.name() + " (run ID=" + runId + ")");
  }

  public void cancelAll(Collection<AsyncCommand> commands) {
    Util.writing(lock, () -> commands.forEach(cmd -> cancelAndWait(cmd, true)));
  }

  private Future<?> futureFor(AsyncCommand command) {
    return Util.reading(
        lock,
        () -> {
          if (futures.get(command) instanceof CommandState s)
            return s.exec;
          return null;
        });
  }

  /**
   * Cleans up state after a command completes or is cancelled. This method is not inherently
   * thread-safe and must be wrapped in calls to the write lock.
   *
   * @param command the command to clean up after
   */
  private void cleanupAfterCompletion(AsyncCommand command, boolean thenRunDefault) {
    // TODO: This may be buggy at the boundary where a command completes and is immediately rescheduled
    var states = futures.get(command);
    if (states == null) {
      Logger.log("CLEANUP", "Cannot clean up for a command that isn't running! Requested command " + command.name() + " has no associated states");
      return;
    }

    long runId = states.id;
    Logger.log("CLEANUP", "Removing future states for " + command.name() + " (run ID=" + runId + ")");
    futures.remove(command);

    // Clear current thread owner
    if (commandOnCurrentThread.get() == command) {
      Logger.log("CLEANUP", "Clearing threadlocal owner " + command.name() + " (run ID=" + runId + ")");
      commandOnCurrentThread.remove();
    }

    for (var resource : command.requirements()) {
      // If the currently running command for this resource is the one we just
      // cancelled, we can schedule its default command
      if (runningCommands.remove(resource, command) && thenRunDefault) {
        Logger.log("CLEANUP", "Scheduling default command for " + resource.getName() + " after run " + runId);
        scheduleDefault(resource);
      }
    }
    Logger.log("CLEANUP", "Completed cleanup for " + command.name() + " (run ID=" + runId + ")");
  }

  /**
   * Checks if a command is currently running. A command that's executed as part of a {@link
   * Sequence} or {@link ParallelGroup} is considered to be running, even if it's not the
   * highest-level owner of its resources.
   *
   * @param command the command to check
   * @return true if the command is running, false if not
   */
  public boolean isRunning(AsyncCommand command) {
    return Util.reading(lock, () -> futures.containsKey(command));
  }

  /**
   * @return
   */
  public Map<HardwareResource, AsyncCommand> getRunningCommands() {
    return Util.reading(lock, () -> Map.copyOf(runningCommands));
  }

  /**
   * Cancels all currently running commands.
   */
  public void cancelAll() {
    Logger.log("CANCELALL", "Cancelling all commands");
    Util.writing(
        lock,
        () -> {
          // Cancel every running command...
          for (AsyncCommand asyncCommand : List.copyOf(runningCommands.values())) {
            if (defaultCommands.containsValue(asyncCommand)) {
              // ... except for default commands (no point in cancelling and then immediately restarting)
              continue;
            }
            cancelAndWait(asyncCommand, false);
          }

          // ... and then schedule the default commands
          defaultCommands.forEach((res, cmd) -> schedule(cmd));
        });
    Logger.log("CANCELALL", "Canceled all and scheduled defaults");
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
   * any of the current command's required resources is scheduled while paused, and if the current
   * command is set to {@link AsyncCommand.InterruptBehavior#Suspend}, then the command will
   * continue to stay paused as long as any higher priority command is running. Otherwise, a higher
   * priority command will interrupt the command while it is paused.
   *
   * @throws InterruptedException if the current command was canceled, or, if the command's
   *                              {@link AsyncCommand#interruptBehavior() interrupt behavior} is not set to suspend,
   *                              interrupted by a higher priority command
   */
  public void pauseCurrentCommand() throws InterruptedException {
    pauseCurrentCommand(DEFAULT_UPDATE_PERIOD);
  }

  /**
   * Pauses the current command for the specified period of time. If a higher priority command on
   * any of the current command's required resources is scheduled while paused, and if the current
   * command is set to {@link AsyncCommand.InterruptBehavior#Suspend}, then the command will
   * continue to stay paused as long as any higher priority command is running. Otherwise, a higher
   * priority command will interrupt the command while it is paused.
   *
   * @throws InterruptedException if the current command was canceled, or, if the command's
   *                              {@link AsyncCommand#interruptBehavior() interrupt behavior} is not set to suspend,
   *                              interrupted by a higher priority command
   */
  @SuppressWarnings("BusyWait")
  public void pauseCurrentCommand(Measure<Time> time) throws InterruptedException {
    var command = commandOnCurrentThread.get();
    if (command == null) {
      throw new IllegalStateException(
          "pauseCurrentCommand() may only be called by a running command!");
    }

    Logger.log("PAUSE", "Reading state of command " + command.name());

    var states = Util.reading(lock, () -> futures.get(command));

    if (Thread.currentThread().isInterrupted()) {
      // Thread interrupt flag was set prior to entering pause
      // This can happen if command code uses Timer.delay or something similar where an interrupted
      // exception is caught and the interrupt flag is set
      Logger.log("PAUSE", "Thread interrupt flag was set for " + command.name() + " prior to entering pause");
      throw new InterruptedException();
    }

    long ms = (long) time.in(Milliseconds);
    int ns = (int) (time.in(Nanoseconds) % 1e6);

    long start = System.nanoTime();

    // Always sleep once
    // Then, if the command is configured to suspend while higher priority commands run,
    // continue to sleep in a loop. If we're ever interrupted, it means the command was canceled
    // outright, and we should respect the cancellation
    do {
      Logger.log("PAUSE", "Sleeping " + command.name() + " (run ID=" + states.id + ")" + " for " + time.in(Microseconds) + " µs...");
      Thread.sleep(ms, ns);
    } while (!Thread.currentThread().isInterrupted() && currentCommandShouldSuspend());
    long end = System.nanoTime();
    Logger.log("PAUSE", "Pause completed for " + command.name()  + " (run ID=" + states.id + ")" + " in " + (end - start) / 1e3 + " µs");

    if (Thread.currentThread().isInterrupted()) {
      // Interrupted while paused, but without an interrupted exception being thrown?
      Logger.log("PAUSE", "Thread interrupt flag was set during sleep for " + command.name() + " (run ID=" + states.id + ")" + "without an InterruptedException being thrown");
      throw new InterruptedException();
    }

    // TODO: How can a command be removed from the futures list without being interrupted?
    var statesAfterPause = Util.reading(lock, () -> futures.get(command));
    if (statesAfterPause != states) {
      String message = "Command " + command.name() + " (run ID=" + states.id + ")" + " was removed from the scheduler while paused without being interrupted!";
      if (statesAfterPause != null) {
        message += " (current run ID=" + statesAfterPause.id + " did not match!)";
      }
      Logger.log("PAUSE", message);
      throw new InterruptedException(message);
    }
  }

  private boolean currentCommandShouldSuspend() {
    var command = commandOnCurrentThread.get();
    if (command.interruptBehavior() != AsyncCommand.InterruptBehavior.Suspend) {
      return false;
    }

    var prior = getHighestPriorityCommandUsingAnyOf(command.requirements());
    return prior.isPresent() && prior.getAsInt() >= command.priority();
  }

  public OptionalInt getHighestPriorityCommandUsingAnyOf(Collection<HardwareResource> resources) {
    return Util.reading(lock, () -> {
      boolean set = false;
      int priority = Integer.MIN_VALUE;
      for (HardwareResource resource : resources) {
        var command = getCommandUsing(resource);
        if (command == null) {
          continue;
        }
        priority = Math.max(priority, command.priority());
        set = true;
      }

      if (set) return OptionalInt.of(priority);
      else return OptionalInt.empty();
    });
  }

  private static final AtomicLong invocationId = new AtomicLong(0);

  static final class CommandState {
    /**
     * A unique ID that identifies the command state. Every run of a command gets a new state,
     * even for the same command object. The ID helps to distinguish the separate runs.
     */
    final long id = invocationId.incrementAndGet();

    // assigned after construction
    Future<?> exec;
    final CompletableFuture<Boolean> schedule;
    final CompletableFuture<Object> completion;

    CommandState(CompletableFuture<Boolean> schedule, CompletableFuture<Object> completion) {
      this.schedule = schedule;
      this.completion = completion;
    }
  }

  public EventLoop getDefaultButtonLoop() {
    return eventLoop;
  }
}
