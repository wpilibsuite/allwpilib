/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.simulation.SimDigitalInput;
import edu.wpi.first.wpilibj.tables.ITable;

/**
 * Class to read a digital input. This class will read digital inputs and return the current value
 * on the channel. Other devices such as encoders, gear tooth sensors, etc. that are implemented
 * elsewhere will automatically allocate digital inputs and outputs as required. This class is only
 * for devices like switches etc. that aren't implemented anywhere else.
 */
public class DigitalInput implements LiveWindowSendable {
  private SimDigitalInput impl;
  private int m_channel;

  /**
   * Create an instance of a Digital Input class. Creates a digital input given a channel.
   *
   * @param channel the port for the digital input
   */
  public DigitalInput(int channel) {
    impl = new SimDigitalInput("simulator/dio/" + channel);
    m_channel = channel;
  }

  /**
   * Get the value from a digital input channel. Retrieve the value of a single digital input
   * channel from the FPGA.
   *
   * @return the stats of the digital input
   */
  public boolean get() {
    return impl.get();
  }

  /**
   * Get the channel of the digital input
   *
   * @return The GPIO channel number that this object represents.
   */
  public int getChannel() {
    return m_channel;
  }

  public boolean getAnalogTriggerForRouting() {
    return false;
  }

  /*
   * Live Window code, only does anything if live window is activated.
   */
  public String getSmartDashboardType() {
    return "Digital Input";
  }

  private ITable m_table;

  /**
   * {@inheritDoc}
   */
  public void initTable(ITable subtable) {
    m_table = subtable;
    updateTable();
  }

  /**
   * {@inheritDoc}
   */
  public void updateTable() {
    if (m_table != null) {
      m_table.putBoolean("Value", get());
    }
  }

  /**
   * {@inheritDoc}
   */
  public ITable getTable() {
    return m_table;
  }

  /**
   * {@inheritDoc}
   */
  public void startLiveWindowMode() {
  }

  /**
   * {@inheritDoc}
   */
  public void stopLiveWindowMode() {
  }
}
