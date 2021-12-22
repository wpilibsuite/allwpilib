// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.counter;

import edu.wpi.first.hal.CounterJNI;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;
import edu.wpi.first.wpilibj.DigitalSource;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/** Up Down Counter. */
public class UpDownCounter implements Sendable, AutoCloseable {
  private DigitalSource m_upSource;
  private DigitalSource m_downSource;

  private final int m_handle;

  private boolean m_reverseDirection;

  /**
   * Constructs a new UpDown Counter.
   *
   * @param upSource The up count source (can be null).
   * @param downSource The down count source (can be null).
   */
  public UpDownCounter(DigitalSource upSource, DigitalSource downSource) {
    ByteBuffer index = ByteBuffer.allocateDirect(4);
    // set the byte order
    index.order(ByteOrder.LITTLE_ENDIAN);
    m_handle = CounterJNI.initializeCounter(CounterJNI.TWO_PULSE, index.asIntBuffer());

    if (upSource != null) {
      m_upSource = upSource;
      CounterJNI.setCounterUpSource(
          m_handle, upSource.getPortHandleForRouting(), upSource.getAnalogTriggerTypeForRouting());
      CounterJNI.setCounterUpSourceEdge(m_handle, true, false);
    }

    if (downSource != null) {
      m_downSource = downSource;
      CounterJNI.setCounterDownSource(
          m_handle,
          downSource.getPortHandleForRouting(),
          downSource.getAnalogTriggerTypeForRouting());
      CounterJNI.setCounterDownSourceEdge(m_handle, true, false);
    }

    reset();

    int intIndex = index.getInt();
    HAL.report(tResourceType.kResourceType_Counter, intIndex + 1);
    SendableRegistry.addLW(this, "UpDown Counter", intIndex);
  }

  @Override
  public void close() throws Exception {
    SendableRegistry.remove(this);
    CounterJNI.freeCounter(m_handle);
    CounterJNI.suppressUnused(m_upSource);
    CounterJNI.suppressUnused(m_downSource);
  }

  /**
   * Sets the configuration for the up source.
   *
   * @param configuration The up source configuration.
   */
  public void setUpEdgeConfiguration(EdgeConfiguration configuration) {
    CounterJNI.setCounterUpSourceEdge(m_handle, configuration.rising, configuration.falling);
  }

  /**
   * Sets the configuration for the down source.
   *
   * @param configuration The down source configuration.
   */
  public void setDownEdgeConfiguration(EdgeConfiguration configuration) {
    CounterJNI.setCounterDownSourceEdge(m_handle, configuration.rising, configuration.falling);
  }

  /** Resets the current count. */
  public void reset() {
    CounterJNI.resetCounter(m_handle);
  }

  /**
   * Sets to revert the counter direction.
   *
   * @param reverseDirection True to reverse counting direction.
   */
  public void setReverseDirection(boolean reverseDirection) {
    m_reverseDirection = reverseDirection;
    CounterJNI.setCounterReverseDirection(m_handle, m_reverseDirection);
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
  public void initSendable(SendableBuilder builder) {
    builder
        .setSmartDashboardType("UpDown Counter")
        .addDoubleProperty("Count", this::getCount, null)
        .addBooleanProperty(
            "reverseDirection", () -> m_reverseDirection, this::setReverseDirection);
  }
}
