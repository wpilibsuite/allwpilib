package edu.wpi.first.wpilibj3.command.async;

import static edu.wpi.first.wpilibj3.command.async.AsyncCommand.InterruptBehavior.SuspendOnInterrupt;

import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.event.EventLoop;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.PriorityQueue;
import java.util.Set;
import java.util.function.Consumer;

public class AsyncScheduler {
  private final Set<RequireableResource> registeredResources = new HashSet<>();

  /** The set of commands scheduled since the start of the previous run. */
  private final Set<CommandState> onDeck = new LinkedHashSet<>();

  /** The states of all running commands (does not include on deck commands). */
  private final Map<AsyncCommand, CommandState> commandStates = new LinkedHashMap<>();

  /**
   * A priority queue of suspended commands. Suspended commands will be resumed at the end of each
   * run() call if all their requirements are available, and are attempted to be resumed based on
   * their priority values (higher priority = earlier attempt).
   */
  private final PriorityQueue<CommandState> suspendedStates =
      new PriorityQueue<>(Comparator.comparingInt(s -> s.command().priority()));

  /** The periodic callbacks to run, outside of the command structure. */
  private final List<Continuation> periodicCallbacks = new ArrayList<>();

  /** Event loop for trigger bindings. */
  private final EventLoop eventLoop = new EventLoop();

  /** The currently executing command. */
  private AsyncCommand currentCommand = null;

  /** The scope for continuations to yield to. */
  private final ContinuationScope scope = new ContinuationScope("async command");

  /** The default scheduler instance. */
  private static final AsyncScheduler defaultScheduler = new AsyncScheduler();

  private final Coroutine coroutine = () -> AsyncScheduler.this;

  /**
   * Gets the default scheduler instance for use in a robot program. Some built in command types use
   * the default scheduler and will not work as expected if used on another scheduler instance.
   *
   * @return the default scheduler instance.
   */
  public static AsyncScheduler getInstance() {
    return defaultScheduler;
  }

  public AsyncScheduler() {}

  /**
   * Registers a resource with the scheduler and schedules its default command.
   *
   * @param resource the resource to register
   */
  public void registerResource(RequireableResource resource) {
    registeredResources.add(resource);
    if (resource.getDefaultCommand() instanceof AsyncCommand defaultCommand) {
      scheduleAsDefaultCommand(resource, defaultCommand);
    }
  }

  /**
   * Sets the default command for a resource. The command must require the resource, and cannot
   * require any others. Default commands must {@link
   * AsyncCommand.InterruptBehavior#SuspendOnInterrupt suspend on interrupt} and have a lower
   * priority than {@link AsyncCommand#DEFAULT_PRIORITY} to function properly.
   *
   * @param resource the resource for which to set the default command
   * @param defaultCommand the default command to execute on the resource
   * @throws IllegalArgumentException if the command does not meet the requirements for being a
   *     default command
   */
  public void scheduleAsDefaultCommand(RequireableResource resource, AsyncCommand defaultCommand) {
    if (!defaultCommand.requires(resource)) {
      throw new IllegalArgumentException("A resource's default command must require that resource");
    }

    if (defaultCommand.requirements().size() > 1) {
      throw new IllegalArgumentException(
          "A resource's default command cannot require other resources");
    }

    if (defaultCommand.interruptBehavior() != SuspendOnInterrupt) {
      throw new IllegalArgumentException(
          "Default commands must have interrupt behavior set to SuspendOnInterrupt");
    }

    if (defaultCommand.priority() >= AsyncCommand.DEFAULT_PRIORITY) {
      throw new IllegalArgumentException("Default commands must be low priority");
    }

    schedule(defaultCommand);
  }

  /**
   * Adds a callback to run as part of the scheduler. The callback should not manipulate or control
   * any resources, but can be used to log information, update data (such as simulations or LED data
   * buffers), or perform some other helpful task. The callback is responsible for managing its own
   * control flow and end conditions. If you want to run a single task periodically for the entire
   * lifespan of the scheduler, use {@link #addPeriodic(Runnable)}.
   *
   * <p><strong>Note:</strong> Like commands, any loops in the callback must appropriately yield
   * control back to the scheduler with {@link Coroutine#yield} or risk stalling your program in an
   * unrecoverable infinite loop!
   *
   * @param callback the callback to sideload
   */
  public void sideload(Consumer<Coroutine> callback) {
    periodicCallbacks.add(
        new Continuation(
            scope,
            () -> {
              callback.accept(coroutine);
            }));
  }

  /**
   * Adds a task to run repeatedly for as long as the scheduler runs. This internally handles the
   * looping and control yielding necessary for proper function. The callback will run at the same
   * periodic frequency as the scheduler.
   *
   * <p>For example:
   *
   * <pre>{@code
   * scheduler.addPeriodic(() -> leds.setData(ledDataBuffer));
   * scheduler.addPeriodic(() -> {
   *   SmartDashboard.putNumber("X", getX());
   *   SmartDashboard.putNumber("Y", getY());
   * });
   * }</pre>
   *
   * @param callback the periodic function to run
   */
  public void addPeriodic(Runnable callback) {
    sideload(
        (coroutine) -> {
          while (coroutine.yield()) {
            callback.run();
          }
        });
  }

