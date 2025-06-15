// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static edu.wpi.first.units.Units.Microseconds;
import static edu.wpi.first.units.Units.Milliseconds;

import edu.wpi.first.util.ErrorMessages;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.event.EventLoop;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Stack;
import java.util.function.Consumer;
import java.util.stream.Collectors;
import org.wpilib.commands3.proto.SchedulerProto;

/** Manages the lifecycles of {@link Coroutine}-based {@link Command Commands}. */
public class Scheduler implements ProtobufSerializable {
  private final Map<RequireableResource, Command> defaultCommands = new LinkedHashMap<>();

  /** The set of commands scheduled since the start of the previous run. */
  private final Set<CommandState> onDeck = new LinkedHashSet<>();

  /** The states of all running commands (does not include on deck commands). */
  private final Map<Command, CommandState> commandStates = new LinkedHashMap<>();

  /**
   * The stack of currently executing commands. Child commands are pushed onto the stack and popped
   * when they complete. Use {@link #currentState()} and {@link #currentCommand()} to get the
   * currently executing command or its state.
   */
  private final Stack<CommandState> executingCommands = new Stack<>();

  /** The periodic callbacks to run, outside of the command structure. */
  private final List<Coroutine> periodicCallbacks = new ArrayList<>();

  /** Event loop for trigger bindings. */
  private final EventLoop eventLoop = new EventLoop();

  /** The scope for continuations to yield to. */
  private final ContinuationScope scope = new ContinuationScope("coroutine commands");

  // Telemetry
  public static final SchedulerProto proto = new SchedulerProto();
  private double lastRunTimeMs = -1;

  private final Set<Consumer<? super SchedulerEvent>> eventListeners = new LinkedHashSet<>();

  /** The default scheduler instance. */
  private static final Scheduler defaultScheduler = new Scheduler();


  /**
   * Gets the default scheduler instance for use in a robot program. Some built in command types use
   * the default scheduler and will not work as expected if used on another scheduler instance.
   *
   * @return the default scheduler instance.
   */
  public static Scheduler getDefault() {
    return defaultScheduler;
  }

  public Scheduler() {}

