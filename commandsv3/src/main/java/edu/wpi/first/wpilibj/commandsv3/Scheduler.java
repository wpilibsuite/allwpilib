package edu.wpi.first.wpilibj.commandsv3;

import static edu.wpi.first.wpilibj.commandsv3.Command.InterruptBehavior.SuspendOnInterrupt;

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
import java.util.Stack;
import java.util.function.Consumer;
import java.util.stream.Collectors;

/**
 * Manages the lifecycles of {@link Coroutine}-based {@link Command Commands}.
 */
public class Scheduler {
  private final Set<RequireableResource> registeredResources = new HashSet<>();

  /** The set of commands scheduled since the start of the previous run. */
  private final Set<CommandState> onDeck = new LinkedHashSet<>();

  /** The states of all running commands (does not include on deck commands). */
  private final Map<Command, CommandState> commandStates = new LinkedHashMap<>();

  /**
   * The stack of currently executing commands. Child commands are pushed onto the stack and
   * popped when they complete. Use {@link #currentState()} and {@link #currentCommand()} to get
   * the currently executing command or its state.
   */
  private final Stack<CommandState> executingCommands = new Stack<>();

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

  /** The scope for continuations to yield to. */
  private final ContinuationScope scope = new ContinuationScope("coroutine commands");

  /** The default scheduler instance. */
  private static final Scheduler defaultScheduler = new Scheduler();

  private final Coroutine coroutine = () -> Scheduler.this;

  /**
   * Gets the default scheduler instance for use in a robot program. Some built in command types use
   * the default scheduler and will not work as expected if used on another scheduler instance.
   *
   * @return the default scheduler instance.
   */
  public static Scheduler getInstance() {
    return defaultScheduler;
  }

  public Scheduler() {}

  /**
   * Registers a resource with the scheduler and schedules its default command.
   *
   * @param resource the resource to register
   */
  public void registerResource(RequireableResource resource) {
    registeredResources.add(resource);
    if (resource.getDefaultCommand() instanceof Command defaultCommand) {
      scheduleAsDefaultCommand(resource, defaultCommand);
    }
  }

