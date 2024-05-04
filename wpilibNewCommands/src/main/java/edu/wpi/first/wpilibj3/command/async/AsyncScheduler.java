package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.units.Units.Milliseconds;
import static edu.wpi.first.units.Units.Nanoseconds;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import edu.wpi.first.wpilibj.event.EventLoop;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.OptionalInt;
import java.util.concurrent.Callable;
import java.util.concurrent.CancellationException;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

public class AsyncScheduler {
  /** The default update period of async commands is 20 milliseconds by convention. */
  public static final Measure<Time> DEFAULT_UPDATE_PERIOD = Milliseconds.of(20);

  private final Map<AsyncCommand, Throwable> errors = new HashMap<>();

  /**
   * The top-level running commands. This would include the highest-level command groups but not any
   * of their nested commands.
   */
  private final Map<HardwareResource, AsyncCommand> runningCommands = new HashMap<>();

  /**
   * Maps <i>all</i> running commands to their resources. Used for tracking exactly what commands
   * are running. Nested commands executed by command groups are wrapped in a {@link NestedCommand}
   * object to track what group forked them.
   */
  final Map<HardwareResource, Collection<AsyncCommand>> shadowrun = new HashMap<>();

  private final Map<HardwareResource, AsyncCommand> defaultCommands = new HashMap<>();
  private final Map<AsyncCommand, Future<?>> futures = new HashMap<>();
  private final EventLoop eventLoop = new EventLoop();

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
    service.submit(() -> pollEventLoop(triggerPollingPeriod));
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
    cancel(oldDefaultCommand);
  }

  public AsyncCommand getDefaultCommand(HardwareResource resource) {
    return defaultCommands.get(resource);
  }

  /**
   * Schedules a command to run. The command will execute in a virtual thread; its status may be
   * checked later with {@link #isRunning(AsyncCommand)}. Scheduling will fail if the command has a
   * lower priority than a currently running command using one or more of the same resources.
   *
   * @param command the command to schedule
   */
  public void schedule(AsyncCommand command) {
    // Don't schedule if a required resource is currently running a higher priority command
    boolean shouldSchedule =
        Util.reading(
            lock,
            () -> {
              for (HardwareResource resource : command.requirements()) {
                var runningCommand = runningCommands.get(resource);
                if (command.isLowerPriorityThan(runningCommand)) {
                  return false;
                }
              }
              return true;
            });

    if (!shouldSchedule) {
      return;
    }

    Util.writing(
        lock,
        () -> {
          // Cancel the currently scheduled commands for each required resource and schedule the
          // incoming command over them
          for (HardwareResource resource : command.requirements()) {
            // Don't allow nested commands to cancel their parents
            if (command instanceof NestedCommand(var parent, var _impl)) {
              if (!shadowrun.getOrDefault(resource, NopSet.nop()).contains(parent)) {
                throw new IllegalStateException(
                    "Nested commands can only be scheduled while their parents are running! "
                        + command
                        + " requires "
                        + parent);
              }

              // Shadow the nested command as well as its the command it wraps (even though the
              // wrapped command never runs)
              var shadowSet = shadowrun.computeIfAbsent(resource, _r -> new HashSet<>());
              shadowSet.add(command);
              shadowSet.add(((NestedCommand) command).impl());
              continue;
            }

            var runningCommand = runningCommands.get(resource);

            // Add to the running commands before we've technically scheduled it
            // Cancelling a command will schedule default commands (which we don't want!) if the
            // cancelled command is still mapped to a resource
            runningCommands.put(resource, command);
            shadowrun.computeIfAbsent(resource, _r -> new HashSet<>()).add(command);

            if (runningCommand != null
                    && runningCommand.interruptBehavior() == AsyncCommand.InterruptBehavior.Cancel) {
              cancel(runningCommand);
            }
          }

          // schedule the command to run on a virtual thread, automatically scheduling the default
          // commands for all its required resources when it completes
          var future = service.submit(createCommandExecutorCallback(command));
          futures.put(command, future);
          return future;
        });
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

  /**
   * Creates a wrapper callback that manages execution of an async command and the associated
   * cleanup after it completes or is cancelled.
   *
   * @param command the command to wrap
   * @return a callback that can be scheduled on an executor service
   */
  private Callable<?> createCommandExecutorCallback(AsyncCommand command) {
    return () -> {
      commandOnCurrentThread.set(command);
      try {
        command.run();
        return null;
      } catch (Throwable e) {
        if (e instanceof InterruptedException || e instanceof CancellationException) {
          // Cancelled during execution. This is expected and totally fine.
        } else {
          // Command execution raised an error.
          // Cache it in the errors map for `checkForErrors` to pick up.
          Util.writing(lock, () -> errors.put(command, e));
        }
        // Rethrow the exception. This will make the future complete with an error state.
        // If the caller uses `await` on the command or `get` on its associated future,
        // then this exception will be raised at the call site. Otherwise, it will have
        // to be picked up by calling `checkForErrors` (presumably in a periodic loop)
        throw e;
      } finally {
        Util.writing(
            lock,
            () -> {
              if (!isRunning(command)) {
                // The command was canceled.
                // There's no cleanup to do; the cancellation process will have handled it
                return;
              }

              cleanupAfterCompletion(command);
            });
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
    var future = futureFor(command);

    if (future == null) {
      // not currently running, nothing to await
      return;
    }

    try {
      future.get();
    } catch (InterruptedException e) {
      Thread.currentThread().interrupt();
    } catch (CancellationException e) {
      // Command was cancelled, nothing wrong here
    }
  }

  /**
   * Cancels a running command and schedules the default commands for all of its required resources.
   * Has no effect if the given command is not currently running.
   *
   * @param command the command to cancel
   */
  public void cancel(AsyncCommand command) {
    Util.writing(
        lock,
        () -> {
          var future = futureFor(command);
          if (future == null) {
            // Not running, nothing to cancel
            return;
          }

          if (!future.cancel(true)) {
            throw new IllegalStateException(
                "Execution for command " + command + " was unable to be canceled!");
          }

          cleanupAfterCompletion(command);
        });
  }

  private Future<?> futureFor(AsyncCommand command) {
    return Util.reading(
        lock,
        () -> {
          return futures.computeIfAbsent(
              command,
              c -> {
                for (var entry : futures.entrySet()) {
                  if (entry.getKey() instanceof NestedCommand(var parent, var impl)) {
                    if (impl.equals(command)) {
                      return entry.getValue();
                    }
                  }
                }
                return null;
              });
        });
  }

  /**
   * Cleans up state after a command completes or is cancelled. This method is not inherently
   * thread-safe and must be wrapped in calls to the write lock.
   *
   * @param command the command to clean up after
   */
  private void cleanupAfterCompletion(AsyncCommand command) {
    futures
        .entrySet()
        .removeIf(
            (e) -> {
              return e.getKey().equals(command)
                  || (e.getKey() instanceof NestedCommand(var _parent, var impl)
                      && impl.equals(command));
            });

    for (var resource : command.requirements()) {
      // Remove from the shadowrun:
      //   1. All instances of the completed command;
      //   2. Any nested commands that wrapped the completed command;
      //   3. The implementation of the completed command, if it was a wrapper
      var shadowSet = shadowrun.getOrDefault(resource, NopSet.nop());
      shadowSet.remove(command);
      shadowSet.removeIf(
          (c) -> c instanceof NestedCommand(var _p, var impl) && impl.equals(command));
      shadowSet.removeIf(
          (c) -> command instanceof NestedCommand(var _p, var impl) && impl.equals(c));

      // If the currently running command for this resource is the one we just
      // cancelled, we can schedule its default command
      if (runningCommands.remove(resource, command)) {
        scheduleDefault(resource);
      }
    }
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
    return Util.reading(lock, () -> futureFor(command) != null);
  }

  /**
   * @return
   */
  public Map<HardwareResource, AsyncCommand> getRunningCommands() {
    return Map.copyOf(runningCommands);
  }

  /** Cancels all currently running commands. */
  public void cancelAll() {
    Util.writing(
        lock,
        () -> List.copyOf(runningCommands.values()).forEach(this::cancel));
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
   *     using it
   */
  public AsyncCommand getCommandUsing(HardwareResource resource) {
    return Util.reading(lock, () -> runningCommands.get(resource));
  }

  private final ThreadLocal<AsyncCommand> commandOnCurrentThread = new ThreadLocal<>();

  /**
   * Pauses the current command for 20 milliseconds. If a higher priority command on
   * any of the current command's required resources is scheduled while paused, and if the current
   * command is set to {@link AsyncCommand.InterruptBehavior#Suspend}, then the command will
   * continue to stay paused as long as any higher priority command is running. Otherwise, a higher
   * priority command will interrupt the command while it is paused.
   *
   * @throws InterruptedException if the current command was canceled, or, if the command's
   *  {@link AsyncCommand#interruptBehavior() interrupt behavior} is not set to suspend,
   *  interrupted by a higher priority command
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
   *  {@link AsyncCommand#interruptBehavior() interrupt behavior} is not set to suspend,
   *  interrupted by a higher priority command
   */
  @SuppressWarnings("BusyWait")
  public void pauseCurrentCommand(Measure<Time> time) throws InterruptedException {
    var command = commandOnCurrentThread.get();
    if (command == null) {
      throw new IllegalStateException(
          "pauseCurrentCommand() may only be called by a running command!");
    }

    long ms = (long) time.in(Milliseconds);
    int ns = (int) (time.in(Nanoseconds) % 1e6);

    // Always sleep once
    Thread.sleep(ms, ns);

    // Then, if the command is configured to suspend while higher priority commands run,
    // continue to sleep in a loop. If we're ever interrupted, it means the command was canceled
    // outright, and we should respect the cancellation
    if (command.interruptBehavior() == AsyncCommand.InterruptBehavior.Suspend) {
      // TODO: We allow commands to interrupt running commands with the same priority level.
      //       However, this check only pauses if a HIGHER priority command is running, and thus can
      //       allow two commands of the same priority level to run concurrently. THIS IS BAD
      for (var highestPriority = getHighestPriorityCommandUsingAnyOf(command.requirements());
           highestPriority.isPresent() && highestPriority.getAsInt() >= command.priority();
           highestPriority = getHighestPriorityCommandUsingAnyOf(command.requirements())) {
        Thread.sleep(ms, ns);
      }
    }
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

  public Collection<AsyncCommand> getAllCommandsUsing(HardwareResource resource) {
    return Collections.unmodifiableCollection(shadowrun.getOrDefault(resource, NopSet.nop()));
  }

  public EventLoop getDefaultButtonLoop() {
    return eventLoop;
  }
}
