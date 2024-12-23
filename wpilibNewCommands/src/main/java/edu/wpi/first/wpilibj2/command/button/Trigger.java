// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.math.filter.Debouncer;
import edu.wpi.first.wpilibj.event.EventLoop;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import java.util.function.BooleanSupplier;

/**
 * This class provides an easy way to link commands to conditions.
 *
 * <p>It is very easy to link a button to a command. For instance, you could link the trigger button
 * of a joystick to a "score" command.
 *
 * <p>Triggers can easily be composed for advanced functionality using the {@link
 * #and(BooleanSupplier)}, {@link #or(BooleanSupplier)}, {@link #negate()} operators.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class Trigger implements BooleanSupplier {
  /** Functional interface for the body of a trigger binding. */
  @FunctionalInterface
  private interface BindingBody {
    /**
     * Executes the body of the binding.
     *
     * @param previous The previous state of the condition.
     * @param current The current state of the condition.
     */
    void run(boolean previous, boolean current);
  }

  private final BooleanSupplier m_condition;
  private final EventLoop m_loop;

  /**
   * Creates a new trigger based on the given condition.
   *
   * @param loop The loop instance that polls this trigger.
   * @param condition the condition represented by this trigger
   */
  public Trigger(EventLoop loop, BooleanSupplier condition) {
    m_loop = requireNonNullParam(loop, "loop", "Trigger");
    m_condition = requireNonNullParam(condition, "condition", "Trigger");
  }

  /**
   * Creates a new trigger based on the given condition.
   *
   * <p>Polled by the default scheduler button loop.
   *
   * @param condition the condition represented by this trigger
   */
  public Trigger(BooleanSupplier condition) {
    this(CommandScheduler.getInstance().getDefaultButtonLoop(), condition);
  }

  /**
   * Adds a binding to the EventLoop.
   *
   * @param body The body of the binding to add.
   */
  private void addBinding(BindingBody body) {
    m_loop.bind(
        new Runnable() {
          private boolean m_previous = m_condition.getAsBoolean();

          @Override
          public void run() {
            boolean current = m_condition.getAsBoolean();

            body.run(m_previous, current);

            m_previous = current;
          }
        });
  }

  /**
   * Starts the command when the condition changes.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  public Trigger onChange(Command command) {
    requireNonNullParam(command, "command", "onChange");
    addBinding(
        (previous, current) -> {
          if (previous != current) {
            command.schedule();
          }
        });
    return this;
  }

  /**
   * Starts the given command whenever the condition changes from `false` to `true`.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  public Trigger onTrue(Command command) {
    requireNonNullParam(command, "command", "onTrue");
    addBinding(
        (previous, current) -> {
          if (!previous && current) {
            command.schedule();
          }
        });
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
    addBinding(
        (previous, current) -> {
          if (previous && !current) {
            command.schedule();
          }
        });
    return this;
  }

  /**
   * Starts the given command when the condition changes to `true` and cancels it when the condition
   * changes to `false`.
   *
   * <p>Doesn't re-start the command if it ends while the condition is still `true`. If the command
   * should restart, see {@link edu.wpi.first.wpilibj2.command.RepeatCommand}.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  public Trigger whileTrue(Command command) {
    requireNonNullParam(command, "command", "whileTrue");
    addBinding(
        (previous, current) -> {
          if (!previous && current) {
            command.schedule();
          } else if (previous && !current) {
            command.cancel();
          }
        });
    return this;
  }

  /**
   * Starts the given command when the condition changes to `false` and cancels it when the
   * condition changes to `true`.
   *
   * <p>Doesn't re-start the command if it ends while the condition is still `false`. If the command
   * should restart, see {@link edu.wpi.first.wpilibj2.command.RepeatCommand}.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  public Trigger whileFalse(Command command) {
    requireNonNullParam(command, "command", "whileFalse");
    addBinding(
        (previous, current) -> {
          if (previous && !current) {
            command.schedule();
          } else if (!previous && current) {
            command.cancel();
          }
        });
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
    addBinding(
        (previous, current) -> {
          if (!previous && current) {
            if (command.isScheduled()) {
              command.cancel();
            } else {
              command.schedule();
            }
          }
        });
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
    addBinding(
        (previous, current) -> {
          if (previous && !current) {
            if (command.isScheduled()) {
              command.cancel();
            } else {
              command.schedule();
            }
          }
        });
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
    return new Trigger(m_loop, () -> m_condition.getAsBoolean() && trigger.getAsBoolean());
  }

  /**
   * Composes two triggers with logical OR.
   *
   * @param trigger the condition to compose with
   * @return A trigger which is active when either component trigger is active.
   */
  public Trigger or(BooleanSupplier trigger) {
    return new Trigger(m_loop, () -> m_condition.getAsBoolean() || trigger.getAsBoolean());
  }

  /**
   * Creates a new trigger that is active when this trigger is inactive, i.e. that acts as the
   * negation of this trigger.
   *
   * @return the negated trigger
   */
  public Trigger negate() {
    return new Trigger(m_loop, () -> !m_condition.getAsBoolean());
  }

  /**
   * Creates a new debounced trigger from this trigger - it will become active when this trigger has
   * been active for longer than the specified period.
   *
   * @param seconds The debounce period.
   * @return The debounced trigger (rising edges debounced only)
   */
  public Trigger debounce(double seconds) {
    return debounce(seconds, Debouncer.DebounceType.kRising);
  }

  /**
   * Creates a new debounced trigger from this trigger - it will become active when this trigger has
   * been active for longer than the specified period.
   *
   * @param seconds The debounce period.
   * @param type The debounce type.
   * @return The debounced trigger.
   */
  public Trigger debounce(double seconds, Debouncer.DebounceType type) {
    return new Trigger(
        m_loop,
        new BooleanSupplier() {
          final Debouncer m_debouncer = new Debouncer(seconds, type);

          @Override
          public boolean getAsBoolean() {
            return m_debouncer.calculate(m_condition.getAsBoolean());
          }
        });
  }
}
