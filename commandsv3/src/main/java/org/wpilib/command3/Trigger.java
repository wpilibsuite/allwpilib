// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.wpilib.units.Units.Seconds;
import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.EnumMap;
import java.util.List;
import java.util.Map;
import java.util.function.BooleanSupplier;
import org.wpilib.event.EventLoop;
import org.wpilib.math.filter.Debouncer;
import org.wpilib.units.measure.Time;

/**
 * Triggers allow users to specify conditions for when commands should run. Triggers can be set up
 * to read from joystick and controller buttons (eg {@link
 * org.wpilib.command3.button.CommandXboxController#x()}) or be customized to read sensor values or
 * any other arbitrary true/false condition.
 *
 * <p>It is very easy to link a button to a command. For instance, you could link the trigger button
 * of a joystick to a "score" command.
 *
 * <p>Triggers can easily be composed for advanced functionality using the {@link
 * #and(BooleanSupplier)}, {@link #or(BooleanSupplier)}, {@link #negate()} operators.
 *
 * <p>Trigger bindings created inside a running command will only be active while that command is
 * running. This is useful for defining trigger-based behavior only in a certain scope and avoids
 * needing to create dozens of global triggers. Any commands scheduled by these triggers will be
 * canceled when the enclosing command exits.
 *
 * <pre>{@code
 * Command shootWhileAiming = Command.noRequirements().executing(co -> {
 *   turret.atTarget.onTrue(shooter.shootOnce());
 *   co.await(turret.lockOnGoal());
 * }).named("Shoot While Aiming");
 * controller.rightBumper().whileTrue(shootWhileAiming);
 * }</pre>
 */
public class Trigger implements BooleanSupplier {
  private final BooleanSupplier m_condition;
  private final EventLoop m_loop;
  private final Scheduler m_scheduler;

  /** The value of the signal before the most recent call to {@link #poll()}. May be null. */
  private Signal m_previousSignal;

  /** The value of the signal from the most recent call to {@link #poll()}. May be null. */
  private Signal m_cachedSignal;

  private final Map<BindingType, List<Binding>> m_bindings = new EnumMap<>(BindingType.class);
  private final Runnable m_eventLoopCallback = this::poll;
  private boolean m_isBoundToEventLoop; // used for lazily binding to the event loop
  private final Collection<Trigger> m_dependencyTriggers = new ArrayList<>();

  /**
   * Represents the state of a signal: high or low. Used instead of a boolean for nullity on the
   * first run, when the previous signal value is undefined and unknown.
   */
  enum Signal {
    /** The signal is high. */
    HIGH,
    /** The signal is low. */
    LOW
  }

  /**
   * Creates a new trigger based on the given condition. Polled by the scheduler's {@link
   * Scheduler#getDefaultEventLoop() default event loop}.
   *
   * @param scheduler The scheduler that should execute triggered commands.
   * @param condition the condition represented by this trigger
   */
  public Trigger(Scheduler scheduler, BooleanSupplier condition) {
    this(
        requireNonNullParam(scheduler, "scheduler", "Trigger"),
        scheduler.getDefaultEventLoop(),
        requireNonNullParam(condition, "condition", "Trigger"));
  }

  /**
   * Creates a new trigger based on the given condition. Triggered commands are executed by the
   * {@link Scheduler#getDefault() default scheduler}.
   *
   * <p>Polled by the default scheduler button loop.
   *
   * @param condition the condition represented by this trigger
   */
  public Trigger(BooleanSupplier condition) {
    this(Scheduler.getDefault(), condition);
  }

  /**
   * Creates a new trigger based on the given condition.
   *
   * @param scheduler The scheduler that should execute triggered commands.
   * @param loop The event loop to poll the trigger.
   * @param condition the condition represented by this trigger
   */
  public Trigger(Scheduler scheduler, EventLoop loop, BooleanSupplier condition) {
    m_scheduler = requireNonNullParam(scheduler, "scheduler", "Trigger");
    m_loop = requireNonNullParam(loop, "loop", "Trigger");
    m_condition = requireNonNullParam(condition, "condition", "Trigger");
  }

