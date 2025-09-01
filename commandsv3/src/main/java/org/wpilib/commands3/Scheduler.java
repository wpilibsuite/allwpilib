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
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.SequencedMap;
import java.util.SequencedSet;
import java.util.Set;
import java.util.Stack;
import java.util.function.Consumer;
import java.util.stream.Collectors;
import org.wpilib.commands3.button.CommandGenericHID;
import org.wpilib.commands3.proto.SchedulerProto;

/**
 * Manages the lifecycles of {@link Coroutine}-based {@link Command Commands}. Commands may be
 * scheduled directly using {@link #schedule(Command)}, or be bound to {@link Trigger Triggers} to
 * automatically handle scheduling and cancellation based on internal or external events. User code
 * is responsible for calling {@link #run()} periodically to update trigger conditions and execute
 * scheduled commands. Most often, this is done by overriding {@link TimedRobot#robotPeriodic()} to
 * include a call to {@code Scheduler.getDefault().run()}:
 *
 * <pre>{@code
 * public class Robot extends TimedRobot {
 *   @Override
 *   public void robotPeriodic() {
 *     // Update the scheduler on every loop
 *     Scheduler.getDefault().run();
 *   }
 * }
 * }</pre>
 *
 * <h2>Danger</h2>
 *
 * <p>The scheduler <i>must</i> be used in a single-threaded program. Commands must be scheduled and
 * cancelled by the same thread that runs the scheduler, and cannot be run in a virtual thread.
 *
 * <p><strong>Using the commands framework in a multithreaded environment can cause crashes in the
 * Java virtual machine at any time, including on an official field during a match.</strong> The
 * Java JIT compilers make assumptions that rely on coroutines being used on a single thread.
 * Breaking those assumptions can cause incorrect JIT code to be generated with undefined behavior,
 * potentially causing control issues or crashes deep in JIT-generated native code.
 *
 * <p><strong>Normal concurrency constructs like locks, atomic references, and synchronized blocks
 * or methods cannot save you.</strong>
 *
 * <h2>Lifecycle</h2>
 *
 * <p>The {@link #run()} method runs five steps:
 *
 * <ol>
 *   <li>Poll all registered triggers to queue and cancel commands
 *   <li>Call {@link #sideload(Consumer) periodic sideload functions}
 *   <li>Start all queued commands. This happens after all triggers are checked in case multiple
 *       commands with conflicting requirements are queued in the same update; the last command to
 *       be queued takes precedence over the rest.
 *   <li>Loop over all running commands, mounting and calling each in turn until they either exit or
 *       call {@link Coroutine#yield()}. Commands run in the order in which they were scheduled.
 *   <li>Queue default commands for any mechanisms without a running command. The queued commands
 *       can be superseded by any manual scheduling or commands scheduled by triggers in the next
 *       run.
 * </ol>
 *
 * <h2>Telemetry</h2>
 *
 * <p>There are two mechanisms for telemetry for a scheduler. A protobuf serializer can be used to
 * take a snapshot of a scheduler instance, and report what commands are queued (scheduled but have
 * not yet started to run), commands that are running (along with timing data for each command), and
 * total time spent in the most recent {@link #run()} call. However, it cannot detect one-shot
 * commands that are scheduled, run, and complete all in a single {@code run()} invocation -
 * effectively, commands that never call {@link Coroutine#yield()} are invisible.
 *
 * <p>A second telemetry mechanism is provided by {@link #addEventListener(Consumer)}. The scheduler
 * will issue events to all registered listeners when certain events occur (see {@link
 * SchedulerEvent} for all event types). These events are emitted immediately and can be used to
 * detect lifecycle events for all commands, including one-shots that would be invisible to the
 * protobuf serializer. However, it is up to the user to log those events themselves.
 */
public class Scheduler implements ProtobufSerializable {
  private final Map<Mechanism, Command> m_defaultCommands = new LinkedHashMap<>();

  /** The set of commands scheduled since the start of the previous run. */
  private final SequencedSet<CommandState> m_queuedToRun = new LinkedHashSet<>();

  /**
   * The states of all running commands (does not include on deck commands). We preserve insertion
   * order to guarantee that child commands run after their parents.
   */
  private final SequencedMap<Command, CommandState> m_runningCommands = new LinkedHashMap<>();

