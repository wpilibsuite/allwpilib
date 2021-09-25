// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.counters;

import edu.wpi.first.wpilibj.DigitalSource;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import edu.wpi.first.hal.CounterJNI;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

public class ExternalDirectionCounter implements Sendable, AutoCloseable {
  private DigitalSource m_countSource;
  private DigitalSource m_directionSource;

  private final int m_handle;

  public ExternalDirectionCounter(DigitalSource countSource, DigitalSource directionSource) {
    m_countSource = requireNonNullParam(countSource, "countSource", "ExternalDirectionCounter");
    m_directionSource = requireNonNullParam(directionSource, "directionSource", "ExternalDirectionCounter");

    ByteBuffer index = ByteBuffer.allocateDirect(4);
    // set the byte order
    index.order(ByteOrder.LITTLE_ENDIAN);
    m_handle = CounterJNI.initializeCounter(3, index.asIntBuffer());

    CounterJNI.setCounterUpSource(m_handle, countSource.getPortHandleForRouting(),
        countSource.getAnalogTriggerTypeForRouting());
    CounterJNI.setCounterUpSourceEdge(m_handle, true, false);

    CounterJNI.setCounterDownSource(m_handle, directionSource.getPortHandleForRouting(),
        directionSource.getAnalogTriggerTypeForRouting());
    CounterJNI.setCounterDownSourceEdge(m_handle, false, true);
    CounterJNI.resetCounter(m_handle);

    int intIndex = index.getInt();
    HAL.report(tResourceType.kResourceType_Counter, intIndex + 1);
    SendableRegistry.addLW(this, "External Direction Counter", intIndex);
  }

  public int getCount() {
    return CounterJNI.getCounter(m_handle);
  }

  public void setReverseDirection(boolean reverseDirection) {
    CounterJNI.setCounterReverseDirection(m_handle, reverseDirection);
  }

  public void reset() {
    CounterJNI.resetCounter(m_handle);
  }

  public void setDirectionSource(DigitalSource source) {
    m_directionSource = requireNonNullParam(source, "source", "setDirectionSource");
    CounterJNI.setCounterDownSource(m_handle, source.getPortHandleForRouting(),
        source.getAnalogTriggerTypeForRouting());
  }

  public void setCountSource(DigitalSource source) {
    m_countSource = requireNonNullParam(source, "source", "setCountSource");
    CounterJNI.setCounterUpSource(m_handle, source.getPortHandleForRouting(),
        source.getAnalogTriggerTypeForRouting());
  }

  public void setEdgeConfiguration(EdgeConfiguration configuration) {
    CounterJNI.setCounterUpSourceEdge(m_handle, configuration.rising, configuration.falling);
  }

  @Override
  public void close() throws Exception {
    SendableRegistry.remove(this);
    CounterJNI.freeCounter(m_handle);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("External Direction Counter");
    builder.addDoubleProperty("Count", this::getCount, null);
  }


}
