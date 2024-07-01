// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.event;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.math.filter.Debouncer;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.function.BiFunction;
import java.util.function.BooleanSupplier;

/**
 * This class provides an easy way to link actions to active high logic signals. Each object
 * represents a digital signal to which callback actions can be bound using {@link
 * #ifHigh(Runnable)}.
 *
 * <p>BooleanEvents can easily be composed for advanced functionality using {@link
 * #and(BooleanSupplier)}, {@link #or(BooleanSupplier)}, and {@link #negate()}.
 *
 * <p>To get a new BooleanEvent that triggers when this one changes see {@link #falling()} and
 * {@link #rising()}.
 */
public class BooleanEvent implements BooleanSupplier {
  /** Poller loop. */
  protected final EventLoop m_loop;

  private final BooleanSupplier m_signal;

  /** The state of the condition in the current loop poll. Nightmare to manage. */
  private final AtomicBoolean m_state = new AtomicBoolean(false);

  /**
   * Creates a new event that is active when the condition is true.
   *
   * @param loop the loop that polls this event.
   * @param signal the digital signal represented by this object.
   */
  public BooleanEvent(EventLoop loop, BooleanSupplier signal) {
    m_loop = requireNonNullParam(loop, "loop", "BooleanEvent");
    m_signal = requireNonNullParam(signal, "signal", "BooleanEvent");
    m_state.set(m_signal.getAsBoolean());
    m_loop.bind(() -> m_state.set(m_signal.getAsBoolean()));
  }

  /**
   * Returns the state of this signal (high or low) as of the last loop poll.
   *
   * @return true for the high state, false for the low state. If the event was never polled, it
   *     returns the state at event construction.
   */
  @Override
  public final boolean getAsBoolean() {
    return m_state.get();
  }

  /**
   * Bind an action to this event.
   *
   * @param action the action to run if this event is active.
   */
  public final void ifHigh(Runnable action) {
    m_loop.bind(
        () -> {
          if (m_state.get()) {
            action.run();
          }
        });
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
    return ctor.apply(m_loop, m_state::get);
  }

  /**
   * Creates a new event that is active when this event is inactive.
   *
   * @return the new event.
   */
  public BooleanEvent negate() {
    return new BooleanEvent(m_loop, () -> !m_state.get());
  }

  /**
   * Composes this event with another event, returning a new event that is active when both events
   * are active.
   *
   * <p>The events must use the same event loop. If the events use different event loops, the
   * composed signal won't update until both loops are polled.
   *
   * @param other the event to compose with.
   * @return the new event.
   */
  public BooleanEvent and(BooleanSupplier other) {
    requireNonNullParam(other, "other", "and");
    return new BooleanEvent(m_loop, () -> m_state.get() && other.getAsBoolean());
  }

  /**
   * Composes this event with another event, returning a new event that is active when either event
   * is active.
   *
   * <p>The events must use the same event loop. If the events use different event loops, the
   * composed signal won't update until both loops are polled.
   *
   * @param other the event to compose with.
   * @return the new event.
   */
  public BooleanEvent or(BooleanSupplier other) {
    requireNonNullParam(other, "other", "or");
    return new BooleanEvent(m_loop, () -> m_state.get() || other.getAsBoolean());
  }

  /**
   * Creates a new event that triggers when this one changes from false to true.
   *
   * @return the new event.
   */
  public BooleanEvent rising() {
    return new BooleanEvent(
        m_loop,
        new BooleanSupplier() {
          private boolean m_previous = m_state.get();

          @Override
          public boolean getAsBoolean() {
            boolean present = m_state.get();
            boolean ret = !m_previous && present;
            m_previous = present;
            return ret;
          }
        });
  }

  /**
   * Creates a new event that triggers when this one changes from true to false.
   *
   * @return the event.
   */
  public BooleanEvent falling() {
    return new BooleanEvent(
        m_loop,
        new BooleanSupplier() {
          private boolean m_previous = m_state.get();

          @Override
          public boolean getAsBoolean() {
            boolean present = m_state.get();
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
   * @return The debounced event (rising edges debounced only).
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
            return m_debouncer.calculate(m_state.get());
          }
        });
  }
}