  /**
   * The stack of currently executing commands. Child commands are pushed onto the stack and popped
   * when they complete. Use {@link #currentState()} and {@link #currentCommand()} to get the
   * currently executing command or its state.
   */
  private final Stack<CommandState> m_currentCommandAncestry = new Stack<>();

  /** The periodic callbacks to run, outside of the command structure. */
  private final List<Coroutine> m_periodicCallbacks = new ArrayList<>();

  /** Event loop for trigger bindings. */
  private final EventLoop m_eventLoop = new EventLoop();

  /** The scope for continuations to yield to. */
  private final ContinuationScope m_scope = new ContinuationScope("coroutine commands");

  // Telemetry
  /** Protobuf serializer for a scheduler. */
  public static final SchedulerProto proto = new SchedulerProto();

  private double m_lastRunTimeMs = -1;

  private final Set<Consumer<? super SchedulerEvent>> m_eventListeners = new LinkedHashSet<>();

  /** The default scheduler instance. */
  private static final Scheduler s_defaultScheduler = new Scheduler();

  /**
   * Gets the default scheduler instance for use in a robot program. Unless otherwise specified,
   * triggers and mechanisms will be registered with the default scheduler and require the default
   * scheduler to run. However, triggers and mechanisms can be constructed to be registered with a
   * specific scheduler instance, which may be useful for isolation for unit tests.
   *
   * @return the default scheduler instance.
   */
  public static Scheduler getDefault() {
    return s_defaultScheduler;
  }

  /**
   * Creates a new scheduler object. Note that most built-in constructs like {@link Trigger} and
   * {@link CommandGenericHID} will use the {@link #getDefault() default scheduler instance} unless
   * they were explicitly constructed with a different scheduler instance. Teams should use the
   * default instance for convenience; however, new scheduler instances can be useful for unit
   * tests.
   */
  public Scheduler() {}

  /**
   * Sets the default command for a mechanism. The command must require that mechanism, and cannot
   * require any other mechanisms.
   *
   * @param mechanism the mechanism for which to set the default command
   * @param defaultCommand the default command to execute on the mechanism
   * @throws IllegalArgumentException if the command does not meet the requirements for being a
   *     default command
   */
  public void scheduleAsDefaultCommand(Mechanism mechanism, Command defaultCommand) {
    if (!defaultCommand.requires(mechanism)) {
      throw new IllegalArgumentException(
          "A mechanism's default command must require that mechanism");
    }

    if (defaultCommand.requirements().size() > 1) {
      throw new IllegalArgumentException(
          "A mechanism's default command cannot require other mechanisms");
    }

    m_defaultCommands.put(mechanism, defaultCommand);
    schedule(defaultCommand);
  }

  /**
   * Gets the default command set for a mechanism.
   *
   * @param mechanism The mechanism
   * @return The default command, or null if no default command was ever set
   */
  public Command getDefaultCommandFor(Mechanism mechanism) {
    return m_defaultCommands.get(mechanism);
  }