  /**
   * Creates a new trigger based on the given condition. Trigger dependencies are optionally added
   * in order to ensure intermediate unbound triggers can still fire in cases like {@code
   * baseTrigger.and(otherTrigger.fallingEdge()).onTrue(someCommand)}, where an intermediate trigger
   * is used to qualify a condition but is not bound to a command itself.
   *
   * @param scheduler The scheduler that should execute triggered commands.
   * @param loop The event loop to poll the trigger.
   * @param condition the condition represented by this trigger
   * @param dependencies Optional dependencies of this trigger.
   */
  public Trigger(
      Scheduler scheduler, EventLoop loop, BooleanSupplier condition, Trigger... dependencies) {
    this(scheduler, loop, condition);
    m_dependencyTriggers.addAll(Arrays.asList(dependencies));
  }

  /**
   * Starts the given command whenever the condition changes from `false` to `true`.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  public Trigger onTrue(Command command) {
    requireNonNullParam(command, "command", "onTrue");
    addBinding(BindingType.SCHEDULE_ON_RISING_EDGE, command);
    return this;
  }

  /**
   * Starts the given command whenever the condition changes from `true` to `false`.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  public Trigger onFalse(Command command) {
    requireNonNullParam(command, "command", "onFalse");
    addBinding(BindingType.SCHEDULE_ON_FALLING_EDGE, command);
    return this;
  }

  /**
   * Starts the given command when the condition changes to `true` and cancels it when the condition
   * changes to `false`.
   *
   * <p>Doesn't re-start the command if it ends while the condition is still `true`.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  public Trigger whileTrue(Command command) {
    requireNonNullParam(command, "command", "whileTrue");
    addBinding(BindingType.RUN_WHILE_HIGH, command);
    return this;
  }

  /**
   * Starts the given command when the condition changes to `false` and cancels it when the
   * condition changes to `true`.
   *
   * <p>Doesn't re-start the command if it ends while the condition is still `false`.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  public Trigger whileFalse(Command command) {
    requireNonNullParam(command, "command", "whileFalse");
    addBinding(BindingType.RUN_WHILE_LOW, command);
    return this;
  }

  /**
   * Toggles a command when the condition changes from `false` to `true`.
   *
   * @param command the command to toggle
   * @return this trigger, so calls can be chained
   */
  public Trigger toggleOnTrue(Command command) {
    requireNonNullParam(command, "command", "toggleOnTrue");
    addBinding(BindingType.TOGGLE_ON_RISING_EDGE, command);
    return this;
  }

  /**
   * Toggles a command when the condition changes from `true` to `false`.
   *
   * @param command the command to toggle
   * @return this trigger, so calls can be chained
   */
  public Trigger toggleOnFalse(Command command) {
    requireNonNullParam(command, "command", "toggleOnFalse");
    addBinding(BindingType.TOGGLE_ON_FALLING_EDGE, command);
    return this;
  }

  /**
   * Gets the boolean state of the trigger. Because triggers are checked when their event loop is
   * polled, which by default occurs when {@link Scheduler#run()} is called in user code, but may
   * differ for custom event loops, the state is only valid immediately after the event loop is
   * polled. If the underlying signal changes without a subsequent event loop poll, the return value
   * from {@code getAsBoolean()} may not agree with the result of checking the signal directly.
   *
   * <p>Triggers are only bound to an event loop when at least one command has been bound to the
   * trigger. This method will always return {@code false} before commands have been bound.
   *
   * @return The state of the trigger.
   */
  @Override
  public boolean getAsBoolean() {
    return m_cachedSignal == Signal.HIGH;
  }

  /**
   * Composes two triggers with logical AND.
   *
   * @param trigger the condition to compose with
   * @return A trigger which is active when both component triggers are active.
   */
  public Trigger and(BooleanSupplier trigger) {
    Trigger[] dependencies;
    if (trigger instanceof Trigger t) {
      dependencies = new Trigger[] {this, t};
    } else {
      dependencies = new Trigger[] {this};
    }
    return new Trigger(
        m_scheduler,
        m_loop,
        () -> m_condition.getAsBoolean() && trigger.getAsBoolean(),
        dependencies);
  }

