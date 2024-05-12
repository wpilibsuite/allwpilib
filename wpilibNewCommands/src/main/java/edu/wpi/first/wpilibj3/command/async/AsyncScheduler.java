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

  private record CommandState(AsyncCommand command, Continuation continuation) { }

  /** The default commands for each resource. */
  private final Map<HardwareResource, AsyncCommand> defaultCommands = new HashMap<>();
  /** The set of commands scheduled since the start of the previous run. */
  private final Set<AsyncCommand> onDeck = new LinkedHashSet<>();
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

  public static AsyncScheduler getInstance() {
    return defaultScheduler;
  }

  public AsyncScheduler() {
  }

  public void registerResource(HardwareResource resource) {
    registerResource(resource, new IdleCommand(resource));
  }

  public void registerResource(HardwareResource resource, AsyncCommand defaultCommand) {
    setDefaultCommand(resource, defaultCommand);
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

    for (var scheduledCommand : onDeck) {
      if (!command.conflictsWith(scheduledCommand)) {
        // No shared requirements, skip
        continue;
      }
      if (command.isLowerPriorityThan(scheduledCommand)) {
        // Lower priority than an already-scheduled (but not yet running) command that requires at
        // one of the same resource
        return;
      }
    }

    // Evict conflicting on-deck commands
    // We check above if the input command is lower priority than any of these,
    // so at this point we're guaranteed to be >= priority than anything already on deck
    onDeck.removeIf(c -> c.conflictsWith(command));

    onDeck.add(command);

    commandStates
        .entrySet()
        .removeIf(e -> e.getKey() != currentCommand && e.getKey().conflictsWith(command));
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
    eventLoop.poll();

    // Apply scheduled commands, evicting any commands that require the same resources
    // Priority checking is done at schedule time so we don't need to repeat those checks here
    // Note that if multiple commands are scheduled in the same loop that use the same resources,
    // then the final scheduled command will win out.
    for (var command : onDeck) {
      commandStates.put(command, new CommandState(command, buildContinuation(command)));
    }

    // Clear the set of on-deck commands,
    // since we just put them all into the set of running commands
    onDeck.clear();

    for (Continuation callback : periodicCallbacks) {
      callback.run();
    }

    periodicCallbacks.removeIf(Continuation::isDone);

    // Tick every command that hasn't been completed yet
    for (var entry : List.copyOf(commandStates.entrySet())) {
      final var command = entry.getKey();
      Continuation continuation = entry.getValue().continuation;

      if (!commandStates.containsKey(command)) {
        // Probably canceled by an owning composition, do not run
        continue;
      }

      if (!continuation.isDone()) {
        currentCommand = command;
        mountContinuation(continuation);
        try {
          continuation.run();
        } finally {
          mountContinuation(null);
        }
      }
    }

    currentCommand = null;

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

  private Continuation buildContinuation(AsyncCommand command) {
    return new Continuation(scope, () -> {
      try {
        command.run();
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

  public boolean yield() {
    return Continuation.yield(scope);
  }

  public EventLoop getDefaultButtonLoop() {
    return eventLoop;
  }
}
