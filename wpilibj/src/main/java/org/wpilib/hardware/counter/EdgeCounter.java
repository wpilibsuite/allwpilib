// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.counter;

import org.wpilib.hardware.hal.CounterJNI;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.telemetry.TelemetryLoggable;
import org.wpilib.telemetry.TelemetryTable;

/** Counts rising or falling edges on a single digital input. */
public class EdgeCounter implements TelemetryLoggable, AutoCloseable {
  private final int m_handle;

  /**
   * Constructs a new edge counter.
   *
   * @param channel The DIO channel.
   * @param configuration The edge configuration.
   */
  public EdgeCounter(int channel, EdgeConfiguration configuration) {
    m_handle = CounterJNI.initializeCounter(channel, configuration.rising);

    reset();

    HAL.reportUsage("IO", channel, "EdgeCounter");
  }

  @Override
  public void close() {
    CounterJNI.freeCounter(m_handle);
  }

  /**
   * Sets which edge of the digital input is counted.
   *
   * @param configuration The edge configuration.
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
    return "Edge Counter";
  }
}
