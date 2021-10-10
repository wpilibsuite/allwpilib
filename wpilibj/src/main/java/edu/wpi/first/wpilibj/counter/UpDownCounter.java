<<<<<<< HEAD:wpilibj/src/main/java/edu/wpi/first/wpilibj/counters/UpDownCounter.java
package edu.wpi.first.wpilibj.counters;
=======
// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.counter;
>>>>>>> b2cf6d866f5e2166c1b0478274a3315a008015e6:wpilibj/src/main/java/edu/wpi/first/wpilibj/counter/UpDownCounter.java

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import edu.wpi.first.hal.CounterJNI;
import edu.wpi.first.wpilibj.DigitalSource;
<<<<<<< HEAD:wpilibj/src/main/java/edu/wpi/first/wpilibj/counters/UpDownCounter.java
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;
=======
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
>>>>>>> b2cf6d866f5e2166c1b0478274a3315a008015e6:wpilibj/src/main/java/edu/wpi/first/wpilibj/counter/UpDownCounter.java

public class UpDownCounter implements Sendable, AutoCloseable {
  private DigitalSource m_upSource;
  private DigitalSource m_downSource;

  private final int m_handle;

  public UpDownCounter() {
    ByteBuffer index = ByteBuffer.allocateDirect(4);
    // set the byte order
    index.order(ByteOrder.LITTLE_ENDIAN);
    m_handle = CounterJNI.initializeCounter(0, index.asIntBuffer());
  }

  public void setUpSource(DigitalSource source) {
    if (source == null) {
      CounterJNI.clearCounterUpSource(m_handle);
      m_upSource = null;
    } else {
<<<<<<< HEAD:wpilibj/src/main/java/edu/wpi/first/wpilibj/counters/UpDownCounter.java
      m_upSource = source;
=======
      m_upSource = requireNonNullParam(source, "source", "setUpSource");
>>>>>>> b2cf6d866f5e2166c1b0478274a3315a008015e6:wpilibj/src/main/java/edu/wpi/first/wpilibj/counter/UpDownCounter.java
      CounterJNI.setCounterUpSource(m_handle, source.getPortHandleForRouting(),
          source.getAnalogTriggerTypeForRouting());
      CounterJNI.setCounterUpSourceEdge(m_handle, true, false);
    }
  }

  public void setDownSource(DigitalSource source) {
    if (source == null) {
      CounterJNI.clearCounterDownSource(m_handle);
      m_downSource = null;
    } else {
<<<<<<< HEAD:wpilibj/src/main/java/edu/wpi/first/wpilibj/counters/UpDownCounter.java
      m_downSource = source;
=======
      m_downSource = requireNonNullParam(source, "source", "setDownSource");;
>>>>>>> b2cf6d866f5e2166c1b0478274a3315a008015e6:wpilibj/src/main/java/edu/wpi/first/wpilibj/counter/UpDownCounter.java
      CounterJNI.setCounterDownSource(m_handle, source.getPortHandleForRouting(),
          source.getAnalogTriggerTypeForRouting());
      CounterJNI.setCounterDownSourceEdge(m_handle, true, false);
    }
  }

  public void setUpEdgeConfiguration(EdgeConfiguration configuration) {
    CounterJNI.setCounterUpSourceEdge(m_handle, configuration.rising, configuration.falling);
  }

  public void setDownEdgeConfiguration(EdgeConfiguration configuration) {
    CounterJNI.setCounterDownSourceEdge(m_handle, configuration.rising, configuration.falling);
  }

  public void reset() {
    CounterJNI.resetCounter(m_handle);
  }

  public void setReverseDirection(boolean reverseDirection) {
    CounterJNI.setCounterReverseDirection(m_handle, reverseDirection);
  }

  public int getCount() {
    return CounterJNI.getCounter(m_handle);
  }

  @Override
  public void close() throws Exception {
    CounterJNI.freeCounter(m_handle);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
  }
}
