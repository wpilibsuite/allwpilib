// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.commandsv3.button;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.math.filter.Debouncer;
import edu.wpi.first.wpilibj.commandsv3.AsyncCommand;
import edu.wpi.first.wpilibj.commandsv3.AsyncScheduler;
import edu.wpi.first.wpilibj.event.EventLoop;
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
public class AsyncTrigger implements BooleanSupplier {
  private final BooleanSupplier m_condition;
  private final EventLoop m_loop;

  /**
   * Creates a new trigger based on the given condition.
   *
   * @param loop The loop instance that polls this trigger.
   * @param condition the condition represented by this trigger
   */
  public AsyncTrigger(EventLoop loop, BooleanSupplier condition) {
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
  public AsyncTrigger(BooleanSupplier condition) {
    this(AsyncScheduler.getInstance().getDefaultButtonLoop(), condition);
  }

  /**
   * Starts the given command whenever the condition changes from `false` to `true`.
   *
   * @param command the command to start
   * @return this trigger, so calls can be chained
   */
  public AsyncTrigger onTrue(AsyncCommand command) {
    requireNonNullParam(command, "command", "onTrue");
    m_loop.bind(
        new Runnable() {
          private boolean m_pressedLast = m_condition.getAsBoolean();

          @Override
          public void run() {
            boolean pressed = m_condition.getAsBoolean();

            if (!m_pressedLast && pressed) {
              command.schedule();
            }

            m_pressedLast = pressed;
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
  public AsyncTrigger onFalse(AsyncCommand command) {
    requireNonNullParam(command, "command", "onFalse");
    m_loop.bind(
        new Runnable() {
          private boolean m_pressedLast = m_condition.getAsBoolean();

          @Override
          public void run() {
            boolean pressed = m_condition.getAsBoolean();

            if (m_pressedLast && !pressed) {
              command.schedule();
            }

            m_pressedLast = pressed;
          }
        });
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
  public AsyncTrigger whileTrue(AsyncCommand command) {
    requireNonNullParam(command, "command", "whileTrue");
    m_loop.bind(
        new Runnable() {
          private boolean m_pressedLast = m_condition.getAsBoolean();

          @Override
          public void run() {
            boolean pressed = m_condition.getAsBoolean();

            if (!m_pressedLast && pressed) {
              command.schedule();
            } else if (m_pressedLast && !pressed) {
              command.cancel();
            }

            m_pressedLast = pressed;
          }
        });
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
  public AsyncTrigger whileFalse(AsyncCommand command) {
    requireNonNullParam(command, "command", "whileFalse");
    m_loop.bind(
        new Runnable() {
          private boolean m_pressedLast = m_condition.getAsBoolean();

          @Override
          public void run() {
            boolean pressed = m_condition.getAsBoolean();

            if (m_pressedLast && !pressed) {
              command.schedule();
            } else if (!m_pressedLast && pressed) {
              command.cancel();
            }

            m_pressedLast = pressed;
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
  public AsyncTrigger toggleOnTrue(AsyncCommand command) {
    requireNonNullParam(command, "command", "toggleOnTrue");
    m_loop.bind(
        new Runnable() {
          private boolean m_pressedLast = m_condition.getAsBoolean();

          @Override
          public void run() {
            boolean pressed = m_condition.getAsBoolean();

            if (!m_pressedLast && pressed) {
              if (command.isScheduled()) {
                command.cancel();
              } else {
                command.schedule();
              }
            }

            m_pressedLast = pressed;
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
  public AsyncTrigger toggleOnFalse(AsyncCommand command) {
    requireNonNullParam(command, "command", "toggleOnFalse");
    m_loop.bind(
        new Runnable() {
          private boolean m_pressedLast = m_condition.getAsBoolean();

          @Override
          public void run() {
            boolean pressed = m_condition.getAsBoolean();

            if (m_pressedLast && !pressed) {
              if (command.isScheduled()) {
                command.cancel();
              } else {
                command.schedule();
              }
            }

            m_pressedLast = pressed;
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
  public AsyncTrigger and(BooleanSupplier trigger) {
    return new AsyncTrigger(m_loop, () -> m_condition.getAsBoolean() && trigger.getAsBoolean());
  }

  /**
   * Composes two triggers with logical OR.
   *
   * @param trigger the condition to compose with
   * @return A trigger which is active when either component trigger is active.
   */
  public AsyncTrigger or(BooleanSupplier trigger) {
    return new AsyncTrigger(m_loop, () -> m_condition.getAsBoolean() || trigger.getAsBoolean());
  }

  /**
   * Creates a new trigger that is active when this trigger is inactive, i.e. that acts as the
   * negation of this trigger.
   *
   * @return the negated trigger
   */
  public AsyncTrigger negate() {
    return new AsyncTrigger(m_loop, () -> !m_condition.getAsBoolean());
  }

  /**
   * Creates a new debounced trigger from this trigger - it will become active when this trigger has
   * been active for longer than the specified period.
   *
   * @param seconds The debounce period.
   * @return The debounced trigger (rising edges debounced only)
   */
  public AsyncTrigger debounce(double seconds) {
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
  public AsyncTrigger debounce(double seconds, Debouncer.DebounceType type) {
    return new AsyncTrigger(
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
