// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.commandsv3.button;

import static edu.wpi.first.units.Units.Seconds;
import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.math.filter.Debouncer;
import edu.wpi.first.units.measure.Time;
import edu.wpi.first.wpilibj.commandsv3.Command;
import edu.wpi.first.wpilibj.commandsv3.Scheduler;
import edu.wpi.first.wpilibj.event.EventLoop;
import java.util.ArrayList;
import java.util.EnumMap;
import java.util.List;
import java.util.Map;
import java.util.function.BooleanSupplier;

/**
 * This class provides an easy way to link commands to conditions.
 *
 * <p>It is very easy to link a button to a command. For instance, you could link the trigger button
 * of a joystick to a "score" command.
 *
 * <p>Triggers can easily be composed for advanced functionality using the {@link
 * #and(BooleanSupplier)}, {@link #or(BooleanSupplier)}, {@link #negate()} operators.
 */
public class Trigger implements BooleanSupplier {
  private final BooleanSupplier m_condition;
  private final EventLoop m_loop;
  private final Scheduler scheduler;
  private Signal m_previousSignal = null;
  private final Map<BindingType, List<Command>> m_bindings = new EnumMap<>(BindingType.class);
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

  private enum BindingType {
    /**
     * Schedules (forks) a command on a rising edge signal. The command will run until it completes
     * or is interrupted by another command requiring the same resources.
     */
    SCHEDULE_ON_RISING_EDGE,
    /**
     * Schedules (forks) a command on a falling edge signal. The command will run until it completes
     * or is interrupted by another command requiring the same resources.
     */
    SCHEDULE_ON_FALLING_EDGE,
    /**
     * Schedules (forks) a command on a rising edge signal. If the command is still running on the
     * next rising edge, it will be cancelled then; otherwise, it will be scheduled again.
     */
    TOGGLE_ON_RISING_EDGE,
    /**
     * Schedules (forks) a command on a falling edge signal. If the command is still running on the
     * next falling edge, it will be cancelled then; otherwise, it will be scheduled again.
     */
    TOGGLE_ON_FALLING_EDGE,
    /**
     * Schedules a command on a rising edge signal. If the command is still running on the next
     * falling edge, it will be cancelled then - unlike {@link #SCHEDULE_ON_RISING_EDGE}, which
     * would allow it to continue to run.
     */
    RUN_WHILE_HIGH,
    /**
     * Schedules a command on a falling edge signal. If the command is still running on the next
     * rising edge, it will be cancelled then - unlike {@link #SCHEDULE_ON_FALLING_EDGE}, which
     * would allow it to continue to run.
     */
    RUN_WHILE_LOW
  }

  public Trigger(Scheduler scheduler, BooleanSupplier condition) {
    this.scheduler = requireNonNullParam(scheduler, "scheduler", "Trigger");
    this.m_loop = scheduler.getDefaultEventLoop();
    this.m_condition = requireNonNullParam(condition, "condition", "Trigger");
  }

  /**
   * Creates a new trigger based on the given condition.
   *
   * <p>Polled by the default scheduler button loop.
   *
   * @param condition the condition represented by this trigger
   */
  public Trigger(BooleanSupplier condition) {
    this(Scheduler.getInstance(), condition);
  }

  // For internal use
  private Trigger(Scheduler scheduler, EventLoop loop, BooleanSupplier condition) {
    this.scheduler = scheduler;
    m_loop = loop;
    m_condition = condition;
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
        scheduler, m_loop, () -> m_condition.getAsBoolean() && trigger.getAsBoolean());
  }

  /**
   * Composes two triggers with logical OR.
   *
   * @param trigger the condition to compose with
   * @return A trigger which is active when either component trigger is active.
   */
  public Trigger or(BooleanSupplier trigger) {
    return new Trigger(
        scheduler, m_loop, () -> m_condition.getAsBoolean() || trigger.getAsBoolean());
  }

  /**
   * Creates a new trigger that is active when this trigger is inactive, i.e. that acts as the
   * negation of this trigger.
   *
   * @return the negated trigger
   */
  public Trigger negate() {
    return new Trigger(scheduler, m_loop, () -> !m_condition.getAsBoolean());
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
   * @param seconds The debounce period.
   * @param type The debounce type.
   * @return The debounced trigger.
   */
  public Trigger debounce(Time duration, Debouncer.DebounceType type) {
    return new Trigger(
        scheduler,
        m_loop,
        new BooleanSupplier() {
          final Debouncer m_debouncer = new Debouncer(duration.in(Seconds), type);

          @Override
          public boolean getAsBoolean() {
            return m_debouncer.calculate(m_condition.getAsBoolean());
          }
        });
  }

  private void poll() {
    var signal = signal();

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

  private Signal signal() {
    if (m_condition.getAsBoolean()) {
      return Signal.HIGH;
    } else {
      return Signal.LOW;
    }
  }

  /**
   * Schedules all commands bound to the given binding type.
   *
   * @param bindingType the binding type to schedule
   */
  private void scheduleBindings(BindingType bindingType) {
    m_bindings.getOrDefault(bindingType, List.of()).forEach(scheduler::schedule);
  }

  /**
   * Cancels all commands bound to the given binding type.
   *
   * @param bindingType the binding type to cancel
   */
  private void cancelBindings(BindingType bindingType) {
    m_bindings.getOrDefault(bindingType, List.of()).forEach(scheduler::cancel);
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
            command -> {
              if (scheduler.isScheduledOrRunning(command)) {
                scheduler.cancel(command);
              } else {
                scheduler.schedule(command);
              }
            });
  }

  private void addBinding(BindingType bindingType, Command command) {
    m_bindings.computeIfAbsent(bindingType, _k -> new ArrayList<>()).add(command);

    // Ensure this trigger is bound to the event loop. NOP if already bound
    m_loop.bind(m_eventLoopCallback);
  }
}
