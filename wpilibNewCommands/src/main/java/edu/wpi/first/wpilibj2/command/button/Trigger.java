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
  /**
   * Enum specifying the initial state to use for a binding. This impacts whether or not the binding
   * will be triggered immediately.
   */
  public enum InitialState {
    /**
     * Indicates the binding should use false as the initial value. This causes a rising edge at the
     * start if and only if the condition starts true.
     */
    kFalse,
    /**
     * Indicates the binding should use true as the initial value. This causes a falling edge at the
     * start if and only if the condition starts false.
     */
    kTrue,
    /**
     * Indicates the binding should use the trigger's condition as the initial value. This never
     * causes an edge at the start. This is the default behavior.
     */
    kCondition,
    /**
     * Indicates the binding should use the negated trigger's condition as the initial value. This
     * always causes an edge at the start. Rising or falling depends on if the condition starts true
     * or false, respectively.
     */
    kNegCondition;
  }

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
   * Gets the initial state for a binding based on an initial state policy.
   *
   * @param initialState Initial state policy.
   * @return The initial state to use.
   */
  private boolean getInitialState(InitialState initialState) {
    return switch (initialState) {
      case kFalse -> false;
      case kTrue -> true;
      case kCondition -> m_condition.getAsBoolean();
      case kNegCondition -> !m_condition.getAsBoolean();
    };
  }

  /**
   * Adds a binding to the EventLoop.
   *
   * @param body The body of the binding to add.
   */
  private void addBinding(BindingBody body, InitialState initialState) {
    m_loop.bind(
        new Runnable() {
          private boolean m_previous = getInitialState(initialState);

          @Override
          public void run() {
            boolean current = m_condition.getAsBoolean();

            body.run(m_previous, current);

            m_previous = current;
          }
        });
  }

  /**
   * Starts the command when the condition changes. The command is never started immediately.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  public Trigger onChange(Command command) {
    return onChange(command, InitialState.kCondition);
  }

  /**
   * Starts the command when the condition changes.
   *
   * @param command the command to start
   * @param initialState the initial state to use, kCondition (no initial edge) by default
   * @return this trigger, so calls can be chained
   */
  public Trigger onChange(Command command, InitialState initialState) {
    requireNonNullParam(command, "command", "onChange");
    addBinding(
        (previous, current) -> {
          if (previous != current) {
            command.schedule();
          }
        },
        initialState);
    return this;
  }

  /**
   * Starts the given command whenever the condition changes from `false` to `true`. The command is
   * never started immediately.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  public Trigger onTrue(Command command) {
    return onTrue(command, InitialState.kCondition);
  }

  /**
   * Starts the given command whenever the condition changes from `false` to `true`.
   *
   * @param command the command to start
   * @param initialState the initial state to use, kCondition (no initial edge) by default
   * @return this trigger, so calls can be chained
   */
  public Trigger onTrue(Command command, InitialState initialState) {
    requireNonNullParam(command, "command", "onTrue");
    addBinding(
        (previous, current) -> {
          if (!previous && current) {
            command.schedule();
          }
        },
        initialState);
    return this;
  }

  /**
   * Starts the given command whenever the condition changes from `true` to `false`. The command is
   * never started immediately.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  public Trigger onFalse(Command command) {
    return onFalse(command, InitialState.kCondition);
  }

  /**
   * Starts the given command whenever the condition changes from `true` to `false`.
   *
   * @param command the command to start
   * @param initialState the initial state to use, kCondition (no initial edge) by default
   * @return this trigger, so calls can be chained
   */
  public Trigger onFalse(Command command, InitialState initialState) {
    requireNonNullParam(command, "command", "onFalse");
    addBinding(
        (previous, current) -> {
          if (previous && !current) {
            command.schedule();
          }
        },
        initialState);
    return this;
  }

  /**
   * Starts the given command when the condition changes to `true` and cancels it when the condition
   * changes to `false`. The command is never started immediately.
   *
   * <p>Doesn't re-start the command if it ends while the condition is still `true`. If the command
   * should restart, see {@link edu.wpi.first.wpilibj2.command.RepeatCommand}.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  public Trigger whileTrue(Command command) {
    return whileTrue(command, InitialState.kCondition);
  }

  /**
   * Starts the given command when the condition changes to `true` and cancels it when the condition
   * changes to `false`.
   *
   * <p>Doesn't re-start the command if it ends while the condition is still `true`. If the command
   * should restart, see {@link edu.wpi.first.wpilibj2.command.RepeatCommand}.
   *
   * @param command the command to start
   * @param initialState the initial state to use, kCondition (no initial edge) by default
   * @return this trigger, so calls can be chained
   */
  public Trigger whileTrue(Command command, InitialState initialState) {
    requireNonNullParam(command, "command", "whileTrue");
    addBinding(
        (previous, current) -> {
          if (!previous && current) {
            command.schedule();
          } else if (previous && !current) {
            command.cancel();
          }
        },
        initialState);
    return this;
  }

  /**
   * Starts the given command when the condition changes to `false` and cancels it when the
   * condition changes to `true`. The command is never started immediately.
   *
   * <p>Doesn't re-start the command if it ends while the condition is still `false`. If the command
   * should restart, see {@link edu.wpi.first.wpilibj2.command.RepeatCommand}.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  public Trigger whileFalse(Command command) {
    return whileFalse(command, InitialState.kCondition);
  }

  /**
   * Starts the given command when the condition changes to `false` and cancels it when the
   * condition changes to `true`.
   *
   * <p>Doesn't re-start the command if it ends while the condition is still `false`. If the command
   * should restart, see {@link edu.wpi.first.wpilibj2.command.RepeatCommand}.
   *
   * @param command the command to start
   * @param initialState the initial state to use, kCondition (no initial edge) by default
   * @return this trigger, so calls can be chained
   */
  public Trigger whileFalse(Command command, InitialState initialState) {
    requireNonNullParam(command, "command", "whileFalse");
    addBinding(
        (previous, current) -> {
          if (previous && !current) {
            command.schedule();
          } else if (!previous && current) {
            command.cancel();
          }
        },
        initialState);
    return this;
  }

  /**
   * Toggles a command when the condition changes from `false` to `true`. The command is never
   * toggled immediately.
   *
   * @param command the command to toggle
   * @return this trigger, so calls can be chained
   */
  public Trigger toggleOnTrue(Command command) {
    return toggleOnTrue(command, InitialState.kCondition);
  }

  /**
   * Toggles a command when the condition changes from `false` to `true`.
   *
   * @param command the command to toggle
   * @param initialState the initial state to use, kCondition (no initial edge) by default
   * @return this trigger, so calls can be chained
   */
  public Trigger toggleOnTrue(Command command, InitialState initialState) {
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
        },
        initialState);
    return this;
  }

  /**
   * Toggles a command when the condition changes from `true` to `false`. The command is never
   * toggled immediately.
   *
   * @param command the command to toggle
   * @return this trigger, so calls can be chained
   */
  public Trigger toggleOnFalse(Command command) {
    return toggleOnFalse(command, InitialState.kCondition);
  }

  /**
   * Toggles a command when the condition changes from `true` to `false`.
   *
   * @param command the command to toggle
   * @param initialState the initial state to use, kCondition (no initial edge) by default
   * @return this trigger, so calls can be chained
   */
  public Trigger toggleOnFalse(Command command, InitialState initialState) {
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
        },
        initialState);
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