  /**
   * Sets the default command for a resource. The command must require the resource, and cannot
   * require any others. Default commands must {@link Command.InterruptBehavior#SuspendOnInterrupt
   * suspend on interrupt} and have a lower priority than {@link Command#DEFAULT_PRIORITY} to
   * function properly.
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

    if (defaultCommand.interruptBehavior() != SuspendOnInterrupt) {
      throw new IllegalArgumentException(
          "Default commands must have interrupt behavior set to SuspendOnInterrupt");
    }

    if (defaultCommand.priority() >= Command.DEFAULT_PRIORITY) {
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
   * running command. Child commands will automatically be cancelled by the scheduler when their
   * parent command completes or is canceled. Child commands will also immediately begin execution,
   * without needing to wait for the next {@link #run()} invocation. This allows highly nested
   * compositions to begin running the actual meaningful commands sooner without needing to wait
   * one scheduler cycle per nesting level; with the default 20ms update period, 5 levels of nesting
   * would be enough to delay actions by 100 milliseconds - instead of only 20.
   *
   * <p>Does nothing if the command is already scheduled or running, or requires at least one
   * resource already used by a higher priority command.</p>
   *
   * @param command the command to schedule
   * @throws IllegalArgumentException if scheduled by a command composition that has already
   *   scheduled another command that shares at least one required resource
   */
  public void schedule(Command command) {
    if (isScheduledOrRunning(command)) {
      return;
    }

    if (!isSchedulable(command)) {
      return;
    }

    // If scheduled within a composition, prevent the composition from scheduling multiple
    // conflicting commands
    // TODO: Should we return quietly instead of throwing an exception?
    var siblings = potentialSiblings();
    var conflictingSibling = siblings.stream().filter(command::conflictsWith).findFirst();
    if (conflictingSibling.isPresent()) {
      var conflicts = new ArrayList<>(command.requirements());
      conflicts.retainAll(conflictingSibling.get().requirements());
      throw new IllegalArgumentException(
          "Command " + command.name()
              + " requires resources that are already used by "
              + conflictingSibling.get().name()
              + ". Both require "
              + conflicts.stream().map(RequireableResource::getName).collect(Collectors.joining(", ")));
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
          suspend(new CommandState(command, currentCommand(), buildContinuation(command)));
        }
        return;
      }
    }

    // Evict conflicting on-deck commands
    // We check above if the input command is lower priority than any of these,
    // so at this point we're guaranteed to be >= priority than anything already on deck
    evictConflictingOnDeckCommands(command);

    var state = new CommandState(command, currentCommand(), buildContinuation(command));
    onDeck.add(state);

    // Immediately evict any conflicting running command, except for commands in its ancestry
    evictConflictingRunningCommands(state);

    if (currentState() != null) {
      // Scheduling a child command while running. Start it immediately instead of waiting a full
      // cycle. This prevents issues with deeply nested command groups taking many scheduler cycles
      // to start running the commands that actually /do/ things
      commandStates.put(command, state);
      onDeck.remove(state);
      runCommand(state);
    }
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

  private Collection<Command> potentialSiblings() {
    var siblings = new ArrayList<Command>();
    for (var state : commandStates.values()) {
      if (state.parent() != null && state.parent() == currentCommand()) {
        siblings.add(state.command());
      }
    }
    for (var state : onDeck) {
      if (state.parent() != null && state.parent() == currentCommand()) {
        siblings.add(state.command());
      }
    }

    return siblings;
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
    return commandStates
               .values()
               .stream()
               .filter(s -> state.parent() == s.command())
               .anyMatch(s -> inheritsFrom(s, ancestor));
  }

  /**
   * Cancels a command and any other command scheduled by it. This occurs immediately and does not
   * need to wait for a call to {@link #run()}. Any command that it scheduled will also be canceled
   * to ensure commands within compositions do not continue to run.
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
    suspendedStates.removeIf(state -> state.command() == command);

    if (running) {
      // Only run the hook if the command was running. If it was on deck, suspended, or not
      // even in the scheduler at the time, then there's nothing to do
      command.onCancel();
    }

    // Clean up any orphaned child commands; their lifespan must not exceed the parent's
    removeOrphanedChildren(command);
  }

  private void suspend(CommandState state) {
    suspendedStates.add(state);
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
    for (var state : List.copyOf(commandStates.values())) {
      runCommand(state);
    }
  }

  private void runCommand(CommandState state) {
    final var command = state.command();
    var continuation = state.continuation();

    if (!commandStates.containsKey(command)) {
      // Probably canceled by an owning composition, do not run
      return;
    }

    var previousState = currentState();
    var previousContinuation = previousState == null ? null : previousState.continuation();

    Continuation.mountContinuation(continuation);
    try {
      executingCommands.push(state);
      continuation.run();
    } finally {
      if (currentState() == state) {
        // Remove the command we just ran from the top of the stack
        executingCommands.pop();
      }
      Continuation.mountContinuation(previousContinuation);
    }

    if (continuation.isDone()) {
      // Immediately check if the command has completed and remove any children commands.
      // This prevents child commands from being executed one extra time in the run() loop
      commandStates.remove(command);
      removeOrphanedChildren(command);
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
  private Command currentCommand() {
    var state = currentState();
    if (state == null) {
      return null;
    }

    return state.command();
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
        if (resource.getDefaultCommand() instanceof Command defaultCommand) {
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
  private void removeOrphanedChildren(Command parent) {
    commandStates
        .entrySet()
        .stream()
        .filter(e -> e.getValue().parent() == parent)
        .toList() // copy to an intermediate list to avoid concurrent modification
        .forEach(e -> cancel(e.getKey()));
  }

  /**
   * Builds a continuation object that the command will be bound to. The continuation will be scoped
   * to this scheduler object and cannot be used by another scheduler instance.
   *
   * @param command the command for which to build a continuation
   * @return the binding continuation
   */
  private Continuation buildContinuation(Command command) {
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
   * A running command may await completion of another command. The running command will be paused
   * until the command that is awaited upon completes. If the awaited command is not already
   * running, it will be scheduled and will start running the next time the {@link #run()} method
   * is called.
   *
   * @param command the command to wait for
   * @throws IllegalStateException if not called from a running command
   */
  public void await(Command command) {
    if (currentState() == null) {
      throw new IllegalStateException("Cannot await outside a command!");
    }

    checkWaitable(command);

    while (isScheduledOrRunning(command)) {
      this.yield();
    }
  }

  /**
   * A running command may await completion of multiple commands at once. The running command will
   * be paused until every command that is awaited on completes. Any awaited command that is not
   * already running will be scheduled and start running the next time the {@link #run()} method
   * is called.
   *
   * <p>Depending on the command requirements, any resources required by a command that finishes
   * early may not have its default command run or suspended commands resume, and will therefore
   * be in an <i>uncommanded</i> state. This occurs if the invoking (or "parent") command also
   * requires resources used by an awaited (or "child") command; while a "child" command that
   * requires a resource also required by its "parent" will not cancel its parent, when it finishes,
   * that resource is still required by the parent even if the parent is not actively using it. If
   * you need a resource to be able to run its default command, or have suspended commands resume,
   * when a child command finishes, simply drop the requirement for that resource from the parent.
   *
   * @param commands the commands to wait for
   * @throws IllegalStateException if not called from a running command
   */
  public void awaitAll(Collection<Command> commands) {
    if (currentState() == null) {
      throw new IllegalStateException("Cannot await outside a command!");
    }

    for (var command : commands) {
      checkWaitable(command);
    }

    // Wait as long as at least one command is still running
    while (commands.stream().anyMatch(this::isScheduledOrRunning)) {
      this.yield();
    }
  }

  /**
   * A running command may await completion of multiple commands at once. The running command will
   * be paused until <i>any</i> command that is awaited on completes. Once the first awaited on
   * command completes, the rest of the awaited commands are canceled and the running command will
   * be resumed. Any awaited command that is not already running will be scheduled and start running
   * the next time the {@link #run()} method is called.
   *
   * @param commands the commands to wait for
   * @throws IllegalStateException if not called from a running command
   */
  public void awaitAny(Collection<Command> commands) {
    if (currentState() == null) {
      throw new IllegalStateException("Cannot await outside a command!");
    }

    for (var command : commands) {
      checkWaitable(command);
    }

    // Wait as long as all commands are still running
    while (commands.stream().allMatch(this::isScheduledOrRunning)) {
      this.yield();
    }

    // Cancel any command that's still running after the first one completes
    commands.forEach(this::cancel);
  }

  /**
   * Checks if the currently running command is permitted to wait for completion of another command.
   *
   * @param command the command to check
   */
  private void checkWaitable(Command command) {
    final CommandState state;

    if (isRunning(command)) {
      state = commandStates.get(command);
    } else if (isScheduled(command)) {
      state = onDeck.stream().filter(s -> s.command() == command).findFirst().orElseThrow();
    } else {
      throw new IllegalStateException(
          "Cannot wait for command " + command + " because it is not currently running");
    }

    if (state.parent() != null && state.parent() != currentCommand()) {
      throw new IllegalStateException("Only " + state.parent() + " can wait for " + command);
    }
  }

  /**
   * Gets the set of all currently running commands. Commands are returned in the order in which
   * they were scheduled. The returned set is read-only.
   *
   * @return the currently running commands
   */
  public Set<Command> getRunningCommands() {
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
    onDeck.removeIf(s -> commandStates.containsKey(s.parent()));

    // Remove suspended children of running commands
    suspendedStates.removeIf(s -> commandStates.containsKey(s.parent()));

    // Finally, remove running commands
    commandStates.clear();
  }

  /**
   * Called by {@link Command#run(Coroutine)}, this will cause the command's execution to pause and
   * cede control back to the scheduler.
   *
   * @return true
   * @throws IllegalStateException if called outside a command that is currently being executed by
   *     the scheduler
   */
  public boolean yield() {
    return Continuation.yield(scope);
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
}
