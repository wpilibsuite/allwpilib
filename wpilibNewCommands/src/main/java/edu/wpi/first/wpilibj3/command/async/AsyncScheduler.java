package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.units.Units.Milliseconds;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import edu.wpi.first.wpilibj.event.EventLoop;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
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
  private final Map<Resource, AsyncCommand> runningCommands = new HashMap<>();
  private final Map<Resource, AsyncCommand> defaultCommands = new HashMap<>();
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

  public void registerResource(Resource resource) {
    registerResource(resource, new IdleCommand(resource));
  }

  public void registerResource(Resource resource, AsyncCommand defaultCommand) {
    defaultCommands.put(resource, defaultCommand);
    schedule(defaultCommand);
  }

  public void setDefaultCommand(Resource resource, AsyncCommand defaultCommand) {
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

  public AsyncCommand getDefaultCommand(Resource resource) {
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
    for (Resource resource : command.requirements()) {
      var runningCommand = runningCommands.get(resource);
      if (command.isLowerPriorityThan(runningCommand)) {
        return;
      }
    }

    Util.writing(
        lock,
        () -> {
          // Cancel the currently scheduled commands for each required resource and schedule the
          // incoming command over them
          for (Resource resource : command.requirements()) {
            var runningCommand = runningCommands.get(resource);

            // Prevent nested commands from interrupting their parent
            boolean isNested = false;
            var checked = command;
            while (checked instanceof NestedCommand(var parent, var _impl)) {
              if (parent.equals(runningCommand)) {
                isNested = true;
                break;
              }
              for (var shadowed : shadowrun.getOrDefault(resource, NopSet.nop())) {
                if (shadowed.equals(parent)) {
                  isNested = true;
                  break;
                }
                if (shadowed instanceof NestedCommand(var shadowParent, var shadowImpl)) {
                  if (shadowImpl.equals(parent)) {
                    isNested = true;
                    break;
                  }
                }
              }
              checked = parent;
            }

            if (isNested) {
              shadowrun.computeIfAbsent(resource, _r -> new HashSet<>()).add(command);
              continue;
            }

            // Add to the running commands before we've technically scheduled it
            // Cancelling a command will schedule default commands (which we don't want!) if the
            // cancelled command is still mapped to a resource
            runningCommands.put(resource, command);
            shadowrun.computeIfAbsent(resource, _r -> new HashSet<>()).add(command);

            if (runningCommand != null) {
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

  private final Map<Resource, Collection<AsyncCommand>> shadowrun = new HashMap<>();

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
      if (shadowrun.getOrDefault(resource, NopSet.nop()).remove(command)) {
        // This command was a nested command inside a command group
        // It will not be in runningCommands (that's only top-level stuff)
        // and should not schedule any default commands
        // (the parent command's cleanup will handle that)
        continue;
      }

      // If the currently running command for this resource is the one we just
      // cancelled, we can schedule its default command
      if (runningCommands.remove(resource, command)) {
        scheduleDefault(resource);
      }
    }
  }

  /**
   * Checks if a command is currently running. This cannot check a command that's part of a
   * composition or group; only commands that have been directly scheduled may return {@code true}.
   *
   * @param command the command to check
   * @return true if the command is running, false if not
   */
  public boolean isRunning(AsyncCommand command) {
    return Util.reading(lock, () -> futureFor(command) != null);
  }

  public Map<Resource, AsyncCommand> getRunningCommands() {
    return Map.copyOf(runningCommands);
  }

  /** Cancels all currently running commands. */
  public void cancelAll() {
    Util.writing(
        lock, () -> runningCommands.forEach((_resource, runningCommand) -> cancel(runningCommand)));
  }

  /**
   * Schedules the default command for the given resource. If no default command exists, then a new
   * idle command will be created and assigned as the default.
   *
   * @param resource the resource to schedule the default command for
   */
  public void scheduleDefault(Resource resource) {
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
  public AsyncCommand getCommandUsing(Resource resource) {
    return runningCommands.get(resource);
  }

  public EventLoop getDefaultButtonLoop() {
    return eventLoop;
  }
}
