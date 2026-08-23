// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.counter;

import org.wpilib.hardware.hal.CounterJNI;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.internal.UnitTelemetry;
import org.wpilib.telemetry.TelemetryLoggable;
import org.wpilib.telemetry.TelemetryTable;
import org.wpilib.units.Units;

/**
 * Tachometer.
 *
 * <p>The Tachometer class measures the rate of digital pulses to determine the rotation velocity of
 * a mechanism. Examples of devices that could be used with the tachometer class are a hall effect
 * sensor, break beam sensor, or optical sensor detecting tape on a shooter wheel. Unlike encoders,
 * this class only needs a single digital input.
 */
public class Tachometer implements TelemetryLoggable, AutoCloseable {
  private final int m_handle;
  private int m_edgesPerRevolution = 1;

  /**
   * Constructs a new tachometer.
   *
   * @param channel The channel of the Tachometer.
   * @param configuration The edge configuration
   */
  public Tachometer(int channel, EdgeConfiguration configuration) {
    m_handle = CounterJNI.initializeCounter(channel, configuration.rising);

    HAL.reportUsage("IO", channel, "Tachometer");
  }

  @Override
  public void close() {
    CounterJNI.freeCounter(m_handle);
  }

  /**
   * Gets the tachometer frequency.
   *
   * @return Current frequency (in hertz).
   */
  public double getFrequency() {
    return CounterJNI.getCounterRate(m_handle);
  }

  /**
   * Sets the time window used to calculate the tachometer rate.
   *
   * @param windowMilliseconds The rate calculation window in milliseconds. Valid values are 5
   *     through 255. The default is 50.
   */
  public void setRateWindow(int windowMilliseconds) {
    CounterJNI.setCounterRateWindow(m_handle, windowMilliseconds);
  }

  /**
   * Gets the number of edges per revolution.
   *
   * @return Edges per revolution.
   */
  public int getEdgesPerRevolution() {
    return m_edgesPerRevolution;
  }

  /**
   * Sets the number of edges per revolution.
   *
   * @param edgesPerRevolution Edges per revolution.
   */
  public void setEdgesPerRevolution(int edgesPerRevolution) {
    m_edgesPerRevolution = edgesPerRevolution;
  }

  /**
   * Gets the current tachometer revolutions per second.
   *
   * <p>setEdgesPerRevolution must be set with a non 0 value for this to return valid values.
   *
   * @return Current RPS.
   */
  public double getRevolutionsPerSecond() {
    int edgesPerRevolution = getEdgesPerRevolution();
    if (edgesPerRevolution == 0) {
      return 0;
    }
    return getFrequency() / edgesPerRevolution;
  }

  /**
   * Gets the current tachometer revolutions per minute.
   *
   * <p>setEdgesPerRevolution must be set with a non 0 value for this to return valid values.
   *
   * @return Current RPM.
   */
  public double getRevolutionsPerMinute() {
    return getRevolutionsPerSecond() * 60;
  }

  /**
   * Gets if the tachometer is stopped.
   *
   * <p>The tachometer is stopped when its current frequency is zero.
   *
   * @return True if the tachometer is stopped.
   */
  public boolean getStopped() {
    return CounterJNI.getCounterStopped(m_handle);
  }

  @Override
  public void logTo(TelemetryTable table) {
    UnitTelemetry.log(table, "RPM", getRevolutionsPerMinute(), Units.RotationsPerMinute);
  }

  @Override
  public String getTelemetryType() {
    return "Tachometer";
  }
}
