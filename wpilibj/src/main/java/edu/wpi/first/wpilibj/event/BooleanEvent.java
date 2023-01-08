// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.event;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.math.filter.Debouncer;
import java.util.function.BiFunction;
import java.util.function.BooleanSupplier;

/**
 * This class provides an easy way to link actions to high-active logic signals. Each object
 * represents a digital signal to which callback actions can be bound using {@link
 * #ifHigh(Runnable)}.
 *
 * <p>These signals can easily be composed for advanced functionality using {@link
 * #and(BooleanSupplier)}, {@link #or(BooleanSupplier)}, {@link #negate()} etc.
 *
 * <p>To get an event that activates only when this one changes, see {@link #falling()} and {@link
 * #rising()}.
 */
public class BooleanEvent implements BooleanSupplier {
  /** Poller loop. */
  protected final EventLoop m_loop;
  /** Condition. */
  private final BooleanSupplier m_signal;

  /**
   * Creates a new event with the given signal determining whether it is active.
   *
   * @param loop the loop that polls this event
   * @param signal the digital signal represented by this object.
   */
  public BooleanEvent(EventLoop loop, BooleanSupplier signal) {
    m_loop = requireNonNullParam(loop, "loop", "BooleanEvent");
    m_signal = requireNonNullParam(signal, "signal", "BooleanEvent");
  }

  /**
   * Check the state of this signal (high or low).
   *
   * @return true for the high state, false for the low state.
   */
  @Override
  public final boolean getAsBoolean() {
    return m_signal.getAsBoolean();
  }

  /**
   * Bind an action to this event.
   *
   * @param action the action to run if this event is active.
   */
  public final void ifHigh(Runnable action) {
    m_loop.bind(
        () -> {
          if (m_signal.getAsBoolean()) {
            action.run();
          }
        });
  }

  /**
   * Get a new event that events only when this one newly changes to true.
   *
   * @return a new event representing when this one newly changes to true.
   */
  public BooleanEvent rising() {
    return new BooleanEvent(
        m_loop,
        new BooleanSupplier() {
          private boolean m_previous = m_signal.getAsBoolean();

          @Override
          public boolean getAsBoolean() {
            boolean present = m_signal.getAsBoolean();
            boolean ret = !m_previous && present;
            m_previous = present;
            return ret;
          }
        });
  }

  /**
   * Get a new event that triggers only when this one newly changes to false.
   *
   * @return a new event representing when this one newly changes to false.
   */
  public BooleanEvent falling() {
    return new BooleanEvent(
        m_loop,
        new BooleanSupplier() {
          private boolean m_previous = m_signal.getAsBoolean();

          @Override
          public boolean getAsBoolean() {
            boolean present = m_signal.getAsBoolean();
            boolean ret = m_previous && !present;
            m_previous = present;
            return ret;
          }
        });
  }

  /**
   * Creates a new debounced event from this event - it will become active when this event has been
   * active for longer than the specified period.
   *
   * @param seconds The debounce period.
   * @return The debounced event (rising edges debounced only)
   */
  public BooleanEvent debounce(double seconds) {
    return debounce(seconds, Debouncer.DebounceType.kRising);
  }

  /**
   * Creates a new debounced event from this event - it will become active when this event has been
   * active for longer than the specified period.
   *
   * @param seconds The debounce period.
   * @param type The debounce type.
   * @return The debounced event.
   */
  public BooleanEvent debounce(double seconds, Debouncer.DebounceType type) {
    return new BooleanEvent(
        m_loop,
        new BooleanSupplier() {
          private final Debouncer m_debouncer = new Debouncer(seconds, type);

          @Override
          public boolean getAsBoolean() {
            return m_debouncer.calculate(m_signal.getAsBoolean());
          }
        });
  }

  /**
   * Creates a new event that is active when this event is inactive, i.e. that acts as the negation
   * of this event.
   *
   * @return the negated event
   */
  public BooleanEvent negate() {
    return new BooleanEvent(m_loop, () -> !m_signal.getAsBoolean());
  }

  /**
   * Composes this event with another event, returning a new signal that is in the high state when
   * both signals are in the high state.
   *
   * <p>The new event will use this event's polling loop.
   *
   * @param other the event to compose with
   * @return the event that is active when both events are active
   */
  public BooleanEvent and(BooleanSupplier other) {
    requireNonNullParam(other, "other", "and");
    return new BooleanEvent(m_loop, () -> m_signal.getAsBoolean() && other.getAsBoolean());
  }

  /**
   * Composes this event with another event, returning a new signal that is high when either signal
   * is high.
   *
   * <p>The new event will use this event's polling loop.
   *
   * @param other the event to compose with
   * @return a signal that is high when either signal is high.
   */
  public BooleanEvent or(BooleanSupplier other) {
    requireNonNullParam(other, "other", "or");
    return new BooleanEvent(m_loop, () -> m_signal.getAsBoolean() || other.getAsBoolean());
  }

  /**
   * A method to "downcast" a BooleanEvent instance to a subclass (for example, to a command-based
   * version of this class).
   *
   * @param ctor a method reference to the constructor of the subclass that accepts the loop as the
   *     first parameter and the condition/signal as the second.
   * @param <T> the subclass type
   * @return an instance of the subclass.
   */
  public <T extends BooleanSupplier> T castTo(BiFunction<EventLoop, BooleanSupplier, T> ctor) {
    return ctor.apply(m_loop, m_signal);
  }
}
