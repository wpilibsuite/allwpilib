package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.units.Units.Milliseconds;
import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Time;
import edu.wpi.first.units.Units;
import edu.wpi.first.wpilibj.event.EventLoop;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.Callable;
import java.util.concurrent.CancellationException;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;
import java.util.stream.Collectors;

public class AsyncScheduler {
  /**
   * The default update period of async commands is 20 milliseconds by convention.
   */
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
  private final ThreadFactory factory = Thread.ofVirtual().name("async-commands-runner-", 1).factory();
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
      throw new IllegalArgumentException(
          "A resource's default command must require that resource");
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
    System.out.println("Attempting to schedule " + command);

    // Don't schedule if a required resource is currently running a higher priority command
    for (Resource resource : command.requirements()) {
      var runningCommand = runningCommands.get(resource);
      if (command.isLowerPriorityThan(runningCommand)) {
        System.out.println("  Lower priority than " + runningCommand + ", not scheduling");
        return;
      }

      System.out.println("  Resource " + resource + " is available");
    }

    Util.writing(lock, () -> {
      // cancel the currently scheduled command for each required resource and schedule the incoming
      // command over them
      for (Resource resource : command.requirements()) {
        var runningCommand = runningCommands.get(resource);

        // Add to the running commands before we've technically scheduled it
        // Cancelling a command will schedule default commands (which we don't want!) if the
        // cancelled command is still mapped to a resource
        runningCommands.put(resource, command);

        if (runningCommand != null) {
          System.out.println(
              "  Cancelling running command "
                  + runningCommand
                  + " on " + resource.getName()
                  + " due to precedence of " + command);
          cancel(runningCommand);
        }
      }

      // schedule the command to run on a virtual thread, automatically scheduling the default
      // commands for all its required resources when it completes
      var futureRef = new AtomicReference<Future<?>>(null);

      System.out.println("  Submitting " + command + " to executor");
      var future = service.submit(fn(command, futureRef));

      futureRef.set(future);
      futures.put(command, future);
    });
  }

  /**
   * Checks for errors that have been thrown by the asynchronous commands. This will clear the
   * cached errors, and any errors were present, those errors will immediately be rethrown
   */
  public void checkForErrors() {
    var errors = Util.writing(lock, () -> {
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
    errors.forEach((command, error) -> {
      multi.add(new CommandExecutionException(command, error));
    });
    throw multi;
  }

  private Callable<?> fn(AsyncCommand command, AtomicReference<Future<?>> futureRef) {
    return () -> {
      System.out.println("Starting command " + command);

      try {
        command.run();
        System.out.println(command + " completed naturally");
        return null;
      } catch (Throwable e) {
        if (e instanceof InterruptedException || e instanceof CancellationException) {
          System.out.println(command + " was cancelled during execution");
        } else {
          System.out.println(command + " encountered an error during execution: " + e);
          Util.writing(lock, () -> {
            errors.put(command, e);
          });
        }
        throw e;
      } finally {
        Util.writing(lock, () -> {
          var currentMappedFuture = futures.get(command);
          var expectedFuture = futureRef.get();
          if (isRunning(command)) {
            System.out.println("Cleaning up after " + command + " completion/termination");
            Util.writing(lock, () -> {
              System.out.println("  Lock acquired, cleaning up");
              futures.remove(command);
              command.requirements().forEach(resource -> {
                runningCommands.remove(resource, command);
                scheduleDefault(resource);
              });
            });
            System.out.println("  Cleaned up after " + command);
          } else {
            System.out.println("The expected future was not running; not cleaning up after " + command + ". Expected " + expectedFuture + ", got " + currentMappedFuture);
          }
        });
      }
    };
  }

  /**
   * Waits for a command to complete. This will block until the command has completed or been
   * interrupted or cancelled by another command. Does nothing if the given command is not
   * scheduled.
   * @param command the command to wait for.
   * @throws ExecutionException if the command encountered an error while it was executing
   */
  public void await(AsyncCommand command) throws ExecutionException {
    var future = Util.reading(lock, () -> futures.get(command));
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
   * Cancels a running command and schedules the default commands for all of its required
   * resources. Has no effect if the given command is not currently running.
   *
   * @param command the command to cancel
   */
  public void cancel(AsyncCommand command) {
    Util.writing(lock, () -> {
      var future = futures.get(command);
      if (future != null) {
        System.out.println("Cancelling command " + command);
        future.cancel(true);
        if (future.isCancelled()) {
          System.out.println("  Successfully cancelled " + command);
        } else {
          throw new IllegalStateException("Execution for command " + command + " was unable to be canceled!");
        }
        futures.remove(command, future);

        command.requirements().forEach(resource -> {
          if (runningCommands.remove(resource, command)) {
            // If the currently running command for this resource is the one we just cancelled,
            // we can schedule its default command
            scheduleDefault(resource);
          }
        });
      }
    });
  }

  /**
   * Checks if a command is currently running. This cannot check a command that's part of a
   * composition or group; only commands that have been directly scheduled may return {@code true}.
   *
   * @param command the command to check
   * @return true if the command is running, false if not
   */
  public boolean isRunning(AsyncCommand command) {
    return Util.reading(lock, () -> futures.containsKey(command));
  }

  public Map<Resource, AsyncCommand> getRunningCommands() {
    return Map.copyOf(runningCommands);
  }

  /**
   * Cancels all currently running commands.
   */
  public void cancelAll() {
    Util.writing(lock, () -> runningCommands.forEach((_, runningCommand) -> cancel(runningCommand)));
  }

  /**
   * Schedules the default command for the given resource.
   *
   * @param resource the resource to schedule the default command for
   */
  public void scheduleDefault(Resource resource) {
    var defaultCommand = defaultCommands.get(resource);
    if (defaultCommand == null) {
      defaultCommand = new IdleCommand(resource);
    }

    schedule(defaultCommand);
  }

  /**
   * Gets the currently running command that uses the given resource.
   *
   * @param resource the resource to query for
   * @return
   */
  public AsyncCommand getCommandUsing(Resource resource) {
    return runningCommands.get(resource);
  }

  public void schedule(Group group) {
    var resources =
        group.stages()
            .stream()
            .flatMap(s -> s.commands().stream())
            .flatMap(c -> c.requirements().stream())
            .distinct()
            .map(Resource::getName)
            .collect(Collectors.joining(",", "[", "]"));

    // Because a group is only a bag of data with no logic, build a command that will run the
    // commands in the group and schedule that command.
    var groupWrapper = AsyncCommand.noHardware(() -> {
      System.out.println("Starting up group");
      int stageNum = 0;
      for (var stage : group.stages()) {
        stageNum++;
        System.out.println("Starting up stage " + stageNum);
        System.out.println("  Current commands " + runningCommands + ", " + futures);
        try (var scope = new StageScope<>(this, stage.getRequiredCommands())) {
          for (var command : stage.commands()) {
            scope.fork(command);
          }

          try {
            scope.joinWithTimeout(stage.getTimeout()).throwIfError();
          } catch (TimeoutException e) {
            // just means the maximum execution time was reached
            System.out.println("Scope timed out after " + stage.getTimeout().toShortString() + " for stage " + stageNum);
          }
        } finally {
          // cancel any still-running commands
          System.out.println("Cancelling still-running commands in stage " + stageNum);
          stage.commands().forEach(this::cancel);
        }
      }
    }).named("Anonymous Group on " + resources);

    schedule(groupWrapper);
  }

  public EventLoop getDefaultButtonLoop() {
    return eventLoop;
  }

  /**
   * An idle command that only parks the executing thread and does nothing else.
   *
   * @param resource the resource to idle.
   */
  private record IdleCommand(Resource resource) implements AsyncCommand {
    @Override
    @SuppressWarnings({"InfiniteLoopStatement", "BusyWait"})
    public void run() throws Exception {
      while (true) {
        Thread.sleep(Long.MAX_VALUE);
      }
    }

    @Override
    public Set<Resource> requirements() {
      return Set.of(resource);
    }

    @Override
    public String name() {
      return resource.getName() + "[IDLE]";
    }

    @Override
    public int priority() {
      // lowest priority - an idle command can be interrupted by anything else
      return LOWEST_PRIORITY;
    }

    @Override
    public String toString() {
      return name();
    }

    @Override
    public int hashCode() {
      return 31 * resource.hashCode();
    }

    @Override
    public boolean equals(Object obj) {
      if (obj instanceof IdleCommand(var sub)) {
        return sub.equals(resource);
      }
      return false;
    }
  }
}
