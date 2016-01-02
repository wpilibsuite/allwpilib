/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.hal.DIOJNI;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.tables.ITable;

/**
 * Class to read a digital input. This class will read digital inputs and return
 * the current value on the channel. Other devices such as encoders, gear tooth
 * sensors, etc. that are implemented elsewhere will automatically allocate
 * digital inputs and outputs as required. This class is only for devices like
 * switches etc. that aren't implemented anywhere else.
 */
public class DigitalInput extends DigitalSource implements LiveWindowSendable {

  /**
   * Create an instance of a Digital Input class. Creates a digital input given
   * a channel.
   *
   * @param channel the DIO channel for the digital input 0-9 are on-board,
   *        10-25 are on the MXP
   */
  public DigitalInput(int channel) {
    initDigitalPort(channel, true);

    LiveWindow.addSensor("DigitalInput", channel, this);
    UsageReporting.report(tResourceType.kResourceType_DigitalInput, channel);
  }

  /**
   * Get the value from a digital input channel. Retrieve the value of a single
   * digital input channel from the FPGA.
   *
   * @return the status of the digital input
   */
  public boolean get() {
    return DIOJNI.getDIO(m_port);
  }

  /**
   * Get the channel of the digital input
   *
   * @return The GPIO channel number that this object represents.
   */
  public int getChannel() {
    return m_channel;
  }

  @Override
  public boolean getAnalogTriggerForRouting() {
    return false;
  }

  /*
   * Live Window code, only does anything if live window is activated.
   */
  @Override
  public String getSmartDashboardType() {
    return "Digital Input";
  }

  private ITable m_table;

  /**
   * {@inheritDoc}
   */
  @Override
  public void initTable(ITable subtable) {
    m_table = subtable;
    updateTable();
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void updateTable() {
    if (m_table != null) {
      m_table.putBoolean("Value", get());
    }
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public ITable getTable() {
    return m_table;
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void startLiveWindowMode() {}

  /**
   * {@inheritDoc}
   */
  @Override
  public void stopLiveWindowMode() {}
}
