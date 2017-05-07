/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.DIOJNI;
import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.tables.ITable;

/**
 * Class to read a digital input. This class will read digital inputs and return the current value
 * on the channel. Other devices such as encoders, gear tooth sensors, etc. that are implemented
 * elsewhere will automatically allocate digital inputs and outputs as required. This class is only
 * for devices like switches etc. that aren't implemented anywhere else.
 */
public class DigitalInput extends DigitalSource implements LiveWindowSendable {
  private int m_channel = 0;
  private int m_handle = 0;

  /**
   * Create an instance of a Digital Input class. Creates a digital input given a channel.
   *
   * @param channel the DIO channel for the digital input 0-9 are on-board, 10-25 are on the MXP
   */
  public DigitalInput(int channel) {
    checkDigitalChannel(channel);
    m_channel = channel;

    m_handle = DIOJNI.initializeDIOPort(DIOJNI.getPort((byte) channel), true);

    LiveWindow.addSensor("DigitalInput", channel, this);
    HAL.report(tResourceType.kResourceType_DigitalInput, channel);
  }

  /**
   * Frees the resources for this output.
   */
  public void free() {
    if (m_interrupt != 0) {
      cancelInterrupts();
    }

    DIOJNI.freeDIOPort(m_handle);
  }

  /**
   * Get the value from a digital input channel. Retrieve the value of a single digital input
   * channel from the FPGA.
   *
   * @return the status of the digital input
   */
  public boolean get() {
    return DIOJNI.getDIO(m_handle);
  }

  /**
   * Get the channel of the digital input
   *
   * @return The GPIO channel number that this object represents.
   */
  @Override
  public int getChannel() {
    return m_channel;
  }

  /**
   * Get the analog trigger type.
   *
   * @return false
   */
  @Override
  public int getAnalogTriggerTypeForRouting() {
    return 0;
  }

  /**
   * Is this an analog trigger.
   *
   * @return true if this is an analog trigger
   */
  @Override
  public boolean isAnalogTrigger() {
    return false;
  }

  /**
   * Get the HAL Port Handle.
   *
   * @return The HAL Handle to the specified source.
   */
  @Override
  public int getPortHandleForRouting() {
    return m_handle;
  }

  @Override
  public String getSmartDashboardType() {
    return "Digital Input";
  }

  private ITable m_table;


  @Override
  public void initTable(ITable subtable) {
    m_table = subtable;
    updateTable();
  }


  @Override
  public void updateTable() {
    if (m_table != null) {
      m_table.putBoolean("Value", get());
    }
  }


  @Override
  public ITable getTable() {
    return m_table;
  }


  @Override
  public void startLiveWindowMode() {
  }


  @Override
  public void stopLiveWindowMode() {
  }
}
