// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.counter;

import edu.wpi.first.hal.CounterJNI;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * Tachometer.
 *
 * <p>The Tachometer class measures the time between digital pulses to determine the rotation speed
 * of a mechanism. Examples of devices that could be used with the tachometer class are a hall
 * effect sensor, break beam sensor, or optical sensor detecting tape on a shooter wheel. Unlike
 * encoders, this class only needs a single digital input.
 */
public class Tachometer implements Sendable, AutoCloseable {
  private final int m_handle;
  private int m_edgesPerRevolution = 1;

  /**
   * Constructs a new tachometer.
   *
   * @param channel The channel of the Tachometer.
   * @param configuration The edge configuration
   */
  @SuppressWarnings("this-escape")
  public Tachometer(int channel, EdgeConfiguration configuration) {
    m_handle = CounterJNI.initializeCounter(channel, configuration.rising);

    HAL.reportUsage("DIO", channel, "Tachometer");
    SendableRegistry.add(this, "Tachometer", channel);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
    CounterJNI.freeCounter(m_handle);
  }

  /**
   * Gets the tachometer period.
   *
   * @return Current period (in seconds).
   */
  public double getPeriod() {
    return CounterJNI.getCounterPeriod(m_handle);
  }

  /**
   * Gets the tachometer frequency.
   *
   * @return Current frequency (in hertz).
   */
  public double getFrequency() {
    double period = getPeriod();
    if (period == 0) {
      return 0;
    }
    return 1 / period;
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
    double period = getPeriod();
    if (period == 0) {
      return 0;
    }
    int edgesPerRevolution = getEdgesPerRevolution();
    if (edgesPerRevolution == 0) {
      return 0;
    }
    return (1.0 / edgesPerRevolution) / period;
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
   * @return True if the tachometer is stopped.
   */
  public boolean getStopped() {
    return CounterJNI.getCounterStopped(m_handle);
  }

  /**
   * Sets the maximum period before the tachometer is considered stopped.
   *
   * @param maxPeriod The max period (in seconds).
   */
  public void setMaxPeriod(double maxPeriod) {
    CounterJNI.setCounterMaxPeriod(m_handle, maxPeriod);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Tachometer");
    builder.addDoubleProperty("RPS", this::getRevolutionsPerSecond, null);
    builder.addDoubleProperty("RPM", this::getRevolutionsPerMinute, null);
  }
}
