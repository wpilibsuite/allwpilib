// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.DigitalGlitchFilterJNI;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.util.AllocationException;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

/**
 * Class to enable glitch filtering on a set of digital inputs. This class will manage adding and
 * removing digital inputs from an FPGA glitch filter. The filter lets the user configure the time
 * that an input must remain high or low before it is classified as high or low.
 */
public class DigitalGlitchFilter implements Sendable, AutoCloseable {
  /** Configures the Digital Glitch Filter to its default settings. */
  @SuppressWarnings("this-escape")
  public DigitalGlitchFilter() {
    m_channelIndex = allocateFilterIndex();
    if (m_channelIndex < 0) {
      throw new AllocationException("No filters available to allocate.");
    }
    HAL.report(tResourceType.kResourceType_DigitalGlitchFilter, m_channelIndex + 1, 0);
    SendableRegistry.addLW(this, "DigitalGlitchFilter", m_channelIndex);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
    if (m_channelIndex >= 0) {
      m_mutex.lock();
      try {
        m_filterAllocated[m_channelIndex] = false;
      } finally {
        m_mutex.unlock();
      }

      m_channelIndex = -1;
    }
  }

  /**
   * Allocates the next available filter index, or -1 if there are no filters available.
   *
   * @return the filter index
   */
  private static int allocateFilterIndex() {
    m_mutex.lock();
    try {
      for (int index = 0; index < m_filterAllocated.length; index++) {
        if (!m_filterAllocated[index]) {
          m_filterAllocated[index] = true;
          return index;
        }
      }
    } finally {
      m_mutex.unlock();
    }
    return -1;
  }

  private static void setFilter(DigitalSource input, int channelIndex) {
    if (input != null) { // Counter might have just one input
      // analog triggers are not supported for DigitalGlitchFilters
      if (input.isAnalogTrigger()) {
        throw new IllegalStateException("Analog Triggers not supported for DigitalGlitchFilters");
      }
      DigitalGlitchFilterJNI.setFilterSelect(input.getPortHandleForRouting(), channelIndex);

      int selected = DigitalGlitchFilterJNI.getFilterSelect(input.getPortHandleForRouting());
      if (selected != channelIndex) {
        throw new IllegalStateException(
            "DigitalGlitchFilterJNI.setFilterSelect(" + channelIndex + ") failed -> " + selected);
      }
    }
  }

  /**
   * Assigns the DigitalSource to this glitch filter.
   *
   * @param input The DigitalSource to add.
   */
  public void add(DigitalSource input) {
    setFilter(input, m_channelIndex + 1);
  }

  /**
   * Assigns the Encoder to this glitch filter.
   *
   * @param input The Encoder to add.
   */
  public void add(Encoder input) {
    add(input.m_aSource);
    add(input.m_bSource);
  }

  /**
   * Assigns the Counter to this glitch filter.
   *
   * @param input The Counter to add.
   */
  public void add(Counter input) {
    add(input.m_upSource);
    add(input.m_downSource);
  }

  /**
   * Removes this filter from the given digital input.
   *
   * @param input The DigitalSource to stop filtering.
   */
  public void remove(DigitalSource input) {
    setFilter(input, 0);
  }

  /**
   * Removes this filter from the given Encoder.
   *
   * @param input the Encoder to stop filtering.
   */
  public void remove(Encoder input) {
    remove(input.m_aSource);
    remove(input.m_bSource);
  }

  /**
   * Removes this filter from the given Counter.
   *
   * @param input The Counter to stop filtering.
   */
  public void remove(Counter input) {
    remove(input.m_upSource);
    remove(input.m_downSource);
  }

  /**
   * Sets the number of FPGA cycles that the input must hold steady to pass through this glitch
   * filter.
   *
   * @param fpgaCycles The number of FPGA cycles.
   */
  public void setPeriodCycles(int fpgaCycles) {
    DigitalGlitchFilterJNI.setFilterPeriod(m_channelIndex, fpgaCycles);
  }

  /**
   * Sets the number of nanoseconds that the input must hold steady to pass through this glitch
   * filter.
   *
   * @param nanoseconds The number of nanoseconds.
   */
  public void setPeriodNanoSeconds(long nanoseconds) {
    int fpgaCycles = (int) (nanoseconds * SensorUtil.kSystemClockTicksPerMicrosecond / 4 / 1000);
    setPeriodCycles(fpgaCycles);
  }

  /**
   * Gets the number of FPGA cycles that the input must hold steady to pass through this glitch
   * filter.
   *
   * @return The number of cycles.
   */
  public int getPeriodCycles() {
    return DigitalGlitchFilterJNI.getFilterPeriod(m_channelIndex);
  }

  /**
   * Gets the number of nanoseconds that the input must hold steady to pass through this glitch
   * filter.
   *
   * @return The number of nanoseconds.
   */
  public long getPeriodNanoSeconds() {
    int fpgaCycles = getPeriodCycles();

    return fpgaCycles * 1000L / (SensorUtil.kSystemClockTicksPerMicrosecond / 4);
  }

  @Override
  public void initSendable(SendableBuilder builder) {}

  private int m_channelIndex;
  private static final Lock m_mutex = new ReentrantLock(true);
  private static final boolean[] m_filterAllocated = new boolean[3];
}
