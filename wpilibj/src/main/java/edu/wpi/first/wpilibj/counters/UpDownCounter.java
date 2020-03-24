package edu.wpi.first.wpilibj.counters;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import edu.wpi.first.hal.CounterJNI;
import edu.wpi.first.wpilibj.DigitalSource;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

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
      m_upSource = source;
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
      m_downSource = source;
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
