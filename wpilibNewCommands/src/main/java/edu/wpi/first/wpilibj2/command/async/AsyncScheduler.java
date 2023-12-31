package edu.wpi.first.wpilibj2.command.async;

import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.Callable;
import java.util.concurrent.CancellationException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

public class AsyncScheduler {
  private final Map<Resource, AsyncCommand> runningCommands = new HashMap<>();
  private final Map<Resource, AsyncCommand> defaultCommands = new HashMap<>();
  private final Map<AsyncCommand, Future<?>> futures = new HashMap<>();

  private final ThreadFactory factory = Thread.ofVirtual().name("async-commands").factory();
  private final ExecutorService service = Executors.newSingleThreadExecutor(factory);

  private static final AsyncScheduler defaultScheduler = new AsyncScheduler();

  private final ReadWriteLock lock = new ReentrantReadWriteLock();

  public static AsyncScheduler getDefault() {
    return defaultScheduler;
  }

  public void registerResource(Resource resource) {
    registerResource(resource, new IdleCommand(resource));
  }

  public void registerResource(Resource resource, AsyncCommand defaultCommand) {
    runningCommands.put(resource, defaultCommand);
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

  /**
   * Schedules a command to run. The command will execute in a virtual thread; its status may be
   * checked later with {@link #isRunning(AsyncCommand)}. Scheduling will fail if the command is
   * already running or has a lower priority than a currently running command using one or more of
   * the same resources.
   *
   * @param command the command to schedule
   */
  public void schedule(AsyncCommand command) {
    System.out.println("Attempting to schedule " + command);
    if (isRunning(command)) {
      // Already running, don't bother rescheduling
      return;
    }

    // Don't schedule if a required resource is currently running a higher priority command
    for (Resource resource : command.requirements()) {
      var runningCommand = runningCommands.get(resource);
      if (command.isLowerPriorityThan(runningCommand)) {
        System.out.println("  Lower priority than " + runningCommand + ", not scheduling");
        return;
      }

      System.out.println("  Resource " + resource + " is available");
    }

    // cancel the currently scheduled command for each required resource and schedule the incoming
    // command over them
    for (Resource resource : command.requirements()) {
      var runningCommand = runningCommands.get(resource);
      if (runningCommand != null) {
        System.out.println(
            "  Cancelling running command "
                + runningCommand
                + " on " + resource.getName()
                + " due to precedence of " + command);
        cancel(runningCommand);
      }
      runningCommands.put(resource, command);
    }

    Util.writing(lock, () -> {
      // schedule the command to run on a virtual thread, automatically scheduling the default
      // commands for all its required resources when it completes
      var futureRef = new AtomicReference<Future<?>>(null);

      System.out.println("  Submitting " + command + " to executor");
      var future = service.submit(fn(command, futureRef));

      futureRef.set(future);
      futures.put(command, future);
    });
  }

  private Callable<Object> fn(AsyncCommand command, AtomicReference<Future<?>> futureRef) {
    return () -> {
      try {
        command.run();
        System.out.println(command + " completed naturally");
        return null;
      } catch (Throwable e) {
        if (e instanceof InterruptedException || e instanceof CancellationException) {
          System.out.println(command + " was cancelled during execution");
        } else {
          System.out.println(command + " encountered an error during execution: " + e);
        }
        throw e;
      } finally {
        System.out.println("Cleaning up after " + command + " completion/termination");
        Util.writing(lock, () -> futures.remove(command, futureRef.get()));
        command.requirements().forEach(resource -> {
          runningCommands.remove(resource, command);
          scheduleDefault(resource);
        });
      }
    };
  }

  public void await(AsyncCommand command) throws Exception {
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
          runningCommands.remove(resource, command);
          scheduleDefault(resource);
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

  public void schedule(Group<AsyncCommand> group) {
    schedule(AsyncCommand.noHardware(() -> {
      int stageNum = 0;
      for (var stage : group.stages()) {
        stageNum++;
        System.out.println("Starting up stage " + stageNum);
        System.out.println("  Current commands " + runningCommands + ", " + futures);
        try (var scope = new StageScope<>(this, stage.getRequiredCommands())) {
          for (var command : stage.commands()) {
            scope.fork(command);
          }

          if (stage.getTimeout() == null) {
            scope.join().throwIfError();
          } else {
            try {
              scope.joinWithTimeout(stage.getTimeout()).throwIfError();
            } catch (TimeoutException e) {
              // just means the maximum execution time was reached
              System.out.println("Scope timed out after " + stage.getTimeout().toShortString() + " for stage " + stageNum);
            }
          }
        } finally {
          // cancel any still-running commands
          System.out.println("Cancelling still-running commands in stage " + stageNum);
          stage.commands().forEach(this::cancel);
        }
      }
    }));
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
