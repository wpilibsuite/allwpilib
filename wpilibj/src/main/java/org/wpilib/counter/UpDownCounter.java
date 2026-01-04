// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.counter;

import org.wpilib.hardware.hal.CounterJNI;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.telemetry.TelemetryLoggable;
import org.wpilib.telemetry.TelemetryTable;

/**
 * Up Down Counter.
 *
 * <p>This class can count edges on a single digital input or count up based on an edge from one
 * digital input and down on an edge from another digital input.
 */
public class UpDownCounter implements TelemetryLoggable, AutoCloseable {
  private final int m_handle;

  /**
   * Constructs a new UpDown Counter.
   *
   * @param channel The up count source (can be null).
   * @param configuration The edge configuration.
   */
  public UpDownCounter(int channel, EdgeConfiguration configuration) {
    m_handle = CounterJNI.initializeCounter(channel, configuration.rising);

    reset();

    HAL.reportUsage("IO", channel, "UpDownCounter");
  }

  @Override
  public void close() {
    CounterJNI.freeCounter(m_handle);
  }

  /**
   * Sets the configuration for the up source.
   *
   * @param configuration The up source configuration.
   */
  public void setEdgeConfiguration(EdgeConfiguration configuration) {
    CounterJNI.setCounterEdgeConfiguration(m_handle, configuration.rising);
  }

  /** Resets the current count. */
  public final void reset() {
    CounterJNI.resetCounter(m_handle);
  }

  /**
   * Gets the current count.
   *
   * @return The current count.
   */
  public int getCount() {
    return CounterJNI.getCounter(m_handle);
  }

  @Override
  public void logTo(TelemetryTable table) {
    table.log("Count", getCount());
  }

  @Override
  public String getTelemetryType() {
    return "UpDown Counter";
  }
}