  /**
   * Adds a callback to run as part of the scheduler. The callback should not manipulate or control
   * any mechanisms, but can be used to log information, update data (such as simulations or LED
   * data buffers), or perform some other helpful task. The callback is responsible for managing its
   * own control flow and end conditions. If you want to run a single task periodically for the
   * entire lifespan of the scheduler, use {@link #addPeriodic(Runnable)}.
   *
   * <p><strong>Note:</strong> Like commands, any loops in the callback must appropriately yield
   * control back to the scheduler with {@link Coroutine#yield} or risk stalling your program in an
   * unrecoverable infinite loop!
   *
   * @param callback the callback to sideload
   */
  public void sideload(Consumer<Coroutine> callback) {
    var coroutine = new Coroutine(this, m_scope, callback);
    m_periodicCallbacks.add(coroutine);
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
        coroutine -> {
          while (true) {
            callback.run();
            coroutine.yield();
          }
        });
  }

  /** Represents possible results of a command scheduling attempt. */
  public enum ScheduleResult {
    /** The command was successfully scheduled and added to the queue. */
    SUCCESS,
    /** The command is already scheduled or running. */
    ALREADY_RUNNING,
    /** The command is a lower priority and conflicts with a command that's already running. */
    LOWER_PRIORITY_THAN_RUNNING_COMMAND,
  }

  /**
   * Schedules a command to run. If one command schedules another (a "parent" and "child"), the
   * child command will be canceled when the parent command completes. It is not possible to fork a
   * child command and have it live longer than its parent.
   *
   * <p>Does nothing if the command is already scheduled or running, or requires at least one
   * mechanism already used by a higher priority command.
   *
   * @param command the command to schedule
   * @return the result of the scheduling attempt. See {@link ScheduleResult} for details.
   * @throws IllegalArgumentException if scheduled by a command composition that has already
   *     scheduled another command that shares at least one required mechanism
   */
  // Implementation detail: a child command will immediately start running when scheduled by a
  // parent command, skipping the queue entirely. This avoids dead loop cycles where a parent
  // schedules a child, appending it to the queue, then waits for the next cycle to pick it up and
  // start it. With deeply nested commands, dead loops could quickly to add up and cause the
  // innermost commands that actually _do_ something to start running hundreds of milliseconds after
  // their root ancestor was scheduled.
  public ScheduleResult schedule(Command command) {
    // Note: we use a throwable here instead of Thread.currentThread().getStackTrace() for easier
    //       stack frame filtering and modification.
    var binding =
        new Binding(
            BindingScope.global(), BindingType.IMMEDIATE, command, new Throwable().getStackTrace());

    return schedule(binding);
  }

  // Package-private for use by Trigger
  ScheduleResult schedule(Binding binding) {
    var command = binding.command();

    if (isScheduledOrRunning(command)) {
      return ScheduleResult.ALREADY_RUNNING;
    }

    if (lowerPriorityThanConflictingCommands(command)) {
      return ScheduleResult.LOWER_PRIORITY_THAN_RUNNING_COMMAND;
    }

    for (var scheduledState : m_queuedToRun) {
      if (!command.conflictsWith(scheduledState.command())) {
        // No shared requirements, skip
        continue;
      }
      if (command.isLowerPriorityThan(scheduledState.command())) {
        // Lower priority than an already-scheduled (but not yet running) command that requires at
        // one of the same mechanism. Ignore it.
        return ScheduleResult.LOWER_PRIORITY_THAN_RUNNING_COMMAND;
      }
    }

    // Evict conflicting on-deck commands
    // We check above if the input command is lower priority than any of these,
    // so at this point we're guaranteed to be >= priority than anything already on deck
    evictConflictingOnDeckCommands(command);

    // If the binding is scoped to a particular command, that command is the parent. If we're in the
    // middle of a run cycle and running commands, the parent is whatever command is currently
    // running. Otherwise, there is no parent command.
    var parentCommand =
        binding.scope() instanceof BindingScope.ForCommand scope
            ? scope.command()
            : currentCommand();
    var state = new CommandState(command, parentCommand, buildCoroutine(command), binding);

    emitScheduledEvent(command);

    if (currentState() != null) {
      // Scheduling a child command while running. Start it immediately instead of waiting a full
      // cycle. This prevents issues with deeply nested command groups taking many scheduler cycles
      // to start running the commands that actually _do_ things
      evictConflictingRunningCommands(state);
      m_runningCommands.put(command, state);
      runCommand(state);
    } else {
      // Scheduling outside a command, add it to the pending set. If it's not overridden by another
      // conflicting command being scheduled in the same scheduler loop, it'll be promoted and
      // start to run when #runCommands() is called
      m_queuedToRun.add(state);
    }

    return ScheduleResult.SUCCESS;
  }

  /**
   * Checks if a command conflicts with and is a lower priority than any running command. Used when
   * determining if the command can be scheduled.
   */
  private boolean lowerPriorityThanConflictingCommands(Command command) {
    Set<CommandState> ancestors = new HashSet<>();
    for (var state = currentState(); state != null; state = m_runningCommands.get(state.parent())) {
      ancestors.add(state);
    }

    // Check for conflicts with the commands that are already running
    for (var state : m_runningCommands.values()) {
      if (ancestors.contains(state)) {
        // Can't conflict with an ancestor command
        continue;
      }

      var c = state.command();
      if (c.conflictsWith(command) && command.isLowerPriorityThan(c)) {
        return true;
      }
    }

    return false;
  }

  private void evictConflictingOnDeckCommands(Command command) {
    for (var iterator = m_queuedToRun.iterator(); iterator.hasNext(); ) {
      var scheduledState = iterator.next();
      var scheduledCommand = scheduledState.command();
      if (scheduledCommand.conflictsWith(command)) {
        // Remove the lower priority conflicting command from the on deck commands.
        // We don't need to call removeOrphanedChildren here because it hasn't started yet,
        // meaning it hasn't had a chance to schedule any children
        iterator.remove();
        emitInterruptedEvent(scheduledCommand, command);
        emitCanceledEvent(scheduledCommand);
      }
    }
  }

  /**
   * Cancels all running commands with which an incoming state conflicts. Ancestor commands of the
   * incoming state will not be canceled.
   */
  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  private void evictConflictingRunningCommands(CommandState incomingState) {
    // The set of root states with which the incoming state conflicts but does not inherit from
    Set<CommandState> conflictingRootStates =
        m_runningCommands.values().stream()
            .filter(state -> incomingState.command().conflictsWith(state.command()))
            .filter(state -> !isAncestorOf(state.command(), incomingState))
            .map(
                state -> {
                  // Find the highest level ancestor of the conflicting command from which the
                  // incoming state does _not_ inherit. If they're totally unrelated, this will
                  // get the very root ancestor; otherwise, it'll return a direct sibling of the
                  // incoming command
                  CommandState root = state;
                  while (root.parent() != null && root.parent() != incomingState.parent()) {
                    root = m_runningCommands.get(root.parent());
                  }
                  return root;
                })
            .collect(Collectors.toSet());

    // Cancel the root commands
    for (var conflictingState : conflictingRootStates) {
      emitInterruptedEvent(conflictingState.command(), incomingState.command());
      cancel(conflictingState.command());
    }
  }

  /**
   * Checks if a particular command is an ancestor of another.
   *
   * @param ancestor the potential ancestor for which to search
   * @param state the state to check
   * @return true if {@code ancestor} is the direct parent or indirect ancestor, false if not
   */
  @SuppressWarnings({"PMD.CompareObjectsWithEquals", "PMD.SimplifyBooleanReturns"})
  private boolean isAncestorOf(Command ancestor, CommandState state) {
    if (state.parent() == null) {
      // No parent, cannot inherit
      return false;
    }
    if (!m_runningCommands.containsKey(ancestor)) {
      // The given ancestor isn't running
      return false;
    }
    if (state.parent() == ancestor) {
      // Direct child
      return true;
    }
    // Check if the command's parent inherits from the given ancestor
    return m_runningCommands.values().stream()
        .filter(s -> state.parent() == s.command())
        .anyMatch(s -> isAncestorOf(ancestor, s));
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
  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  public void cancel(Command command) {
    boolean running = isRunning(command);

    // Evict the command. The next call to run() will schedule the default command for all its
    // required mechanisms, unless another command requiring those mechanisms is scheduled between
    // calling cancel() and calling run()
    m_runningCommands.remove(command);
    m_queuedToRun.removeIf(state -> state.command() == command);

    if (running) {
      // Only run the hook if the command was running. If it was on deck or not
      // even in the scheduler at the time, then there's nothing to do
      command.onCancel();
      emitCanceledEvent(command);
    }

    // Clean up any orphaned child commands; their lifespan must not exceed the parent's
    removeOrphanedChildren(command);
  }

  /**
   * Updates the command scheduler. This will process trigger bindings on anything attached to the
   * {@link #getDefaultEventLoop() default event loop}, begin running any commands scheduled since
   * the previous call to {@code run()}, process periodic callbacks added with {@link
   * #addPeriodic(Runnable)} and {@link #sideload(Consumer)}, update running commands, and schedule
   * default commands for any mechanisms that are not owned by a running command.
   *
   * <p>This method is intended to be called in a periodic loop like {@link
   * TimedRobot#robotPeriodic()}
   */
  public void run() {
    final long startMicros = RobotController.getTime();

    // Process triggers first; these tend to queue and cancel commands
    m_eventLoop.poll();

    runPeriodicSideloads();
    promoteScheduledCommands();
    runCommands();
    scheduleDefaultCommands();

    final long endMicros = RobotController.getTime();
    m_lastRunTimeMs = Milliseconds.convertFrom(endMicros - startMicros, Microseconds);
  }

  private void promoteScheduledCommands() {
    // Clear any commands that conflict with the scheduled set
    for (var queuedState : m_queuedToRun) {
      evictConflictingRunningCommands(queuedState);
    }

    // Move any scheduled commands to the running set
    for (var queuedState : m_queuedToRun) {
      m_runningCommands.put(queuedState.command(), queuedState);
    }

    // Clear the set of on-deck commands,
    // since we just put them all into the set of running commands
    m_queuedToRun.clear();
  }

  private void runPeriodicSideloads() {
    // Update periodic callbacks
    for (Coroutine coroutine : m_periodicCallbacks) {
      coroutine.mount();
      try {
        coroutine.runToYieldPoint();
      } finally {
        Continuation.mountContinuation(null);
      }
    }

    // And remove any periodic callbacks that have completed
    m_periodicCallbacks.removeIf(Coroutine::isDone);
  }

  private void runCommands() {
    // Tick every command that hasn't been completed yet
    // Run in reverse so parent commands can resume in the same loop cycle an awaited child command
    // completes. Otherwise, parents could only resume on the next loop cycle, introducing a delay
    // at every layer of nesting.
    for (var state : List.copyOf(m_runningCommands.values()).reversed()) {
      runCommand(state);
    }
  }

  /**
   * Mounts and runs a command until it yields or exits.
   *
   * @param state The command state to run
   */
  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  private void runCommand(CommandState state) {
    final var command = state.command();
    final var coroutine = state.coroutine();

    if (!m_runningCommands.containsKey(command)) {
      // Probably canceled by an owning composition, do not run
      return;
    }

    var previousState = currentState();

    m_currentCommandAncestry.push(state);
    long startMicros = RobotController.getTime();
    emitMountedEvent(command);
    coroutine.mount();
    try {
      coroutine.runToYieldPoint();
    } catch (RuntimeException e) {
      // Intercept the exception, inject stack frames from the schedule site, and rethrow it
      var binding = state.binding();
      e.setStackTrace(CommandTraceHelper.modifyTrace(e.getStackTrace(), binding.frames()));
      emitCompletedWithErrorEvent(command, e);
      throw e;
    } finally {
      long endMicros = RobotController.getTime();
      double elapsedMs = Milliseconds.convertFrom(endMicros - startMicros, Microseconds);
      state.setLastRuntimeMs(elapsedMs);

      if (state.equals(currentState())) {
        // Remove the command we just ran from the top of the stack
        m_currentCommandAncestry.pop();
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
      emitCompletedEvent(command);
      m_runningCommands.remove(command);
      removeOrphanedChildren(command);
    } else {
      // Yielded
      emitYieldedEvent(command);
    }
  }

  /**
   * Gets the currently executing command state, or null if no command is currently executing.
   *
   * @return the currently executing command state
   */
  private CommandState currentState() {
    if (m_currentCommandAncestry.isEmpty()) {
      // Avoid EmptyStackException
      return null;
    }

    return m_currentCommandAncestry.peek();
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
    // Schedule the default commands for every mechanism that doesn't currently have a running or
    // scheduled command.
    m_defaultCommands.forEach(
        (mechanism, defaultCommand) -> {
          if (m_runningCommands.keySet().stream().noneMatch(c -> c.requires(mechanism))
              && m_queuedToRun.stream().noneMatch(c -> c.command().requires(mechanism))
              && defaultCommand != null) {
            // Nothing currently running or scheduled
            // Schedule the mechanism's default command, if it has one
            schedule(defaultCommand);
          }
        });
  }

  /**
   * Removes all commands descended from a parent command. This is used to ensure that any command
   * scheduled within a composition or group cannot live longer than any ancestor.
   *
   * @param parent the root command whose descendants to remove from the scheduler
   */
  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  private void removeOrphanedChildren(Command parent) {
    m_runningCommands.entrySet().stream()
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
    return new Coroutine(this, m_scope, command::run);
  }

  /**
   * Checks if a command is currently running.
   *
   * @param command the command to check
   * @return true if the command is running, false if not
   */
  public boolean isRunning(Command command) {
    return m_runningCommands.containsKey(command);
  }

  /**
   * Checks if a command is currently scheduled to run, but is not yet running.
   *
   * @param command the command to check
   * @return true if the command is scheduled to run, false if not
   */
  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  public boolean isScheduled(Command command) {
    return m_queuedToRun.stream().anyMatch(state -> state.command() == command);
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
    return Collections.unmodifiableSet(m_runningCommands.keySet());
  }

  /**
   * Gets all the currently running commands that require a particular mechanism. Commands are
   * returned in the order in which they were scheduled. The returned list is read-only.
   *
   * @param mechanism the mechanism to get the commands for
   * @return the currently running commands that require the mechanism.
   */
  public List<Command> getRunningCommandsFor(Mechanism mechanism) {
    return m_runningCommands.keySet().stream()
        .filter(command -> command.requires(mechanism))
        .toList();
  }

  /**
   * Cancels all currently running and scheduled commands. All default commands will be scheduled on
   * the next call to {@link #run()}, unless a higher priority command is scheduled or triggered
   * after {@code cancelAll()} is used.
   */
  public void cancelAll() {
    for (var onDeckIter = m_queuedToRun.iterator(); onDeckIter.hasNext(); ) {
      var state = onDeckIter.next();
      onDeckIter.remove();
      emitCanceledEvent(state.command());
    }

    for (var liveIter = m_runningCommands.entrySet().iterator(); liveIter.hasNext(); ) {
      var entry = liveIter.next();
      liveIter.remove();
      Command canceledCommand = entry.getKey();
      canceledCommand.onCancel();
      emitCanceledEvent(canceledCommand);
    }
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
    return m_eventLoop;
  }

  /**
   * For internal use.
   *
   * @return The commands that have been scheduled but not yet started.
   */
  public Collection<Command> getQueuedCommands() {
    return m_queuedToRun.stream().map(CommandState::command).toList();
  }

  /**
   * For internal use.
   *
   * @param command The command to check
   * @return The command that forked the provided command. Null if the command is not a child of
   *     another command.
   */
  public Command getParentOf(Command command) {
    var state = m_runningCommands.get(command);
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
  public double lastCommandRuntimeMs(Command command) {
    if (m_runningCommands.containsKey(command)) {
      return m_runningCommands.get(command).lastRuntimeMs();
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
    if (m_runningCommands.containsKey(command)) {
      return m_runningCommands.get(command).totalRuntimeMs();
    } else {
      // Not running; no data
      return -1;
    }
  }

  /**
   * Gets the unique run id for a scheduled or running command. If the command is not currently
   * scheduled or running, this function returns {@code 0}.
   *
   * @param command The command to get the run ID for
   * @return The run of the command
   */
  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  public int runId(Command command) {
    if (m_runningCommands.containsKey(command)) {
      return m_runningCommands.get(command).id();
    }

    // Check scheduled commands
    for (var scheduled : m_queuedToRun) {
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
    return m_lastRunTimeMs;
  }

  // Event-base telemetry and helpers. The static factories are for convenience to automatically
  // set the timestamp instead of littering RobotController.getTime() everywhere.

  private void emitScheduledEvent(Command command) {
    var event = new SchedulerEvent.Scheduled(command, RobotController.getTime());
    emitEvent(event);
  }

  private void emitMountedEvent(Command command) {
    var event = new SchedulerEvent.Mounted(command, RobotController.getTime());
    emitEvent(event);
  }

  private void emitYieldedEvent(Command command) {
    var event = new SchedulerEvent.Yielded(command, RobotController.getTime());
    emitEvent(event);
  }

  private void emitCompletedEvent(Command command) {
    var event = new SchedulerEvent.Completed(command, RobotController.getTime());
    emitEvent(event);
  }

  private void emitCompletedWithErrorEvent(Command command, Throwable error) {
    var event = new SchedulerEvent.CompletedWithError(command, error, RobotController.getTime());
    emitEvent(event);
  }

  private void emitCanceledEvent(Command command) {
    var event = new SchedulerEvent.Canceled(command, RobotController.getTime());
    emitEvent(event);
  }

  private void emitInterruptedEvent(Command command, Command interrupter) {
    var event = new SchedulerEvent.Interrupted(command, interrupter, RobotController.getTime());
    emitEvent(event);
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

    m_eventListeners.add(listener);
  }

  private void emitEvent(SchedulerEvent event) {
    // TODO: Prevent listeners from interacting with the scheduler.
    //       Scheduling or cancelling commands while the scheduler is processing will probably cause
    //       bugs in user code or even a program crash.
    for (var listener : m_eventListeners) {
      listener.accept(event);
    }
  }
}
