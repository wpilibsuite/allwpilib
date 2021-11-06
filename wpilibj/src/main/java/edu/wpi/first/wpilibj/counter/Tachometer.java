// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.counter;

import edu.wpi.first.wpilibj.DigitalSource;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import edu.wpi.first.hal.CounterJNI;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.FRCNetComm.tResourceType;

public class Tachometer implements Sendable, AutoCloseable {
  private final DigitalSource m_source;
  private final int m_handle;
  private int m_edgesPerRevolution = 1;

  public Tachometer(DigitalSource source) {
    m_source = requireNonNullParam(source, "source", "Tachometer");

    ByteBuffer index = ByteBuffer.allocateDirect(4);
    // set the byte order
    index.order(ByteOrder.LITTLE_ENDIAN);
    m_handle = CounterJNI.initializeCounter(0, index.asIntBuffer());

    CounterJNI.setCounterUpSource(m_handle, source.getPortHandleForRouting(),
        source.getAnalogTriggerTypeForRouting());
    CounterJNI.setCounterUpSourceEdge(m_handle, true, false);

    int intIndex = index.getInt();
    HAL.report(tResourceType.kResourceType_Counter, intIndex + 1);
    SendableRegistry.addLW(this, "Tachometer", intIndex);
  }

  @Override
  public void close() throws Exception {
    SendableRegistry.remove(this);
    CounterJNI.freeCounter(m_handle);
  }

  public double getPeriod() {
    return CounterJNI.getCounterPeriod(m_handle);
  }

  public double getFrequency() {
    double period = getPeriod();
    if (period == 0) {
      return 0;
    }
    return period;
  }

  public int getEdgesPerRevolution() {
    return m_edgesPerRevolution;
  }

  public void setEdgesPerRevolution(int edgesPerRevolution) {
    m_edgesPerRevolution = edgesPerRevolution;
  }

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

  public boolean getStopped() {
    return CounterJNI.getCounterStopped(m_handle);
  }

  public int getSamplesToAverage() {
    return CounterJNI.getCounterSamplesToAverage(m_handle);
  }

  public void setSamplesToAverage(int samplesToAverage) {
    CounterJNI.setCounterSamplesToAverage(m_handle, samplesToAverage);
  }

  public void setMaxPeriod(double maxPeriod) {
    CounterJNI.setCounterMaxPeriod(m_handle, maxPeriod);
  }

  public void setUpdateWhenEmpty(boolean updateWhenEmpty) {
    CounterJNI.setCounterUpdateWhenEmpty(m_handle, updateWhenEmpty);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Tachometer");
    builder.addDoubleProperty("RPM", this::getRevolutionsPerMinute, null);
  }
}