  /**
   * Schedules a command to run. If a running command schedules another command (for example,
   * parallel groups will do this), then the new command is assumed to be a bound child of the
   * running command. The scheduling command is expected to have ownership over the scheduled
   * command, including cancelling it if the parent command completes before the child does (for
   * example, deadline groups will cancel unfinished child commands when all the required commands
   * in the group have completed).
   *
   * @param command the command to schedule
   */
  public void schedule(AsyncCommand command) {
    if (!isSchedulable(command)) {
      return;
    }

    if (currentCommand != null) {
      // Scheduled by a command.
      // Permit disjoint requirement sets iff every requirement of the scheduled command is a nested
      // resource under a single requirement of the parent command
      if (currentCommand.requirements().stream()
          .anyMatch(parentReq -> parentReq.nestedResources().containsAll(command.requirements()))) {
        // Allow
      } else {
        // Instead of checking for conflicts with all running commands (which would include the
        // parent), we instead need to verify that the scheduled command ONLY uses resources also
        // used by the parent. Technically, we should also verify that the scheduled command also
        // does not require any of the same resources as any sibling, either. But that can come
        // later.
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
    }

    for (var scheduledState : onDeck) {
      if (!command.conflictsWith(scheduledState.command())) {
        // No shared requirements, skip
        continue;
      }
      if (command.isLowerPriorityThan(scheduledState.command())) {
        // Lower priority than an already-scheduled (but not yet running) command that requires at
        // one of the same resource
        if (command.interruptBehavior() == SuspendOnInterrupt) {
          suspend(new CommandState(command, currentCommand, buildContinuation(command)));
        }
        return;
      }
    }

    // Evict conflicting on-deck commands
    // We check above if the input command is lower priority than any of these,
    // so at this point we're guaranteed to be >= priority than anything already on deck
    evictConflictingOnDeckCommands(command);

    var state = new CommandState(command, currentCommand, buildContinuation(command));
    onDeck.add(state);

    // Immediately evict any conflicting running command, except for commands in its ancestry
    evictConflictingRunningCommands(state);
  }

  private boolean isSchedulable(AsyncCommand command) {
    if (currentCommand != null) {
      // Bypass scheduling check if being scheduled as a nested command.
      // The schedule() method will throw an error when attempting to schedule a nested command
      // that requires a resource that the parent doesn't
      return true;
    }

    // Scheduling from outside a command, eg a trigger binding or manual schedule call
    // Check for conflicts with the commands that are already running
    for (AsyncCommand c : commandStates.keySet()) {
      if (c.conflictsWith(command) && command.isLowerPriorityThan(c)) {
        return false;
      }
    }

    return true;
  }

  private void evictConflictingOnDeckCommands(AsyncCommand command) {
    for (var iterator = onDeck.iterator(); iterator.hasNext(); ) {
      var scheduledState = iterator.next();
      var scheduledCommand = scheduledState.command();
      if (scheduledCommand.conflictsWith(command)) {
        // Remove the lower priority conflicting command from the on deck commands.
        // We don't need to call removeOrphanedChildren here because it hasn't started yet,
        // meaning it hasn't had a chance to schedule any children
        iterator.remove();
        if (scheduledCommand.interruptBehavior() == SuspendOnInterrupt) {
          suspend(scheduledState);
        }
      }
    }
  }

  private void evictConflictingRunningCommands(CommandState state) {
    for (var iterator = commandStates.entrySet().iterator(); iterator.hasNext(); ) {
      var entry = iterator.next();
      var cmd = entry.getKey();
      if (state.command().conflictsWith(cmd) && !inheritsFrom(state, cmd)) {
        // The scheduled command conflicts with and does not inherit from this command
        // Cancel this command and its children
        iterator.remove();
        removeOrphanedChildren(cmd);
        if (cmd.interruptBehavior() == SuspendOnInterrupt) {
          suspend(entry.getValue());
        }
      }
    }
  }

  /**
   * Schedules and waits for a command to complete. Shorthand for {@code schedule(command);
   * waitFor(command);}.
   *
   * @param command the command to schedule
   */
  public void scheduleAndWait(AsyncCommand command) {
    schedule(command);
    await(command);
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
    onDeck.removeIf(state -> state.command() == command);
    suspendedStates.removeIf(state -> state.command() == command);

    // Clean up any orphaned child commands; their lifespan must not exceed the parent's
    removeOrphanedChildren(command);
  }

  private void suspend(CommandState state) {
    suspendedStates.add(state);
  }

  /**
   * Updates the command scheduler. This will process trigger bindings on anything attached to the
   * {@link #getDefaultButtonLoop() default event loop}, begin running any commands scheduled since
   * the previous call to {@code run()}, process periodic callbacks added with {@link
   * #addPeriodic(Runnable)} and {@link #sideload(Consumer)}, update running commands, and schedule
   * default commands for any resources that are not owned by a running command.
   *
   * <p>This method is intended to be called in a periodic loop like {@link
   * TimedRobot#robotPeriodic()}
   */
  public void run() {
    // Process triggers first; these tend to queue and cancel commands
    eventLoop.poll();

    promoteScheduledCommands();
    runPeriodicSideloads();
    runCommands();
    resumeSuspendedCommands();
    scheduleDefaultCommands();
  }

  private void runPeriodicSideloads() {
    // Update periodic callbacks
    for (Continuation callback : periodicCallbacks) {
      callback.run();
    }

    // And remove any periodic callbacks that have completed
    periodicCallbacks.removeIf(Continuation::isDone);
  }

  private void promoteScheduledCommands() {
    // Move any scheduled commands to the running set
    for (var queuedState : onDeck) {
      commandStates.put(queuedState.command(), queuedState);
    }

    // Clear the set of on-deck commands,
    // since we just put them all into the set of running commands
    onDeck.clear();
  }

  private void runCommands() {
    // Tick every command that hasn't been completed yet
    for (var entry : List.copyOf(commandStates.entrySet())) {
      final var command = entry.getKey();
      var continuation = entry.getValue().continuation();

      if (!commandStates.containsKey(command)) {
        // Probably canceled by an owning composition, do not run
        continue;
      }

      currentCommand = command;
      Continuation.mountContinuation(continuation);
      try {
        continuation.run();
      } finally {
        currentCommand = null;
        Continuation.mountContinuation(null);
      }

      if (continuation.isDone()) {
        // Immediately check if the command has completed and remove any children commands.
        // This prevents child commands from being executed one extra time in the run() loop
        commandStates.remove(command);
        removeOrphanedChildren(command);
      }
    }
  }

  private void resumeSuspendedCommands() {
    // Attempt to schedule as many suspended commands as possible
    // Work on a copy since we'll be popping elements off the queue
    for (var suspendedState : List.copyOf(suspendedStates).reversed()) {
      if (isSchedulable(suspendedState.command())) {
        schedule(suspendedState.command());
        suspendedStates.remove(suspendedState);
      }
      // TODO: Early exit once every resource has a scheduled or running command
    }
  }

  private void scheduleDefaultCommands() {
    // Schedule the default commands for every resource that doesn't currently have a running or
    // scheduled command.
    for (var resource : registeredResources) {
      if (commandStates.keySet().stream().noneMatch(c -> c.requires(resource))
          && onDeck.stream().noneMatch(c -> c.command().requires(resource))) {
        // Nothing currently running or scheduled
        // Schedule the resource's default command, if it has one
        if (resource.getDefaultCommand() instanceof AsyncCommand defaultCommand) {
          schedule(defaultCommand);
        }
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
      if (state.parent() == parent) {
        iterator.remove();
        removeOrphanedChildren(state.command());
        suspendedStates.remove(state);
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
    return new Continuation(
        scope,
        () -> {
          try {
            command.run(coroutine);
          } catch (Exception e) {
            throw new CommandExecutionException(command, e);
          }
        });
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
    return onDeck.stream().anyMatch(state -> state.command() == command);
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
  public void await(AsyncCommand command) {
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
  public void awaitAll(Collection<AsyncCommand> commands) {
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
  public void awaitAny(Collection<AsyncCommand> commands) {
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
   *
   * @param command the command to check
   */
  private void checkWaitable(AsyncCommand command) {
    if (!isScheduledOrRunning(command)) {
      throw new IllegalStateException(
          "Cannot wait for command " + command + " because it is not currently running");
    }

    var state =
        commandStates.getOrDefault(
            command, onDeck.stream().filter(s -> s.command() == command).findFirst().orElseThrow());

    if (state.parent() != null && state.parent() != currentCommand) {
      throw new IllegalStateException("Only " + state.parent() + " can wait for " + command);
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
   * Gets all the currently running commands that require a particular resource. Commands are
   * returned in the order in which they were scheduled. The returned list is read-only.
   *
   * @param resource the resource to get the commands for
   * @return the currently running commands that require the resource.
   */
  public List<AsyncCommand> getRunningCommandsFor(RequireableResource resource) {
    return commandStates.keySet().stream().filter(command -> command.requires(resource)).toList();
  }

  /**
   * Cancels all currently running commands, then starts the default commands for any resources that
   * had canceled commands. A default command that is currently running will not be canceled.
   */
  public void cancelAll() {
    commandStates.clear();
  }

  /**
   * Called by {@link AsyncCommand#run(Coroutine)}, this will cause the command's execution to pause
   * and cede control back to the scheduler.
   *
   * @return true
   * @throws IllegalStateException if called outside a command that is currently being executed by
   *     the scheduler
   */
  public boolean yield() {
    return Continuation.yield(scope);
  }

  public EventLoop getDefaultButtonLoop() {
    return eventLoop;
  }
}
