package edu.wpi.first.wpilibj.counters;

import edu.wpi.first.wpilibj.DigitalSource;
import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import edu.wpi.first.hal.CounterJNI;

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
  }

  public double getPeriod() {
    return CounterJNI.getCounterPeriod(m_handle);
  }

  public double getFrequency() {
    return 1 / getPeriod();
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
      return Double.MAX_VALUE;
    }
    return ((1.0 / getEdgesPerRevolution()) / period) * 60;
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
  public void close() throws Exception {
    CounterJNI.freeCounter(m_handle);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
  }
}