  /**
   * Sets the default command for a resource. The command must require that resource, and cannot
   * require any other resources.
   *
   * @param resource the resource for which to set the default command
   * @param defaultCommand the default command to execute on the resource
   * @throws IllegalArgumentException if the command does not meet the requirements for being a
   *     default command
   */
  public void scheduleAsDefaultCommand(RequireableResource resource, Command defaultCommand) {
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
   * Gets the default command set for a resource.
   *
   * @param resource The resource
   * @return The default command, or null if no default command was ever set
   */
  public Command getDefaultCommandFor(RequireableResource resource) {
    return defaultCommands.get(resource);
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
    var coroutine = new Coroutine(this, scope, callback);
    periodicCallbacks.add(coroutine);
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
    // TODO: Add a unit test for this
    sideload(
        (coroutine) -> {
          while (coroutine.yield()) {
            callback.run();
          }
        });
  }

  public enum ScheduleResult {
    /** The command was successfully scheduled and added to the queue. */
    SUCCESS,
    /** The command is already scheduled or running. */
    ALREADY_RUNNING,
    /** The command is a lower priority and conflicts with a command that's already running. */
    LOWER_PRIORITY_THAN_RUNNING_COMMAND,
  }

  /**
   * Schedules a command to run. If a running command schedules another command (for example,
   * parallel groups will do this), then the new command is assumed to be a bound child of the
   * running command. Child commands will automatically be cancelled by the scheduler when their
   * parent command completes or is canceled. Child commands will also immediately begin execution,
   * without needing to wait for the next {@link #run()} invocation. This allows highly nested
   * compositions to begin running the actual meaningful commands sooner without needing to wait one
   * scheduler cycle per nesting level; with the default 20ms update period, 5 levels of nesting
   * would be enough to delay actions by 100 milliseconds - instead of only 20.
   *
   * <p>Does nothing if the command is already scheduled or running, or requires at least one
   * resource already used by a higher priority command.
   *
   * <p>If one command schedules another ("parent" and "fork"), the forked command will be canceled
   * when the parent command completes. It is not possible to fork a command and have it live longer
   * than the command that forked it.
   *
   * @param command the command to schedule
   * @throws IllegalArgumentException if scheduled by a command composition that has already
   *     scheduled another command that shares at least one required resource
   */
  public ScheduleResult schedule(Command command) {
    var binding = new Binding(
        BindingScope.global(),
        BindingType.IMMEDIATE,
        command,
        new Throwable().getStackTrace()
    );

    return schedule(binding);
  }

  // Package-private for use by Trigger
  ScheduleResult schedule(Binding binding) {
    var command = binding.command();

    if (isScheduledOrRunning(command)) {
      return ScheduleResult.ALREADY_RUNNING;
    }

    if (!isSchedulable(command)) {
      return ScheduleResult.LOWER_PRIORITY_THAN_RUNNING_COMMAND;
    }

    for (var scheduledState : onDeck) {
      if (!command.conflictsWith(scheduledState.command())) {
        // No shared requirements, skip
        continue;
      }
      if (command.isLowerPriorityThan(scheduledState.command())) {
        // Lower priority than an already-scheduled (but not yet running) command that requires at
        // one of the same resource. Ignore it.
        return ScheduleResult.LOWER_PRIORITY_THAN_RUNNING_COMMAND;
      }
    }

    // Evict conflicting on-deck commands
    // We check above if the input command is lower priority than any of these,
    // so at this point we're guaranteed to be >= priority than anything already on deck
    evictConflictingOnDeckCommands(command);

    var state = new CommandState(
        command,
        currentCommand(),
        buildCoroutine(command),
        binding);

    emitEvent(SchedulerEvent.scheduled(command));

    if (currentState() != null) {
      // Scheduling a child command while running. Start it immediately instead of waiting a full
      // cycle. This prevents issues with deeply nested command groups taking many scheduler cycles
      // to start running the commands that actually /do/ things
      evictConflictingRunningCommands(state);
      commandStates.put(command, state);
      runCommand(state);
    } else {
      // Scheduling outside a command, add it to the pending set. If it's not overridden by another
      // conflicting command being scheduled in the same scheduler loop, it'll be promoted and
      // start to run when #runCommands() is called
      onDeck.add(state);
    }

    return ScheduleResult.SUCCESS;
  }

  private boolean isSchedulable(Command command) {
    if (currentState() != null) {
      // Bypass scheduling check if being scheduled as a nested command.
      // The schedule() method will throw an error when attempting to schedule a nested command
      // that requires a resource that the parent doesn't
      return true;
    }

    // Scheduling from outside a command, eg a trigger binding or manual schedule call
    // Check for conflicts with the commands that are already running
    for (Command c : commandStates.keySet()) {
      if (c.conflictsWith(command) && command.isLowerPriorityThan(c)) {
        return false;
      }
    }

    return true;
  }

  private void evictConflictingOnDeckCommands(Command command) {
    for (var iterator = onDeck.iterator(); iterator.hasNext(); ) {
      var scheduledState = iterator.next();
      var scheduledCommand = scheduledState.command();
      if (scheduledCommand.conflictsWith(command)) {
        // Remove the lower priority conflicting command from the on deck commands.
        // We don't need to call removeOrphanedChildren here because it hasn't started yet,
        // meaning it hasn't had a chance to schedule any children
        iterator.remove();
        emitEvent(SchedulerEvent.interrupted(scheduledCommand, command));
      }
    }
  }

  /**
   * Cancels all running commands with which an incoming state conflicts. Ancestor commands of the
   * incoming state will not be canceled.
   */
  private void evictConflictingRunningCommands(CommandState incomingState) {
    // The set of root states with which the incoming state conflicts but does not inherit from
    Set<CommandState> conflictingRootStates =
        commandStates.values().stream()
            .filter(state -> incomingState.command().conflictsWith(state.command()))
            .filter(state -> !inheritsFrom(incomingState, state.command()))
            .map(
                state -> {
                  // Find the highest level ancestor of the conflicting command from which the
                  // incoming state does _not_ inherit. If they're totally unrelated, this will
                  // get the very root ancestor; otherwise, it'll return a direct sibling of the
                  // incoming command
                  CommandState root = state;
                  while (root.parent() != null && root.parent() != incomingState.parent()) {
                    root = commandStates.get(root.parent());
                  }
                  return root;
                })
            .collect(Collectors.toSet());

    // Cancel the root commands
    for (var conflictingState : conflictingRootStates) {
      emitEvent(SchedulerEvent.interrupted(conflictingState.command(), incomingState.command()));
      cancel(conflictingState.command());
    }
  }

  /**
   * Checks if a particular command is an ancestor of another.
   *
   * @param state the state to check
   * @param ancestor the potential ancestor for which to search
   * @return true if {@code ancestor} is the direct parent or indirect ancestor, false if not
   */
  private boolean inheritsFrom(CommandState state, Command ancestor) {
    if (state.parent() == null) {
      // No parent, cannot inherit
      return false;
    }
    if (!commandStates.containsKey(ancestor)) {
      // The given ancestor isn't running
      return false;
    }
    if (state.parent() == ancestor) {
      // Direct child
      return true;
    }
    // Check if the command's parent inherits from the given ancestor
    return commandStates.values().stream()
        .filter(s -> state.parent() == s.command())
        .anyMatch(s -> inheritsFrom(s, ancestor));
  }

  /**
   * Cancels a command and any other command scheduled by it. This occurs immediately and does not
   * need to wait for a call to {@link #run()}. Any command that it scheduled will also be canceled
   * to ensure commands within compositions do not continue to run.
   *
   * <p>This has no effect if the given command is not currently scheduled or running.
   *
   * @param command the command to cancel
   */
  public void cancel(Command command) {
    boolean running = isRunning(command);

    // Evict the command. The next call to run() will schedule the default command for all its
    // required resources, unless another command requiring those resources is scheduled between
    // calling cancel() and calling run()
    commandStates.remove(command);
    onDeck.removeIf(state -> state.command() == command);

    if (running) {
      // Only run the hook if the command was running. If it was on deck or not
      // even in the scheduler at the time, then there's nothing to do
      command.onCancel();
      emitEvent(SchedulerEvent.evicted(command));
    }

    // Clean up any orphaned child commands; their lifespan must not exceed the parent's
    removeOrphanedChildren(command);
  }

  /**
   * Updates the command scheduler. This will process trigger bindings on anything attached to the
   * {@link #getDefaultEventLoop() default event loop}, begin running any commands scheduled since
   * the previous call to {@code run()}, process periodic callbacks added with {@link
   * #addPeriodic(Runnable)} and {@link #sideload(Consumer)}, update running commands, and schedule
   * default commands for any resources that are not owned by a running command.
   *
   * <p>This method is intended to be called in a periodic loop like {@link
   * TimedRobot#robotPeriodic()}
   */
  public void run() {
    long startMicros = RobotController.getTime();

    // Process triggers first; these tend to queue and cancel commands
    eventLoop.poll();

    runPeriodicSideloads();
    promoteScheduledCommands();
    runCommands();
    scheduleDefaultCommands();

    long endMicros = RobotController.getTime();
    lastRunTimeMs = Milliseconds.convertFrom(endMicros - startMicros, Microseconds);
  }

  private void promoteScheduledCommands() {
    // Clear any commands that conflict with the scheduled set
    for (var queuedState : onDeck) {
      evictConflictingRunningCommands(queuedState);
    }

    // Move any scheduled commands to the running set
    for (var queuedState : onDeck) {
      commandStates.put(queuedState.command(), queuedState);
    }

    // Clear the set of on-deck commands,
    // since we just put them all into the set of running commands
    onDeck.clear();
  }

  private void runPeriodicSideloads() {
    // Update periodic callbacks
    for (Coroutine coroutine : periodicCallbacks) {
      coroutine.mount();
      try {
        coroutine.runToYieldPoint();
      } finally {
        Continuation.mountContinuation(null);
      }
    }

    // And remove any periodic callbacks that have completed
    periodicCallbacks.removeIf(Coroutine::isDone);
  }

  private void runCommands() {
    // Tick every command that hasn't been completed yet
    for (var state : List.copyOf(commandStates.values())) {
      runCommand(state);
    }
  }

  private void runCommand(CommandState state) {
    final var command = state.command();
    var coroutine = state.coroutine();

    if (!commandStates.containsKey(command)) {
      // Probably canceled by an owning composition, do not run
      return;
    }

    var previousState = currentState();

    executingCommands.push(state);
    long startMicros = RobotController.getTime();
    emitEvent(SchedulerEvent.mounted(command));
    coroutine.mount();
    try {
      coroutine.runToYieldPoint();
    } catch (RuntimeException e) {
      // Intercept the exception, inject stack frames from the schedule site, and rethrow it
      var binding = state.binding();
      e.setStackTrace(CommandTraceHelper.modifyTrace(e.getStackTrace(), binding.frames()));
      emitEvent(SchedulerEvent.completedWithError(command, e));
      throw e;
    } finally {
      long endMicros = RobotController.getTime();
      double elapsedMs = Milliseconds.convertFrom(endMicros - startMicros, Microseconds);
      state.setLastRuntimeMs(elapsedMs);

      if (currentState() == state) {
        // Remove the command we just ran from the top of the stack
        executingCommands.pop();
      }

      if (previousState != null) {
        // Remount the parent command, if there is one
        previousState.coroutine().mount();
      } else {
        Continuation.mountContinuation(null);
      }
    }

    if (coroutine.isDone()) {
      // Immediately check if the command has completed and remove any children commands.
      // This prevents child commands from being executed one extra time in the run() loop
      emitEvent(SchedulerEvent.completed(command));
      commandStates.remove(command);
      removeOrphanedChildren(command);
    } else {
      // Yielded
      emitEvent(SchedulerEvent.yielded(command));
    }
  }

  /**
   * Gets the currently executing command state, or null if no command is currently executing.
   *
   * @return the currently executing command state
   */
  private CommandState currentState() {
    if (executingCommands.isEmpty()) {
      // Avoid EmptyStackException
      return null;
    }

    return executingCommands.peek();
  }

  /**
   * Gets the currently executing command, or null if no command is currently executing.
   *
   * @return the currently executing command
   */
  public Command currentCommand() {
    var state = currentState();
    if (state == null) {
      return null;
    }

    return state.command();
  }

  private void scheduleDefaultCommands() {
    // Schedule the default commands for every resource that doesn't currently have a running or
    // scheduled command.
    defaultCommands.forEach(
        (resource, defaultCommand) -> {
          if (commandStates.keySet().stream().noneMatch(c -> c.requires(resource))
              && onDeck.stream().noneMatch(c -> c.command().requires(resource))) {
            // Nothing currently running or scheduled
            // Schedule the resource's default command, if it has one
            if (defaultCommand != null) {
              schedule(defaultCommand);
            }
          }
        });
  }

  /**
   * Removes all commands descended from a parent command. This is used to ensure that any command
   * scheduled within a composition or group cannot live longer than any ancestor.
   *
   * @param parent the root command whose descendants to remove from the scheduler
   */
  private void removeOrphanedChildren(Command parent) {
    commandStates.entrySet().stream()
        .filter(e -> e.getValue().parent() == parent)
        .toList() // copy to an intermediate list to avoid concurrent modification
        .forEach(e -> cancel(e.getKey()));
  }

  /**
   * Builds a coroutine object that the command will be bound to. The coroutine will be scoped to
   * this scheduler object and cannot be used by another scheduler instance.
   *
   * @param command the command for which to build a coroutine
   * @return the binding coroutine
   */
  private Coroutine buildCoroutine(Command command) {
    return new Coroutine(this, scope, command::run);
  }

  /**
   * Checks if a command is currently running.
   *
   * @param command the command to check
   * @return true if the command is running, false if not
   */
  public boolean isRunning(Command command) {
    return commandStates.containsKey(command);
  }

  /**
   * Checks if a command is currently scheduled to run, but is not yet running.
   *
   * @param command the command to check
   * @return true if the command is scheduled to run, false if not
   */
  public boolean isScheduled(Command command) {
    return onDeck.stream().anyMatch(state -> state.command() == command);
  }

  /**
   * Checks if a command is currently scheduled to run, or is already running.
   *
   * @param command the command to check
   * @return true if the command is scheduled to run or is already running, false if not
   */
  public boolean isScheduledOrRunning(Command command) {
    return isScheduled(command) || isRunning(command);
  }

  /**
   * Gets the set of all currently running commands. Commands are returned in the order in which
   * they were scheduled. The returned set is read-only.
   *
   * @return the currently running commands
   */
  public Collection<Command> getRunningCommands() {
    return Collections.unmodifiableSet(commandStates.keySet());
  }

  /**
   * Gets all the currently running commands that require a particular resource. Commands are
   * returned in the order in which they were scheduled. The returned list is read-only.
   *
   * @param resource the resource to get the commands for
   * @return the currently running commands that require the resource.
   */
  public List<Command> getRunningCommandsFor(RequireableResource resource) {
    return commandStates.keySet().stream().filter(command -> command.requires(resource)).toList();
  }

  /**
   * Cancels all currently running commands. Commands that are scheduled that haven't yet started
   * will remain scheduled, and will start on the next call to {@link #run()}.
   */
  public void cancelAll() {
    // Remove scheduled children of running commands
    for (Iterator<CommandState> iterator = onDeck.iterator(); iterator.hasNext(); ) {
      CommandState state = iterator.next();
      if (commandStates.containsKey(state.parent())) {
        iterator.remove();
        emitEvent(SchedulerEvent.evicted(state.command()));
      }
    }

    // Finally, remove running commands
    commandStates.forEach((command, _state) -> emitEvent(SchedulerEvent.evicted(command)));
    commandStates.clear();
  }

  /**
   * An event loop used by the scheduler to poll triggers that schedule or cancel commands. This
   * event loop is always polled on every call to {@link #run()}. Custom event loops need to be
   * bound to this one for synchronicity with the scheduler; however, they can always be polled
   * manually before or after the call to {@link #run()}.
   *
   * @return the default event loop.
   */
  public EventLoop getDefaultEventLoop() {
    return eventLoop;
  }

  /** For internal use. */
  public Collection<Command> getQueuedCommands() {
    return onDeck.stream().map(CommandState::command).toList();
  }

  /** For internal use. */
  public Command getParentOf(Command command) {
    var state = commandStates.get(command);
    if (state == null) {
      return null;
    }
    return state.parent();
  }

  /**
   * Gets how long a command took to run in the previous cycle. If the command is not currently
   * running, returns -1.
   *
   * @param command The command to check
   * @return How long, in milliseconds, the command last took to execute.
   */
  public double lastRuntimeMs(Command command) {
    if (commandStates.containsKey(command)) {
      return commandStates.get(command).lastRuntimeMs();
    } else {
      return -1;
    }
  }

  /**
   * Gets how long a command has taken to run, in aggregate, since it was most recently scheduled.
   * If the command is not currently running, returns -1.
   *
   * @param command The command to check
   * @return How long, in milliseconds, the command has taken to execute in total
   */
  public double totalRuntimeMs(Command command) {
    if (commandStates.containsKey(command)) {
      return commandStates.get(command).totalRuntimeMs();
    } else {
      // Not running; no data
      return -1;
    }
  }

  public int runId(Command command) {
    if (commandStates.containsKey(command)) {
      return commandStates.get(command).id();
    }

    // Check scheduled commands
    for (var scheduled : onDeck) {
      if (scheduled.command() == command) {
        return scheduled.id();
      }
    }

    return 0;
  }

  /**
   * Gets how long the scheduler took to process its most recent {@link #run()} invocation, in
   * milliseconds. Defaults to -1 if the scheduler has not yet run.
   *
   * @return How long, in milliseconds, the scheduler last took to execute.
   */
  public double lastRuntimeMs() {
    return lastRunTimeMs;
  }

  /**
   * Adds a listener to handle events that are emitted by the scheduler. Events are emitted when
   * certain actions are taken by user code or by internal processing logic in the scheduler.
   * Listeners should take care to be quick, simple, and not schedule or cancel commands, as that
   * may cause inconsistent scheduler behavior or even cause a program crash.
   *
   * <p>Listeners are primarily expected to be for data logging and telemetry. In particular, a
   * one-shot command (one that never calls {@link Coroutine#yield()}) will never appear in the
   * standard protobuf telemetry because it is scheduled, runs, and finishes all in a single
   * scheduler cycle. However, {@link SchedulerEvent.Scheduled},{@link SchedulerEvent.Mounted}, and
   * {@link SchedulerEvent.Completed} events will be emitted corresponding to those actions, and
   * user code can listen for and log such events.
   *
   * @param listener The listener to add. Cannot be null.
   * @throws NullPointerException if given a null listener
   */
  public void addEventListener(Consumer<? super SchedulerEvent> listener) {
    ErrorMessages.requireNonNullParam(listener, "listener", "addEventListener");

    eventListeners.add(listener);
  }

  private void emitEvent(SchedulerEvent event) {
    // TODO: Prevent listeners from interacting with the scheduler.
    //       Scheduling or cancelling commands while the scheduler is processing will probably cause
    //       bugs in user code or even a program crash.
    for (var listener : eventListeners) {
      listener.accept(event);
    }
  }
}
