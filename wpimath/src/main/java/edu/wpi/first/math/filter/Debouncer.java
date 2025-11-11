// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.filter;

import edu.wpi.first.math.MathSharedStore;
import java.util.OptionalDouble;

/**
 * A simple debounce filter for boolean streams. Requires that the boolean change value from
 * baseline for a specified period of time before the filtered value changes.
 */
public class Debouncer {
  /** Type of debouncing to perform. */
  public enum DebounceType {
    /** Rising edge. */
    kRising,
    /** Falling edge. */
    kFalling,
    /** Both rising and falling edges. */
    kBoth
  }

  private double m_debounceTimeSeconds;
  private DebounceType m_debounceType;
  private boolean m_baseline;

  private OptionalDouble m_prevTimeSeconds;

  /**
   * Creates a new Debouncer.
   *
   * @param debounceTime The number of seconds the value must change from baseline for the filtered
   *     value to change.
   * @param type Which type of state change the debouncing will be performed on.
   */
  public Debouncer(double debounceTime, DebounceType type) {
    m_debounceTimeSeconds = debounceTime;
    m_debounceType = type;

    resetTimer();

    m_baseline = m_debounceType == DebounceType.kFalling;
  }

  /**
   * Creates a new Debouncer. Baseline value defaulted to "false."
   *
   * @param debounceTime The number of seconds the value must change from baseline for the filtered
   *     value to change.
   */
  public Debouncer(double debounceTime) {
    this(debounceTime, DebounceType.kRising);
  }

  private void resetTimer() {
    m_prevTimeSeconds = OptionalDouble.of(MathSharedStore.getTimestamp());
  }

  private boolean hasElapsed() {
    return m_prevTimeSeconds.isEmpty()
        || MathSharedStore.getTimestamp() - m_prevTimeSeconds.getAsDouble()
            >= m_debounceTimeSeconds;
  }

  /**
   * Applies the debouncer to the input stream.
   *
   * @param input The current value of the input stream.
   * @return The debounced value of the input stream.
   */
  public boolean calculate(boolean input) {
    if (input == m_baseline) {
      resetTimer();
    }

    if (hasElapsed()) {
      if (m_debounceType == DebounceType.kBoth) {
        m_baseline = input;
        resetTimer();
      }
      return input;
    } else {
      return m_baseline;
    }
  }

  /**
   * Resets the debouncer such that it will now behave as if enough time has passed, even if the
   * actual elapsed time is below the limit. This is helpful if you want to clear the internal
   * "memory" or state.
   */
  public void reset() {
    m_prevTimeSeconds = OptionalDouble.empty();
  }

  /**
   * Sets the time to debounce.
   *
   * @param time The number of seconds the value must change from baseline for the filtered value to
   *     change.
   */
  public void setDebounceTime(double time) {
    m_debounceTimeSeconds = time;
  }

  /**
   * Gets the time to debounce.
   *
   * @return The number of seconds the value must change from baseline for the filtered value to
   *     change.
   */
  public double getDebounceTime() {
    return m_debounceTimeSeconds;
  }

  /**
   * Sets the debounce type.
   *
   * @param type Which type of state change the debouncing will be performed on.
   */
  public void setDebounceType(DebounceType type) {
    m_debounceType = type;

    m_baseline = m_debounceType == DebounceType.kFalling;
  }

  /**
   * Gets the debounce type.
   *
   * @return Which type of state change the debouncing will be performed on.
   */
  public DebounceType getDebounceType() {
    return m_debounceType;
  }
}
