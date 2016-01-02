/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import edu.wpi.first.wpilibj.DigitalSource;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.Counter;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;

import edu.wpi.first.wpilibj.hal.DigitalGlitchFilterJNI;

/**
 * Class to enable glitch filtering on a set of digital inputs.
 * This class will manage adding and removing digital inputs from a FPGA glitch
 * filter. The filter lets the user configure the time that an input must remain
 * high or low before it is classified as high or low.
 */
public class DigitalGlitchFilter extends SensorBase {
  public DigitalGlitchFilter() {
    synchronized(m_mutex) {
      int i = 0;
      while (m_filterAllocated[i] != false && i < m_filterAllocated.length) {
        i++;
      }
      if (i != m_filterAllocated.length) {
        m_channelIndex = i;
        m_filterAllocated[i] = true;
        UsageReporting.report(tResourceType.kResourceType_DigitalFilter,
                              m_channelIndex, 0);
      }
    }
  }

  public void free() {
    if (m_channelIndex >= 0) {
      synchronized(m_mutex) {
        m_filterAllocated[m_channelIndex] = false;
      }
      m_channelIndex = -1;
    }
  }

  private static void setFilter(DigitalSource input, int channelIndex) {
    if (input != null) { // Counter might have just one input
      DigitalGlitchFilterJNI.setFilterSelect(input.m_port, channelIndex);

      int selected = DigitalGlitchFilterJNI.getFilterSelect(input.m_port);
      if (selected != channelIndex) {
        throw new IllegalStateException("DigitalGlitchFilterJNI.setFilterSelect("
            + channelIndex + ") failed -> " + selected);
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
   * Sets the number of FPGA cycles that the input must hold steady to pass
   * through this glitch filter.
   *
   * @param fpga_cycles The number of FPGA cycles.
   */
  public void setPeriodCycles(int fpga_cycles) {
    DigitalGlitchFilterJNI.setFilterPeriod(m_channelIndex, fpga_cycles);
  }

  /**
   * Sets the number of nanoseconds that the input must hold steady to pass
   * through this glitch filter.
   *
   * @param nanoseconds The number of nanoseconds.
   */
  public void setPeriodNanoSeconds(long nanoseconds) {
    int fpga_cycles = (int) (nanoseconds * kSystemClockTicksPerMicrosecond / 4 /
                             1000);
    setPeriodCycles(fpga_cycles);
  }

  /**
   * Gets the number of FPGA cycles that the input must hold steady to pass
   * through this glitch filter.
   *
   * @return The number of cycles.
   */
  public int getPeriodCycles() {
    return DigitalGlitchFilterJNI.getFilterPeriod(m_channelIndex);
  }

  /**
   * Gets the number of nanoseconds that the input must hold steady to pass
   * through this glitch filter.
   *
   * @return The number of nanoseconds.
   */
  public long getPeriodNanoSeconds() {
    int fpga_cycles = getPeriodCycles();

    return (long) fpga_cycles * 1000L /
           (long) (kSystemClockTicksPerMicrosecond / 4);
  }

  private int m_channelIndex = -1;
  private static final Lock m_mutex = new ReentrantLock(true);
  private static final boolean[] m_filterAllocated = new boolean[3];
};
