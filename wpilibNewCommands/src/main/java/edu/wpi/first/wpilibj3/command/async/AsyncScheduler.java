package edu.wpi.first.wpilibj3.command.async;

import edu.wpi.first.wpilibj.event.EventLoop;
import java.lang.invoke.MethodHandle;
import java.lang.invoke.MethodHandles;
import java.lang.invoke.MethodType;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class AsyncScheduler {
  private static final MethodHandle java_lang_thread_setContinuation;

  static {
    try {
      var lookup = MethodHandles.privateLookupIn(Thread.class, MethodHandles.lookup());

      java_lang_thread_setContinuation = lookup.findVirtual(
          Thread.class,
          "setContinuation",
          MethodType.methodType(void.class, Continuation.jdk_internal_vm_Continuation)
      );
    } catch (Throwable t) {
      throw new ExceptionInInitializerError(t);
    }
  }

  /**
   * The top-level running commands. This would include the highest-level command groups but not any
   * of their nested commands.
   */
  private final Map<HardwareResource, AsyncCommand> defaultCommands = new HashMap<>();
  private final Set<AsyncCommand> onDeck = new LinkedHashSet<>();
  private final Map<AsyncCommand, CommandState> commandStates = new LinkedHashMap<>();
  private final List<Continuation> periodicCallbacks = new ArrayList<>();
  private final EventLoop eventLoop = new EventLoop();

  private AsyncCommand currentCommand = null;

  private final ContinuationScope scope = new ContinuationScope("async command");

  private static AsyncScheduler defaultScheduler = new AsyncScheduler();

  // package-private for tests
  static void setDefaultScheduler(AsyncScheduler defaultScheduler) {
    AsyncScheduler.defaultScheduler = defaultScheduler;
  }

  public static AsyncScheduler getInstance() {
    return defaultScheduler;
  }

  public AsyncScheduler() {
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

    defaultCommands.put(resource, defaultCommand);
    schedule(defaultCommand);
  }

  public AsyncCommand getDefaultCommand(HardwareResource resource) {
    return defaultCommands.get(resource);
  }

  public void sideload(Runnable callback) {
    periodicCallbacks.add(new Continuation(scope, callback));
  }

  public void addPeriodic(Runnable callback) {
    sideload(() -> {
      while (true) {
        AsyncScheduler.this.yield();
        callback.run();
      }
    });
  }

  public void scheduleComposed(AsyncCommand command, AsyncCommand parent) {
    if (currentCommand == parent) {
      // The parent command is currently executing; skip conflict checks
      onDeck.add(command);
    } else {
      // The parent isn't currently running
      schedule(command);
    }
  }

  /**
   * Schedules a command to run. The command will execute in a virtual thread; its status may be
   * checked later with {@link #isRunning(AsyncCommand)}. Scheduling will fail if the command has a
   * lower priority than a currently running command using one or more of the same resources.
   *
   * @param command the command to schedule
   */
  public void schedule(AsyncCommand command) {
    Logger.log("SCHEDULE", "Attempting to schedule " + command);
    for (AsyncCommand c : commandStates.keySet()) {
      if (c.conflictsWith(command) && command.isLowerPriorityThan(c)) {
        Logger.log("SCHEDULE", command + " conflicts with and is lower priority than " + c + ", not scheduling");
        return;
      }
    }

    for (var scheduledCommand : onDeck) {
      if (!command.conflictsWith(scheduledCommand)) {
        // No shared requirements, skip
        continue;
      }
      if (command.isLowerPriorityThan(scheduledCommand)) {
        // Lower priority than an already-scheduled (but not yet running) command that requires at
        // one of the same resource
        Logger.log("SCHEDULE", command + " is lower priority than already-scheduled command " + scheduledCommand + ", not scheduling");
        return;
      }
    }

    // Evict conflicting on-deck commands
    // We check above if the input command is lower priority than any of these,
    // so at this point we're guaranteed to be >= priority than anything already on deck
    onDeck.removeIf(c -> c.conflictsWith(command));

    Logger.log("SCHEDULE", "Adding " + command + " to on deck commands");
    onDeck.add(command);

    Logger.log("SCHEDULE", "Dropping commands that conflict with " + command);
    commandStates.entrySet().removeIf(e -> e.getKey().conflictsWith(command));
  }

  public void cancel(AsyncCommand command) {
    // Evict the command. The next call to run() will schedule the default command for all its
    // required resources, unless another command requiring those resources is scheduled between
    // calling cancel() and calling run()
    commandStates.remove(command);
  }

  private int runNum = 0;

  public void run() {
    runNum++;
    Logger.log("RUN", "Starting run #" + runNum);
    // Apply scheduled commands, evicting any commands that require the same resources
    // Priority checking is done at schedule time so we don't need to repeat those checks here
    // Note that if multiple commands are scheduled in the same loop that use the same resources,
    // then the final scheduled command will win out.
    for (var command : onDeck) {
      Logger.log("RUN", "Building continuation for " + command);
      commandStates.put(command, new CommandState(command, buildContinuation(command)));
    }

    // Clear the set of on-deck commands,
    // since we just put them all into the set of running commands
    Logger.log("RUN", "Clearing scheduled commands");
    onDeck.clear();

    Logger.log("RUN", "Executing periodic callbacks");
    for (Continuation callback : periodicCallbacks) {
      callback.run();
    }

    periodicCallbacks.removeIf(Continuation::isDone);

    // Tick every command that hasn't been completed yet
    Logger.log("RUN", "Updating commands (" + commandStates.keySet() + ")");
    for (var entry : List.copyOf(commandStates.entrySet())) {
      final var command = entry.getKey();
      Continuation continuation = entry.getValue().continuation;

      if (!commandStates.containsKey(command)) {
        // Probably canceled by an owning composition, do not run
        continue;
      }

      if (!continuation.isDone()) {
        currentCommand = command;
        Logger.log("RUN", "Current command is " + command);
        Logger.log("RUN", "Mounting continuation for command " + command);
        mountContinuation(continuation);
        try {
          Logger.log("RUN", "Running continuation for command " + command + " until it yields");
          continuation.run();
        } finally {
          Logger.log("RUN", "Unmounting continuation for command " + command);
          mountContinuation(null);
        }
      } else {
        Logger.log("RUN", "Command " + command + " is done, not running");
      }
    }

    currentCommand = null;

    // Remove completed commands
    Logger.log("RUN", "Cleaning up after completed commands");
    commandStates.entrySet().removeIf(e -> e.getValue().continuation.isDone());

    // Schedule default commands for any resource currently without a command
    for (var resource : defaultCommands.keySet()) {
      Logger.log("RUN", "Checking if default command needs to run for " + resource + "...");
      var owner = commandStates.keySet().stream().filter(c -> c.requires(resource)).findAny();
      if (owner.isPresent()) {
        Logger.log("RUN", "No, already running " + owner.get());
      } else {
        Logger.log("RUN", "Yes, scheduling");
        scheduleDefault(resource);
      }
    }

    Logger.log("RUN", "Ending run #" + runNum);
  }

  private Continuation buildContinuation(AsyncCommand command) {
    return new Continuation(scope, () -> {
      try {
        command.run();
        Logger.log("CONTINUATION", "Command " + command + " completed naturally");
      } catch (Exception e) {
        throw new CommandExecutionException(command, e);
      }
    });
  }

  private void mountContinuation(Continuation continuation) {
    try {
      if (continuation == null) {
        java_lang_thread_setContinuation.invoke(Thread.currentThread(), null);
      } else {
        java_lang_thread_setContinuation.invoke(Thread.currentThread(), continuation.continuation);
      }
    } catch (Throwable t) {
      // Anything thrown internally by Thread.setContinuation.
      // It only assigns to a field, no way to throw
      // However, if the invocation fails for some reason, we'll end up with an
      // IllegalStateException when attempting to run an unmounted continuation
    }
  }

  /**
   * Checks if a command is currently running.
   *
   * @param command the command to check
   * @return true if the command is running, false if not
   */
  public boolean isRunning(AsyncCommand command) {
    return commandStates.containsKey(command);
  }

  public Set<AsyncCommand> getRunningCommands() {
    return Collections.unmodifiableSet(commandStates.keySet());
  }

  /**
   * Cancels all currently running commands, then starts the default commands for any resources
   * that had canceled commands. A default command that is currently running will not be canceled.
   */
  public void cancelAll() {
    commandStates.clear();
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
    return commandStates.keySet().stream().filter(c -> c.requires(resource)).findFirst().orElse(null);
  }

  public boolean yield() {
    return Continuation.yield(scope);
  }

  record CommandState(AsyncCommand command, Continuation continuation) { }

  public EventLoop getDefaultButtonLoop() {
    return eventLoop;
  }
}
