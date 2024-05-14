package edu.wpi.first.wpilibj3.command.async;

import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.event.EventLoop;
import java.lang.invoke.MethodHandle;
import java.lang.invoke.MethodHandles;
import java.lang.invoke.MethodType;
import java.util.ArrayList;
import java.util.Collection;
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
   * Represents the state of a command as it is executed by the scheduler.
   *
   * @param command The command being tracked.
   * @param parent The parent command composition that scheduled the tracked command. Null if the
   *               command was scheduled by a top level construct like trigger bindings or manually
   *               scheduled by a user. For deeply nested compositions, this tracks the <i>direct
   *               parent command</i> that invoked the schedule() call; in this manner, an ancestry
   *               tree can be built, where each {@code CommandState} object references a parent
   *               node in the tree.
   * @param continuation The continuation to which the command is bound.
   */
  private record CommandState(
      AsyncCommand command,
      AsyncCommand parent,
      Continuation continuation) { }

  /** The default commands for each resource. */
  private final Map<HardwareResource, AsyncCommand> defaultCommands = new HashMap<>();
  /** The set of commands scheduled since the start of the previous run. */
  private final Set<CommandState> onDeck = new LinkedHashSet<>();
  /** The states of all running commands (does not include on deck commands). */
  private final Map<AsyncCommand, CommandState> commandStates = new LinkedHashMap<>();
  /** The periodic callbacks to run, outside of the command structure. */
  private final List<Continuation> periodicCallbacks = new ArrayList<>();
  /** Event loop for trigger bindings. */
  private final EventLoop eventLoop = new EventLoop();

  /** The currently executing command. */
  private AsyncCommand currentCommand = null;

  /** The scope for continuations to yield to. */
  private final ContinuationScope scope = new ContinuationScope("async command");

  /** The default scheduler instance. */
  private static AsyncScheduler defaultScheduler = new AsyncScheduler();

  // package-private for tests
  static void setDefaultScheduler(AsyncScheduler defaultScheduler) {
    AsyncScheduler.defaultScheduler = defaultScheduler;
  }

  /**
   * Gets the default scheduler instance for use in a robot program. Some built in command types
   * use the default scheduler and will not work as expected if used on another scheduler instance.
   *
   * @return the default scheduler instance.
   */
  public static AsyncScheduler getInstance() {
    return defaultScheduler;
  }

  public AsyncScheduler() {
  }

  /**
   * Registers a resource with the scheduler and assigns it a default idle command.
   * @param resource the resource to schedule
   */
  public void registerResource(HardwareResource resource) {
    registerResource(resource, new IdleCommand(resource));
  }

  /**
   * Registers a resource with the scheduler and its default command.
   *
   * @param resource the resource to schedule
   * @param defaultCommand the default command to use for the resource
   */
  public void registerResource(HardwareResource resource, AsyncCommand defaultCommand) {
    setDefaultCommand(resource, defaultCommand);
  }

  /**
   * Sets the default command for a resource. The command must require the resource, and cannot
   * require any others. A default command will run if no other commands are scheduled to use its
   * resource.
   *
   * @param resource the resource for which to set the default command
   * @param defaultCommand the default command to execute on the resource
   * @throws IllegalArgumentException if the command does not require only the give resource
   */
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

  /**
   * Gets the registered default command for a given resource. If no default command has been
   * explicitly provided with {@link #setDefaultCommand(HardwareResource, AsyncCommand)}, then
   * an {@link IdleCommand} will be set as the default by the scheduler.
   *
   * @param resource the resource for which to retrieve the configured default command
   * @return the default command
   */
  public AsyncCommand getDefaultCommand(HardwareResource resource) {
    return defaultCommands.get(resource);
  }

  /**
   * Adds a callback to run as part of the scheduler. The callback should not manipulate or control
   * any resources, but can be used to log information, update data (such as simulations or LED
   * data buffers), or perform some other helpful task. The callback is responsible for managing its
   * own control flow and end conditions. If you want to run a single task periodically for the
   * entire lifespan of the scheduler, use {@link #addPeriodic(Runnable)}.
   *
   * <p><strong>Note:</strong> Like commands, any loops in the callback must appropriately yield
   * control back to the scheduler with {@link #yield()} or {@link AsyncCommand#yield()} or risk
   * stalling your program in an unrecoverable infinite loop!
   *
   * @param callback the callback to sideload
   */
  public void sideload(Runnable callback) {
    periodicCallbacks.add(new Continuation(scope, callback));
  }

  /**
   * Adds a task to run repeatedly for as long as the scheduler runs. This internally handles the
   * looping and control yielding necessary for proper function. The callback will run at the same
   * periodic frequency as the scheduler.
   *
   * <p>For example:</p>
   * <pre>{@code
   *   scheduler.addPeriodic(() -> leds.setData(ledDataBuffer));
   *   scheduler.addPeriodic(() -> {
   *     SmartDashboard.putNumber("X", getX());
   *     SmartDashboard.putNumber("Y", getY());
   *   });
   * }</pre>
   *
   * @param callback the periodic function to run
   */
  public void addPeriodic(Runnable callback) {
    sideload(() -> {
      while (true) {
        AsyncScheduler.this.yield();
        callback.run();
      }
    });
  }

  /**
   * Schedules a command to run. If a running command schedules another command (for example,
   * parallel groups will do this), then the new command is assumed to be a bound child of the
   * running command. The scheduling command is expected to have ownership over the scheduled
   * command, including cancelling it if the parent command completes before the child does
   * (for example, deadline groups will cancel unfinished child commands when all the required
   * commands in the group have completed).
   *
   * @param command the command to schedule
   */
  public void schedule(AsyncCommand command) {
    if (currentCommand == null) {
      // Scheduling from outside a command, eg a trigger binding or manual schedule call
      // Check for conflicts with the commands that are already running
      for (AsyncCommand c : commandStates.keySet()) {
        if (c.conflictsWith(command) && command.isLowerPriorityThan(c)) {
          return;
        }
      }
    } else {
      // Scheduled by a command.
      // Instead of checking for conflicts with all running commands (which would include the
      // parent), we instead need to verify that the scheduled command ONLY uses resources also
      // used by the parent. Technically, we should also verify that the scheduled command also
      // does not require any of the same resources as any sibling, either. But that can come later.
      if (!currentCommand.requirements().containsAll(command.requirements())) {
        var disallowedResources = new LinkedHashSet<>(command.requirements());
        disallowedResources.removeIf(currentCommand::requires);
        throw new IllegalStateException(
            "Nested commands can only require resources used by their parents. "
                + "Command "
                + command
                + " requires these resources that are not used by "
                + currentCommand
                + ": "
                + disallowedResources);
      }
    }

    for (var scheduledState : onDeck) {
      if (!command.conflictsWith(scheduledState.command())) {
        // No shared requirements, skip
        continue;
      }
      if (command.isLowerPriorityThan(scheduledState.command())) {
        // Lower priority than an already-scheduled (but not yet running) command that requires at
        // one of the same resource
        return;
      }
    }

    // Evict conflicting on-deck commands
    // We check above if the input command is lower priority than any of these,
    // so at this point we're guaranteed to be >= priority than anything already on deck
    onDeck.removeIf(c -> c.command().conflictsWith(command));

    var state = new CommandState(command, currentCommand, buildContinuation(command));
    onDeck.add(state);

    // Immediately evict any conflicting running command, except for commands in its ancestry
    for (var iterator = commandStates.entrySet().iterator(); iterator.hasNext(); ) {
      var entry = iterator.next();
      var cmd = entry.getKey();
      if (command.conflictsWith(cmd) && !inheritsFrom(state, cmd)) {
        // The scheduled command conflicts with and does not inherit from this command
        // Cancel this command and its children
        iterator.remove();
        removeOrphanedChildren(cmd);
      }
    }
  }

  /**
   * Checks if a particular command is an ancestor of another.
   *
   * @param state the state to check
   * @param ancestor the potential ancestor for which to search
   * @return true if {@code ancestor} is the direct parent or indirect ancestor, false if not
   */
  private boolean inheritsFrom(CommandState state, AsyncCommand ancestor) {
    if (state.parent() == null) {
      return false;
    }
    if (!commandStates.containsKey(ancestor)) {
      return false;
    }
    if (state.parent() == ancestor) {
      return true;
    }
    return commandStates.values().stream().anyMatch(s -> inheritsFrom(s, ancestor));
  }

  /**
   * Cancels a command and any other command scheduled by it. This occurs immediately and does not
   * need to wait for a call to {@link #run()}. Any command that it scheduled will also be canceled
   * to ensure commands within compositions do not continue to run.
   *
   * @param command the command to cancel
   */
  public void cancel(AsyncCommand command) {
    // Evict the command. The next call to run() will schedule the default command for all its
    // required resources, unless another command requiring those resources is scheduled between
    // calling cancel() and calling run()
    commandStates.remove(command);

    // Clean up any orphaned child commands; their lifespan must not exceed the parent's
    removeOrphanedChildren(command);
  }

  /**
   * Updates the command scheduler. This will process trigger bindings on anything attached to the
   * {@link #getDefaultButtonLoop() default event loop}, begin running any commands scheduled since
   * the previous call to {@code run()}, process periodic callbacks added with
   * {@link #addPeriodic(Runnable)} and {@link #sideload(Runnable)}, update running commands, and
   * schedule default commands for any resources that are not owned by a running command.
   *
   * <p>This method is intended to be called in a periodic loop like
   * {@link TimedRobot#robotPeriodic()}</p>
   */
  public void run() {
    // Process triggers first; these tend to queue and cancel commands
    eventLoop.poll();

    // Move any scheduled commands to the running set
    for (var queuedState : onDeck) {
      commandStates.put(queuedState.command, queuedState);
    }

    // Clear the set of on-deck commands,
    // since we just put them all into the set of running commands
    onDeck.clear();

    // Update periodic callbacks
    for (Continuation callback : periodicCallbacks) {
      callback.run();
    }

    // And remove any periodic callbacks that have completed
    periodicCallbacks.removeIf(Continuation::isDone);

    // Tick every command that hasn't been completed yet
    for (var entry : List.copyOf(commandStates.entrySet())) {
      final var command = entry.getKey();
      Continuation continuation = entry.getValue().continuation;

      if (!commandStates.containsKey(command)) {
        // Probably canceled by an owning composition, do not run
        continue;
      }

      currentCommand = command;
      mountContinuation(continuation);
      try {
        continuation.run();
      } finally {
        currentCommand = null;
        mountContinuation(null);
      }

      if (continuation.isDone()) {
        // Immediately check if the command has completed and remove any children commands.
        // This prevents child commands from being executed one extra time in the run() loop
        commandStates.remove(command);
        removeOrphanedChildren(command);
      }
    }

    // Remove completed commands
    commandStates.entrySet().removeIf(e -> e.getValue().continuation.isDone());

    // Schedule default commands for any resource currently without a command
    for (var resource : defaultCommands.keySet()) {
      var unowned = commandStates.keySet().stream().noneMatch(c -> c.requires(resource));
      if (unowned) {
        scheduleDefault(resource);
      }
    }
  }

  /**
   * Removes all commands descended from a parent command. This is used to ensure that any command
   * scheduled within a composition or group cannot live longer than any ancestor.
   *
   * @param parent the root command whose descendants to remove from the scheduler
   */
  private void removeOrphanedChildren(AsyncCommand parent) {
    for (var iterator = commandStates.values().iterator(); iterator.hasNext(); ) {
      var state = iterator.next();
      if (state.parent == parent) {
        iterator.remove();
        removeOrphanedChildren(state.command);
      }
    }
  }

  /**
   * Builds a continuation object that the command will be bound to. The continuation will be scoped
   * to this scheduler object and cannot be used by another scheduler instance.
   *
   * @param command the command for which to build a continuation
   * @return the binding continuation
   */
  private Continuation buildContinuation(AsyncCommand command) {
    return new Continuation(scope, () -> {
      try {
        command.run();
      } catch (Exception e) {
        throw new CommandExecutionException(command, e);
      }
    });
  }

  /**
   * Mounds a continuation to the current thread. Accepts null for clearing the currently mounted
   * continuation.
   *
   * @param continuation the continuation to mount
   */
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

  /**
   * Checks if a command is currently scheduled to run, but is not yet running.
   *
   * @param command the command to check
   * @return true if the command is scheduled to run, false if not
   */
  public boolean isScheduled(AsyncCommand command) {
    return onDeck.stream().anyMatch(state -> state.command == command);
  }

  /**
   * Checks if a command is currently scheduled to run, or is already running.
   *
   * @param command the command to check
   * @return true if the command is scheduled to run or is already running, false if not
   */
  public boolean isScheduledOrRunning(AsyncCommand command) {
    return isScheduled(command) || isRunning(command);
  }

  /**
   * When called from a command group or composition, this method will keep that group paused until
   * the specified command completes.
   *
   * @param command the command to wait for
   */
  public void waitFor(AsyncCommand command) {
    checkWaitable(command);

    while (isScheduledOrRunning(command)) {
      this.yield();
    }
  }

  /**
   * When called from a command group or composition, this method will keep that group paused until
   * every specified command completes.
   *
   * @param commands the commands to wait for
   */
  public void waitForAll(Collection<AsyncCommand> commands) {
    for (var command : commands) {
      checkWaitable(command);
    }

    // Wait as long as at least one command is still running
    while (commands.stream().anyMatch(this::isScheduledOrRunning)) {
      this.yield();
    }
  }

  /**
   * When called from a command group or composition, this method will keep that group paused until
   * any of the specified commands completes.
   *
   * @param commands the commands to wait for
   */
  public void waitForAny(Collection<AsyncCommand> commands) {
    for (var command : commands) {
      checkWaitable(command);
    }

    // Wait as long as all commands are still running
    while (commands.stream().allMatch(this::isScheduledOrRunning)) {
      this.yield();
    }
  }

  /**
   * Checks if the currently running command is permitted to wait for completion of another command.
   * @param command the command to check
   */
  private void checkWaitable(AsyncCommand command) {
    if (!isScheduledOrRunning(command)) {
      throw new IllegalStateException(
          "Cannot wait for command " + command + " because it is not currently running");
    }

    var state =
        commandStates.getOrDefault(
            command,
            onDeck.stream().filter(s -> s.command() == command).findFirst().orElseThrow());

    if (state.parent() != null && state.parent() != currentCommand) {
      throw new IllegalStateException(
          "Only " + state.parent() + " can wait for " + command);
    }
  }

  /**
   * Gets the set of all currently running commands. Commands are returned in the order in which
   * they were scheduled. The returned set is read-only.
   *
   * @return the currently running commands
   */
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
   * Called by {@link AsyncCommand#run()}, this will cause the command's execution to pause and
   * cede control back to the scheduler.
   *
   * @throws IllegalStateException if called outside a command that is currently being executed by
   * the scheduler
   */
  public void yield() {
    Continuation.yield(scope);
  }

  public EventLoop getDefaultButtonLoop() {
    return eventLoop;
  }
}
