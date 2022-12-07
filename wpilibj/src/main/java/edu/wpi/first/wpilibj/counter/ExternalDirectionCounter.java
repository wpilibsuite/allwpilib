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

/**
 * Counter using external direction.
 *
 * <p>This counts on an edge from one digital input and the whether it counts up or down based on
 * the state of a second digital input.
 */
public class ExternalDirectionCounter implements Sendable, AutoCloseable {
  private final DigitalSource m_countSource;
  private final DigitalSource m_directionSource;

  private final int m_handle;

  /**
   * Constructs a new ExternalDirectionCounter.
   *
   * @param countSource The source for counting.
   * @param directionSource The source for selecting count direction.
   */
  public ExternalDirectionCounter(DigitalSource countSource, DigitalSource directionSource) {
    m_countSource = requireNonNullParam(countSource, "countSource", "ExternalDirectionCounter");
    m_directionSource =
        requireNonNullParam(directionSource, "directionSource", "ExternalDirectionCounter");

    ByteBuffer index = ByteBuffer.allocateDirect(4);
    // set the byte order
    index.order(ByteOrder.LITTLE_ENDIAN);
    m_handle = CounterJNI.initializeCounter(CounterJNI.EXTERNAL_DIRECTION, index.asIntBuffer());

    CounterJNI.setCounterUpSource(
        m_handle,
        countSource.getPortHandleForRouting(),
        countSource.getAnalogTriggerTypeForRouting());
    CounterJNI.setCounterUpSourceEdge(m_handle, true, false);

    CounterJNI.setCounterDownSource(
        m_handle,
        directionSource.getPortHandleForRouting(),
        directionSource.getAnalogTriggerTypeForRouting());
    CounterJNI.setCounterDownSourceEdge(m_handle, false, true);
    CounterJNI.resetCounter(m_handle);

    int intIndex = index.getInt();
    HAL.report(tResourceType.kResourceType_Counter, intIndex + 1);
    SendableRegistry.addLW(this, "External Direction Counter", intIndex);
  }

  /**
   * Gets the current count.
   *
   * @return The current count.
   */
  public int getCount() {
    return CounterJNI.getCounter(m_handle);
  }

  /**
   * Sets to revese the counter direction.
   *
   * @param reverseDirection True to reverse counting direction.
   */
  public void setReverseDirection(boolean reverseDirection) {
    CounterJNI.setCounterReverseDirection(m_handle, reverseDirection);
  }

  /** Resets the current count. */
  public void reset() {
    CounterJNI.resetCounter(m_handle);
  }

  /**
   * Sets the edge configuration for counting.
   *
   * @param configuration The counting edge configuration.
   */
  public void setEdgeConfiguration(EdgeConfiguration configuration) {
    CounterJNI.setCounterUpSourceEdge(m_handle, configuration.rising, configuration.falling);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
    CounterJNI.freeCounter(m_handle);
    CounterJNI.suppressUnused(m_countSource);
    CounterJNI.suppressUnused(m_directionSource);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("External Direction Counter");
    builder.addDoubleProperty("Count", this::getCount, null);
  }
}