  /**
   * Composes two triggers with logical OR.
   *
   * @param trigger the condition to compose with
   * @return A trigger which is active when either component trigger is active.
   */
  public Trigger or(BooleanSupplier trigger) {
    Trigger[] dependencies;
    if (trigger instanceof Trigger t) {
      dependencies = new Trigger[] {this, t};
    } else {
      dependencies = new Trigger[] {this};
    }
    return new Trigger(
        m_scheduler,
        m_loop,
        () -> m_condition.getAsBoolean() || trigger.getAsBoolean(),
        dependencies);
  }

  /**
   * Creates a new trigger that is active when this trigger is inactive, i.e. that acts as the
   * negation of this trigger.
   *
   * @return the negated trigger
   */
  public Trigger negate() {
    return new Trigger(m_scheduler, m_loop, () -> !m_condition.getAsBoolean(), this);
  }

  /**
   * Creates a new debounced trigger from this trigger - it will become active when this trigger has
   * been active for longer than the specified period.
   *
   * @param duration The debounce period.
   * @return The debounced trigger (rising edges debounced only)
   */
  public Trigger debounce(Time duration) {
    return debounce(duration, Debouncer.DebounceType.kRising);
  }

  /**
   * Creates a new debounced trigger from this trigger - it will become active when this trigger has
   * been active for longer than the specified period.
   *
   * @param duration The debounce period.
   * @param type The debounce type.
   * @return The debounced trigger.
   */
  public Trigger debounce(Time duration, Debouncer.DebounceType type) {
    var debouncer = new Debouncer(duration.in(Seconds), type);
    return new Trigger(
        m_scheduler, m_loop, () -> debouncer.calculate(m_condition.getAsBoolean()), this);
  }

  /**
   * Creates a trigger that activates on a rising edge of this trigger's signal. The rising edge
   * trigger is active in the same cycle that this trigger's condition is {@code true} while its
   * condition in the previous cycle was {@code false}. The resulting trigger will only be active
   * for that single cycle before going inactive again; therefore, {@link #onTrue(Command)} should
   * be used instead of {@link #whileTrue(Command)}, as commands bound using the latter method will
   * be immediately canceled after a single scheduler cycle.
   *
   * @return A rising edge trigger.
   */
  public Trigger risingEdge() {
    return new Trigger(
        m_scheduler,
        m_loop,
        () -> m_cachedSignal == Signal.HIGH && m_previousSignal == Signal.LOW,
        this);
  }

  /**
   * Creates a trigger that activates on a falling edge of this trigger's signal. The falling edge
   * trigger is active in the same cycle that this trigger's condition is {@code false} while its
   * condition in the previous cycle was {@code true}. The resulting trigger will only be active for
   * that single cycle before going inactive again; therefore, {@link #onTrue(Command)} should be
   * used instead of {@link #whileTrue(Command)}, as commands bound using the latter method will be
   * immediately canceled after a single scheduler cycle.
   *
   * @return A falling edge trigger.
   */
  public Trigger fallingEdge() {
    return new Trigger(
        m_scheduler,
        m_loop,
        () -> m_cachedSignal == Signal.LOW && m_previousSignal == Signal.HIGH,
        this);
  }

  private void poll() {
    // Clear bindings that no longer need to run
    // This should always be checked, regardless of signal change, since bindings may be scoped
    // and those scopes may become inactive.
    clearStaleBindings();

    m_previousSignal = m_cachedSignal;
    m_cachedSignal = readSignal();

    if (m_cachedSignal == m_previousSignal) {
      // No change in the signal. Nothing to do
      return;
    }

    if (m_cachedSignal == Signal.HIGH) {
      // Signal is now high when it wasn't before - a rising edge
      scheduleBindings(BindingType.SCHEDULE_ON_RISING_EDGE);
      scheduleBindings(BindingType.RUN_WHILE_HIGH);
      cancelBindings(BindingType.RUN_WHILE_LOW);
      toggleBindings(BindingType.TOGGLE_ON_RISING_EDGE);
    }

    if (m_cachedSignal == Signal.LOW) {
      // Signal is now low when it wasn't before - a falling edge
      scheduleBindings(BindingType.SCHEDULE_ON_FALLING_EDGE);
      scheduleBindings(BindingType.RUN_WHILE_LOW);
      cancelBindings(BindingType.RUN_WHILE_HIGH);
      toggleBindings(BindingType.TOGGLE_ON_FALLING_EDGE);
    }
  }

