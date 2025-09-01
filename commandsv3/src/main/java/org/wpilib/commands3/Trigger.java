// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static edu.wpi.first.units.Units.Seconds;
import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.math.filter.Debouncer;
import edu.wpi.first.units.measure.Time;
import edu.wpi.first.wpilibj.event.EventLoop;
import java.util.ArrayList;
import java.util.EnumMap;
import java.util.List;
import java.util.Map;
import java.util.function.BooleanSupplier;

/**
 * Triggers allow users to specify conditions for when commands should run. Triggers can be set up
 * to read from joystick and controller buttons (eg {@link
 * org.wpilib.commands3.button.CommandXboxController#x()}) or be customized to read sensor values or
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
  private Signal m_previousSignal;
  private final Map<BindingType, List<Binding>> m_bindings = new EnumMap<>(BindingType.class);
  private final Runnable m_eventLoopCallback = this::poll;

  /**
   * Represents the state of a signal: high or low. Used instead of a boolean for nullity on the
   * first run, when the previous signal value is undefined and unknown.
   */
  private enum Signal {
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
    m_scheduler = requireNonNullParam(scheduler, "scheduler", "Trigger");
    m_loop = scheduler.getDefaultEventLoop();
    m_condition = requireNonNullParam(condition, "condition", "Trigger");
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

  @Override
  public boolean getAsBoolean() {
    return m_condition.getAsBoolean();
  }

  /**
   * Composes two triggers with logical AND.
   *
   * @param trigger the condition to compose with
   * @return A trigger which is active when both component triggers are active.
   */
  public Trigger and(BooleanSupplier trigger) {
    return new Trigger(
        m_scheduler, m_loop, () -> m_condition.getAsBoolean() && trigger.getAsBoolean());
  }

  /**
   * Composes two triggers with logical OR.
   *
   * @param trigger the condition to compose with
   * @return A trigger which is active when either component trigger is active.
   */
  public Trigger or(BooleanSupplier trigger) {
    return new Trigger(
        m_scheduler, m_loop, () -> m_condition.getAsBoolean() || trigger.getAsBoolean());
  }

  /**
   * Creates a new trigger that is active when this trigger is inactive, i.e. that acts as the
   * negation of this trigger.
   *
   * @return the negated trigger
   */
  public Trigger negate() {
    return new Trigger(m_scheduler, m_loop, () -> !m_condition.getAsBoolean());
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
    return new Trigger(m_scheduler, m_loop, () -> debouncer.calculate(m_condition.getAsBoolean()));
  }

  private void poll() {
    // Clear bindings that no longer need to run
    // This should always be checked, regardless of signal change, since bindings may be scoped
    // and those scopes may become inactive.
    clearStaleBindings();

    var signal = readSignal();

    if (signal == m_previousSignal) {
      // No change in the signal. Nothing to do
      return;
    }

    if (signal == Signal.HIGH) {
      // Signal is now high when it wasn't before - a rising edge
      scheduleBindings(BindingType.SCHEDULE_ON_RISING_EDGE);
      scheduleBindings(BindingType.RUN_WHILE_HIGH);
      cancelBindings(BindingType.RUN_WHILE_LOW);
      toggleBindings(BindingType.TOGGLE_ON_RISING_EDGE);
    }

    if (signal == Signal.LOW) {
      // Signal is now low when it wasn't before - a falling edge
      scheduleBindings(BindingType.SCHEDULE_ON_FALLING_EDGE);
      scheduleBindings(BindingType.RUN_WHILE_LOW);
      cancelBindings(BindingType.RUN_WHILE_HIGH);
      toggleBindings(BindingType.TOGGLE_ON_FALLING_EDGE);
    }

    m_previousSignal = signal;
  }

  private Signal readSignal() {
    if (m_condition.getAsBoolean()) {
      return Signal.HIGH;
    } else {
      return Signal.LOW;
    }
  }

  /** Removes bindings in inactive scopes. Running commands tied to those bindings are cancelled. */
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

  // package-private for testing
  void addBinding(BindingScope scope, BindingType bindingType, Command command) {
    // Note: we use a throwable here instead of Thread.currentThread().getStackTrace() for easier
    //       stack frame filtering and modification.
    m_bindings
        .computeIfAbsent(bindingType, _k -> new ArrayList<>())
        .add(new Binding(scope, bindingType, command, new Throwable().getStackTrace()));

    // Ensure this trigger is bound to the event loop. NOP if already bound
    m_loop.bind(m_eventLoopCallback);
  }

  private void addBinding(BindingType bindingType, Command command) {
    BindingScope scope =
        switch (m_scheduler.currentCommand()) {
          case Command c -> {
            // A command is creating a binding - make it scoped to that specific command
            yield BindingScope.forCommand(m_scheduler, c);
          }
          case null -> {
            // Creating a binding outside a command - it's global in scope
            yield BindingScope.global();
          }
        };

    addBinding(scope, bindingType, command);
  }
}
