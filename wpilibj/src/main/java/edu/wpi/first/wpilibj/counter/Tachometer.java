// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.counter;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.hal.CounterJNI;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;
import edu.wpi.first.wpilibj.DigitalSource;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/** Tachometer. */
public class Tachometer implements Sendable, AutoCloseable {
  private final DigitalSource m_source;
  private final int m_handle;
  private int m_edgesPerRevolution = 1;
  private double m_maxPeriod;

  /**
   * Constructs a new tachometer.
   *
   * @param source The source.
   */
  public Tachometer(DigitalSource source) {
    m_source = requireNonNullParam(source, "source", "Tachometer");

    ByteBuffer index = ByteBuffer.allocateDirect(4);
    // set the byte order
    index.order(ByteOrder.LITTLE_ENDIAN);
    m_handle = CounterJNI.initializeCounter(CounterJNI.TWO_PULSE, index.asIntBuffer());

    CounterJNI.setCounterUpSource(
        m_handle, source.getPortHandleForRouting(), source.getAnalogTriggerTypeForRouting());
    CounterJNI.setCounterUpSourceEdge(m_handle, true, false);

    int intIndex = index.getInt();
    HAL.report(tResourceType.kResourceType_Counter, intIndex + 1);
    SendableRegistry.addLW(this, "Tachometer", intIndex);
  }

  @Override
  public void close() throws Exception {
    SendableRegistry.remove(this);
    CounterJNI.freeCounter(m_handle);
    CounterJNI.suppressUnused(m_source);
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
    return period;
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
   * Gets the current tachometer revolution per minute.
   *
   * <p>setEdgesPerRevolution must be set with a non 0 value for this to return valid values.
   *
   * @return Current RPM.
   */
  public double getRevolutionsPerMinute() {
    double period = getPeriod();
    if (period == 0) {
      return 0;
    }
    int edgesPerRevolution = getEdgesPerRevolution();
    if (edgesPerRevolution == 0) {
      return 0;
    }
    return ((1.0 / edgesPerRevolution) / period) * 60;
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
   * Gets the number of samples to average.
   *
   * @return Samples to average.
   */
  public int getSamplesToAverage() {
    return CounterJNI.getCounterSamplesToAverage(m_handle);
  }

  /**
   * Sets the number of samples to average.
   *
   * @param samplesToAverage Samples to average.
   */
  public void setSamplesToAverage(int samplesToAverage) {
    CounterJNI.setCounterSamplesToAverage(m_handle, samplesToAverage);
  }

  /**
   * Sets the maximum period before the tachometer is considered stopped.
   *
   * @param maxPeriod The max period (in seconds).
   */
  public void setMaxPeriod(double maxPeriod) {
    m_maxPeriod = maxPeriod;
    CounterJNI.setCounterMaxPeriod(m_handle, m_maxPeriod);
  }

  /**
   * Sets if to update when empty.
   *
   * @param updateWhenEmpty Update when empty if true.
   */
  public void setUpdateWhenEmpty(boolean updateWhenEmpty) {
    CounterJNI.setCounterUpdateWhenEmpty(m_handle, updateWhenEmpty);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder
        .addDoubleProperty("RPM", this::getRevolutionsPerMinute, null)
        .addDoubleProperty(
            "edgesPerRev", this::getEdgesPerRevolution, epr -> setEdgesPerRevolution((int) epr))
        .addDoubleProperty("MaxPeriod", () -> m_maxPeriod, this::setMaxPeriod)
        .addDoubleProperty(
            "samplesToAverage",
            this::getSamplesToAverage,
            samplesToAverage -> {
              setSamplesToAverage((int) samplesToAverage);
            })
        .addBooleanProperty("Stopped", this::getStopped, null);
  }
}