  private Signal readSignal() {
    if (m_condition.getAsBoolean()) {
      return Signal.HIGH;
    } else {
      return Signal.LOW;
    }
  }

  /** Removes bindings in inactive scopes. Running commands tied to those bindings are canceled. */
  private void clearStaleBindings() {
    m_bindings.forEach(
        (_bindingType, bindings) -> {
          for (var iterator = bindings.iterator(); iterator.hasNext(); ) {
            var binding = iterator.next();
            if (binding.scope().active()) {
              // This binding's scope is still active, leave it running
              continue;
            }

            // The scope is no long active. Remove the binding and immediately cancel its command.
            iterator.remove();
            m_scheduler.cancel(binding.command());
          }
        });
  }

  /**
   * Schedules all commands bound to the given binding type.
   *
   * @param bindingType the binding type to schedule
   */
  private void scheduleBindings(BindingType bindingType) {
    m_bindings.getOrDefault(bindingType, List.of()).forEach(m_scheduler::schedule);
  }

  /**
   * Cancels all commands bound to the given binding type.
   *
   * @param bindingType the binding type to cancel
   */
  private void cancelBindings(BindingType bindingType) {
    m_bindings
        .getOrDefault(bindingType, List.of())
        .forEach(binding -> m_scheduler.cancel(binding.command()));
  }

  /**
   * Toggles all commands bound to the given binding type. If a command is currently scheduled or
   * running, it will be canceled; otherwise, it will be scheduled.
   *
   * @param bindingType the binding type to cancel
   */
  private void toggleBindings(BindingType bindingType) {
    m_bindings
        .getOrDefault(bindingType, List.of())
        .forEach(
            binding -> {
              var command = binding.command();
              if (m_scheduler.isScheduledOrRunning(command)) {
                m_scheduler.cancel(command);
              } else {
                m_scheduler.schedule(binding);
              }
            });
  }

  /**
   * Package-private for testing. Reads the signal of the trigger as it was <i>after</i> the most
   * recent call to {@link #poll()}. May be null.
   *
   * @return The most recent signal.
   */
  Signal getCachedSignal() {
    return m_cachedSignal;
  }

  /**
   * Package-private for testing. Reads the signal of the trigger as it was <i>before</i> the most
   * recent call to {@link #poll()}. May be null.
   *
   * @return The previous signal.
   */
  Signal getPreviousSignal() {
    return m_previousSignal;
  }

  /** Ensures that this trigger and all triggers it depends on are bound. */
  private void ensureBound() {
    if (m_isBoundToEventLoop) {
      // nothing to do
      return;
    }

    // Ensure dependencies are bound BEFORE binding this trigger so that the event loop polls
    // dependencies first. This guarantees that composite triggers (e.g., rising/falling edge)
    // observe up-to-date base trigger signals within the same scheduler cycle.
    for (var trigger : m_dependencyTriggers) {
      trigger.ensureBound();
    }

    m_loop.bind(m_eventLoopCallback);
    m_isBoundToEventLoop = true;
  }

  // package-private for testing
  void addBinding(BindingScope scope, BindingType bindingType, Command command) {
    // Note: we use a throwable here instead of Thread.currentThread().getStackTrace() for easier
    //       stack frame filtering and modification.
    m_bindings
        .computeIfAbsent(bindingType, _k -> new ArrayList<>())
        .add(new Binding(scope, bindingType, command, new Throwable().getStackTrace()));
    ensureBound();
  }

  private void addBinding(BindingType bindingType, Command command) {
    Command currentCommand = m_scheduler.currentCommand();
    long currentOpmode = OpModeFetcher.getFetcher().getOpModeId();

    BindingScope scope;
    if (currentCommand != null) {
      // A command is creating a binding - make it scoped to that specific command.
      // The binding will be removed when the command exits.
      scope = BindingScope.forCommand(m_scheduler, currentCommand);
    } else if (currentOpmode != 0) {
      // An opmode is currently running; scope the binding to just that mode.
      // The binding will be removed when the opmode exits.
      scope = BindingScope.forOpmode(currentOpmode);
    } else {
      // No opmode selected and no command is running; the binding is global in scope.
      scope = BindingScope.global();
    }

    addBinding(scope, bindingType, command);
  }
}
