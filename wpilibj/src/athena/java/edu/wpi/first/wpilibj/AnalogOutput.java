/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.hal.AnalogJNI;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.util.AllocationException;
import edu.wpi.first.wpilibj.util.CheckedAllocationException;

/**
 * Analog output class.
 */
public class AnalogOutput extends SensorBase implements LiveWindowSendable {
  private static Resource channels = new Resource(kAnalogOutputChannels);
  private long m_port;
  private int m_channel;

  /**
   * Construct an analog output on a specified MXP channel.
   *
   * @param channel The channel number to represent.
   */
  public AnalogOutput(final int channel) {
    m_channel = channel;

    if (!AnalogJNI.checkAnalogOutputChannel(channel)) {
      throw new AllocationException("Analog output channel " + m_channel
          + " cannot be allocated. Channel is not present.");
    }
    try {
      channels.allocate(channel);
    } catch (CheckedAllocationException e) {
      throw new AllocationException("Analog output channel " + m_channel + " is already allocated");
    }

    long port_pointer = AnalogJNI.getPort((byte) channel);
    m_port = AnalogJNI.initializeAnalogOutputPort(port_pointer);

    LiveWindow.addSensor("AnalogOutput", channel, this);
    UsageReporting.report(tResourceType.kResourceType_AnalogOutput, channel);
  }

  /**
   * Channel destructor.
   */
  public void free() {
    AnalogJNI.freeAnalogOutputPort(m_port);
    m_port = 0;
    channels.free(m_channel);
    m_channel = 0;
  }

  public void setVoltage(double voltage) {
    AnalogJNI.setAnalogOutput(m_port, voltage);
  }

  public double getVoltage() {
    return AnalogJNI.getAnalogOutput(m_port);
  }

  /*
   * Live Window code, only does anything if live window is activated.
   */
  public String getSmartDashboardType() {
    return "Analog Output";
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
      m_table.putNumber("Value", getVoltage());
    }
  }

  /**
   * {@inheritDoc}
   */
  public ITable getTable() {
    return m_table;
  }

  /**
   * Analog Channels don't have to do anything special when entering the
   * LiveWindow. {@inheritDoc}
   */
  public void startLiveWindowMode() {}

  /**
   * Analog Channels don't have to do anything special when exiting the
   * LiveWindow. {@inheritDoc}
   */
  public void stopLiveWindowMode() {}
}
