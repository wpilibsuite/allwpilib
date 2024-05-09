package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.units.Units.Microseconds;
import static edu.wpi.first.units.Units.Milliseconds;
import static edu.wpi.first.units.Units.Nanoseconds;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import edu.wpi.first.util.WPIUtilJNI;
import edu.wpi.first.wpilibj.event.EventLoop;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
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
  @interface Reads {}
  @interface Writes {}
  @interface NotLocking {}

  static {
    String vthreadParallelism = System.getProperty("jdk.virtualThreadScheduler.parallelism");
    if (!Objects.equals(vthreadParallelism, "1")) {
      throw new ExceptionInInitializerError(
          "The async command framework requires -Djdk.virtualThreadScheduler.parallelism=1, but was set to " + Objects.toString(vthreadParallelism));
    }
  }

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
    this(triggerPollingPeriod, Milliseconds.of(2.5), Milliseconds.of(2.5));
  }

  public AsyncScheduler(Measure<Time> triggerPollingPeriod, Measure<Time> cancelTimeout, Measure<Time> scheduleTimeout) {
    // Start polling the event loop
    // This intentionally runs on the same carrier thread as commands so the boolean checks
    // will be guaranteed to run on the same thread as the commands that manipulate state
    addPeriodic(this::pollEventLoop);

    this.cancelTimeout = cancelTimeout.copy();
    this.scheduleTimeout = scheduleTimeout.copy();
  }

  private void pollEventLoop() {
    try {
      Thread.sleep(20);
      eventLoop.poll();
    } catch (Exception e) {
      // ignore and keep running
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

  public void addPeriodic(ThrowingRunnable task, Measure<Time> period) {
    long ms = (long) period.in(Milliseconds);
    int ns = (int) (period.in(Nanoseconds) % 1e6);

    service.submit(() -> {
      while (true) {
        Thread.sleep(ms, ns);
        task.run();
      }
    });
  }

  public void addPeriodic(ThrowingRunnable task) {
    addPeriodic(task, DEFAULT_UPDATE_PERIOD);
  }

  @Reads
  private boolean isSchedulable(AsyncCommand command) {
    return Util.reading(lock, () -> {
      // Don't schedule if a required resource is currently running a higher priority command
      boolean shouldSchedule = true;
      for (HardwareResource resource : command.requirements()) {
        var runningCommand = runningCommands.get(resource);
        if (command.isLowerPriorityThan(runningCommand)) {
          shouldSchedule = false;
          break;
        }
      }

      if (shouldSchedule) {
        Logger.log("SCHEDULE", "Command " + command.name() + " is higher priority than all conflicting commands. Cancelling the running commands");
        return true;
      } else {
        Logger.log("SCHEDULE", "Lower priority than running commands, not scheduling");
        return false;
      }
    });
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
  @Writes
  public void schedule(AsyncCommand command) {
    Logger.log("SCHEDULE", "Scheduling " + command.name());
    final long start = System.nanoTime();

    var currentState = Util.reading(lock, () -> commandStates.get(command));
    if (currentState != null) {
      Logger.log("SCHEDULE", "Command " + command.name() + " is already running (run ID=" + currentState.id + "), not rescheduling");
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
          if (!isSchedulable(command)) {
            return false;
          }

          // Cancel the currently scheduled commands for each required resource and schedule the
          // incoming command over them
          for (HardwareResource resource : command.requirements()) {
            var runningCommand = runningCommands.get(resource);

            if (runningCommand != null) {
              // Cancel the current command, but don't start the default command for the resource
              // because we're about to start our own command that requires it
              // Fire the event but don't wait for completion, because that will deadlock.
              // Instead, after exiting this block, we wait for all the canceled commands to exit
              // OUTSIDE the lock
              canceledCommandStates.add(fireCancelEvent(runningCommand));
            }

            // Add to the running commands before we've technically scheduled it
            // Cancelling a command will schedule default commands (which we don't want!) if the
            // cancelled command is still mapped to a resource
            runningCommands.put(resource, command);
          }
          return true;
        });

    if (!isSchedulable) {
      // Not schedulable, bail
      return;
    }

    // Wait for cancellation OUTSIDE the lock
    // NOTE: If the lock is owned by the current thread, this has the potential to deadlock!
    awaitBulkCancellation(canceledCommandStates);

    var newCommandStates = fireScheduleEvent(command);

    // Wait for the command to have started before returning
    // Note: this may take up to 1 or 2 milliseconds! Normally seen on the order of ~0.1ms
    try {
      awaitBulkSchedulation(List.of(newCommandStates));
    } finally {
      long end = System.nanoTime();
      Logger.log("SCHEDULE", "Took " + (end - start) / 1e3 + " µs to schedule " + command.name() + " (assigned run ID=" + newCommandStates.id + ")");
    }
  }

  @NotLocking
  private void awaitBulkSchedulation(Collection<CommandState> scheduledCommandStates) {
    try {
      CompletableFuture
          .allOf(scheduledCommandStates.stream().map(s -> s.schedule).toArray(CompletableFuture[]::new))
          .get((long) scheduleTimeout.in(Nanoseconds), TimeUnit.NANOSECONDS);
    } catch (InterruptedException e) {
      throw new IllegalStateException("Interrupted while waiting for all commands to start", e);
    } catch (ExecutionException e) {
      // Shouldn't happen, the "schedule" future is marked complete before the command begins to execute
      throw new IllegalStateException("One or more scheduled commands encountered an internal exception", e.getCause());
    } catch (TimeoutException e) {
      var unscheduled = scheduledCommandStates.stream().filter(s -> !s.schedule.isDone()).map(s -> s.command.name() + " (run ID=" + s.id + ")").toList();
      String message = "Timed out while waiting for all scheduled commands to start. Still waiting on: " + unscheduled + ". Locks: " + Util.owners();
      Logger.log("AWAIT BULK SCHEDULATION", message);
      try {
        Files.writeString(Paths.get("thread-dump.txt"), Logger.formattedLogTable());
      } catch (IOException ignore) {}
      throw new IllegalStateException(message, e);
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
      var stillRunning = canceledCommandStates.stream().filter(s -> !s.completion.isDone()).map(s -> s.command.name() + " (run ID=" + s.id + ")").toList();
      String message = "Timed out while waiting for all cancelled commands to complete. Still running: " + stillRunning + ". Locks: " + Util.owners();
      Logger.log("AWAIT BULK CANCELLATION", message);
      try {
        Files.writeString(Paths.get("thread-dump.txt"), Logger.formattedLogTable());
      } catch (IOException ignore) {}
      throw new IllegalStateException(message, e);
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

      boolean wasInterrupted = false;
      Object result = null;
      Throwable exception = null;
      try {
        Logger.log("COMMAND", "Alerting scheduler about command startup for " + command.name() + " (run ID=" + runId + ")");
        Util.writing(lock, () -> {
          Logger.log("COMMAND", "Setting thread local command owner to " + command.name() + " (run ID=" + runId + ")");
          commandOnCurrentThread.set(state);

          Logger.log("COMMAND", "Completing the schedule future for " + command.name() + " (run ID=" + runId + ")");
          state.schedule.complete(true);
        });

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
        Logger.log("COMMAND", "Acquiring lock..." + " (run ID=" + runId + ", owners: " + Util.owners() + ")");
        // Not using Util.writing() because `wasInterrupted` can't be captured
        lock.writeLock().lock();
        Util.push();
        Logger.log("COMMAND", "Lock acquired" + " (run ID=" + runId + ")");
        try {
          if (commandStates.get(command) instanceof CommandState s && s.id == runId) {
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
          try {
            if (exception != null) {
              state.completion.completeExceptionally(exception);
            } else if (result != null) {
              state.completion.complete(result);
            } else {
              // note: result shouldn't be null, but just in case...
              state.completion.complete(COMPLETED_VALUE);
            }
          } finally {
            Util.pop();
            lock.writeLock().unlock();
          }
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
  @Reads
  public void await(AsyncCommand command) throws ExecutionException {
    Logger.log("AWAIT", "Awaiting " + command.name());
    var states = Util.reading(lock, () -> commandStates.get(command));

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

  @Writes
  private CommandState fireScheduleEvent(AsyncCommand command) {
    return Util.writing(
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
          Logger.log("SCHEDULE", "Created async future for command " + command.name() + " (assigned run ID=" + states.id + ")");
          Logger.log("SCHEDULE", "All command states: " + commandStates);
          return states;
        });
  }

  @Writes
  private CommandState fireCancelEvent(AsyncCommand command) {
    if (command == null) {
      // Nothing to do
      return null;
    }

    Logger.log("CANCEL", "Cancelling command " + command.name());
    return Util.writing(
        lock,
        () -> {
          var state = commandStates.get(command);
          if (state == null) {
            // Not running, nothing to cancel
            Logger.log("CANCEL", "Command " + command.name() + " is not running. Nothing to cancel");
            return null;
          }

          Logger.log("CANCEL", "Triggering interrupt for command " + command.name() + " (run ID=" + state.id + ")");
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
  @Writes
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
      states.completion.get((long) cancelTimeout.in(Nanoseconds), TimeUnit.NANOSECONDS);
      Logger.log("CANCEL", "Command " + command.name() + " (run ID=" + runId + ") exited within " + cancelTimeout.in(Microseconds) + " µs of the cancellation request");

      // Clean up only after the command has completed
      cleanupAfterCompletion(command, thenRunDefault);
    } catch (TimeoutException e) {
      String message = "Command " + command.name() + " (run ID=" + runId + ") did not stop within " + cancelTimeout.in(Microseconds) + " µs of the cancellation request!";
      Logger.log("CANCEL", message);
      Logger.log("CANCEL", "Current lock owner: " + Util.getCurrentOwnerName());
      try {
        Files.writeString(Path.of("thread-dump.txt"), Logger.formattedLogTable());
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

  /**
   * Cleans up state after a command completes or is cancelled. This method is not inherently
   * thread-safe and must be wrapped in calls to the write lock.
   *
   * @param command the command to clean up after
   */
  @NotLocking
  private void cleanupAfterCompletion(AsyncCommand command, boolean thenRunDefault) {
    // TODO: This may be buggy at the boundary where a command completes and is immediately rescheduled
    var states = commandStates.get(command);
    if (states == null) {
      Logger.log("CLEANUP", "Cannot clean up for a command that isn't running! Requested command " + command.name() + " has no associated states");
      return;
    }

    long runId = states.id;
    Logger.log("CLEANUP", "Removing future states for " + command.name() + " (run ID=" + runId + ")");
    commandStates.remove(command);

    // Clear current thread owner
    if (commandOnCurrentThread.get().command == command) {
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
   * Checks if a command is currently running.
   *
   * @param command the command to check
   * @return true if the command is running, false if not
   */
  @Reads
  public boolean isRunning(AsyncCommand command) {
    return Util.reading(lock, () -> commandStates.containsKey(command));
  }

  /**
   * @return
   */
  @Reads
  public Map<HardwareResource, AsyncCommand> getRunningCommands() {
    return Util.reading(lock, () -> Map.copyOf(runningCommands));
  }

  /**
   * Cancels all currently running commands, then starts the default commands for any resources
   * that had canceled commands. A default command that is currently running will not be canceled.
   */
  @Writes
  public void cancelAll() {
    Logger.log("CANCELALL", "Cancelling all commands");
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
    Logger.log("CANCELALL", "All interrupts fired, waiting for commands to exit...");
    awaitBulkCancellation(canceledStates);
    Logger.log("CANCELALL", "All commands successfully exited, starting default commands...");
    var newStates = Util.writing(lock, () -> {
      return
          defaultCommands
              .values()
              .stream()
              .filter(c -> !isRunning(c)) // Don't schedule a default command that's already running
              .filter(this::isSchedulable)
              .map(this::fireScheduleEvent)
              .toList();
    });
    // WAIT OUTSIDE THE LOCK
    awaitBulkSchedulation(newStates);
    Logger.log("CANCELALL", "Complete");
  }

  /**
   * Schedules the default command for the given resource. If no default command exists, then a new
   * idle command will be created and assigned as the default.
   *
   * @param resource the resource to schedule the default command for
   */
  @Writes
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
  @Reads
  public AsyncCommand getCommandUsing(HardwareResource resource) {
    return Util.reading(lock, () -> runningCommands.get(resource));
  }

  private static final ThreadLocal<CommandState> commandOnCurrentThread = new ThreadLocal<>();

  /**
   * Pauses the current command for 20 milliseconds. If a higher priority command on
   * any of the current command's required resources is scheduled while paused, the higher
   * priority command will interrupt the command while it is paused.
   *
   * @throws InterruptedException if the current command was canceled or interrupted by a higher
   *   priority command
   */
  @Reads
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
  @Reads
  public void pauseCurrentCommand(Measure<Time> time) throws InterruptedException {
    var states = commandOnCurrentThread.get();
    if (states == null) {
      throw new IllegalStateException(
          "pauseCurrentCommand() may only be called by a running command!");
    }
    var command = states.command;

    Logger.log("PAUSE", "Pausing " + command.name());

//    var states = Util.reading(lock, () -> {
//      Logger.log("PAUSE", "All states: " + commandStates);
//      return commandStates.get(command);
//    });
//
//    if (states == null) {
//      System.err.println(Logger.formattedLogTable());
//      throw new IllegalStateException("Command " + command.name() + " is not tracked!");
//    }

    if (Thread.currentThread().isInterrupted()) {
      // Thread interrupt flag was set prior to entering pause
      // This can happen if command code uses Timer.delay or something similar where an interrupted
      // exception is caught and the interrupt flag is set
      Logger.log("PAUSE", "Command " + command.name() + " was interrupted without raising an exception");
      throw new InterruptedException();
    }

    long ms = (long) time.in(Milliseconds);
    int ns = (int) (time.in(Nanoseconds) % 1e6);

    long start = WPIUtilJNI.now();
    Logger.log("PAUSE", "Parking for " + time.in(Microseconds) + " µs...");
    Thread.sleep(ms, ns);
    long end = WPIUtilJNI.now();
    Logger.log("PAUSE", "Woke up after " + (end - start) + " µs waiting (target=" + time.in(Microseconds) + " µs)");

    if (Thread.currentThread().isInterrupted()) {
      // Interrupted while paused, but without an interrupted exception being thrown?
      Logger.log("PAUSE", "Command " + command.name() + " was interrupted while parked without raising an exception!");
      System.err.println(Logger.formattedLogTable());
      throw new IllegalStateException();
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

    CommandState(
        AsyncCommand command,
        CompletableFuture<Boolean> schedule,
        CompletableFuture<Object> completion) {
      this.command = command;
      this.schedule = schedule;
      this.completion = completion;
    }

    @Override
    public String toString() {
      return "CommandState(" +
                 "id=" + id +
                 ", command=" + command.name() +
                 ", exec=" + exec.state() +
                 ", schedule=" + schedule.state() +
                 ", completion=" + completion.state() +
                 ')';
    }
  }

  public EventLoop getDefaultButtonLoop() {
    return eventLoop;
  }
}
