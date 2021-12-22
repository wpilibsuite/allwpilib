// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.util.sendable.EnumHelper;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;

/**
 * A simple debounce filter for boolean streams. Requires that the boolean change value from
 * baseline for a specified period of time before the filtered value changes.
 */
public class Debouncer implements Sendable {
  public enum DebounceType {
    kRising,
    kFalling,
    kBoth
  }

  private final Timer m_timer = new Timer();
  private double m_debounceTime;
  private DebounceType m_debounceType;
  private boolean m_baseline;

  /**
   * Creates a new Debouncer.
   *
   * @param debounceTime The number of seconds the value must change from baseline for the filtered
   *     value to change.
   * @param type Which type of state change the debouncing will be performed on.
   */
  public Debouncer(double debounceTime, DebounceType type) {
    m_debounceTime = debounceTime;
    m_debounceType = type;
    m_timer.start();

    switch (m_debounceType) {
      case kBoth: // fall-through
      case kRising:
        m_baseline = false;
        break;
      case kFalling:
        m_baseline = true;
        break;
      default:
        throw new IllegalArgumentException("Invalid debounce type!");
    }
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

  /**
   * Applies the debouncer to the input stream.
   *
   * @param input The current value of the input stream.
   * @return The debounced value of the input stream.
   */
  public boolean calculate(boolean input) {
    if (input == m_baseline) {
      m_timer.reset();
    }

    if (m_timer.hasElapsed(m_debounceTime)) {
      if (m_debounceType == DebounceType.kBoth) {
        m_baseline = input;
        m_timer.reset();
      }
      return input;
    } else {
      return m_baseline;
    }
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder
        .addDoubleProperty(
            "debounceTime", () -> m_debounceTime, debounceTime -> m_debounceTime = debounceTime)
        .addStringProperty(
            "debounceType",
            () -> m_debounceType.name(),
            type -> m_debounceType = EnumHelper.enumFromString(type, DebounceType.kRising));
  }
}
