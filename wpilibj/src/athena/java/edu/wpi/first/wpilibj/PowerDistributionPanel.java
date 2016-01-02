/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.PDPJNI;
import edu.wpi.first.wpilibj.hal.HALUtil;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.tables.ITable;

/**
 * Class for getting voltage, current, temperature, power and energy from the
 * CAN PDP.
 *$
 * @author Thomas Clark
 */
public class PowerDistributionPanel extends SensorBase implements LiveWindowSendable {

  int m_module;

  public PowerDistributionPanel(int module) {
    m_module = module;
    checkPDPModule(m_module);
    PDPJNI.initializePDP(m_module);
  }

  public PowerDistributionPanel() {
    this(0);
  }


  /**
   * Query the input voltage of the PDP
   *$
   * @return The voltage of the PDP in volts
   */
  public double getVoltage() {
    return PDPJNI.getPDPVoltage(m_module);
  }

  /**
   * Query the temperature of the PDP
   *$
   * @return The temperature of the PDP in degrees Celsius
   */
  public double getTemperature() {
    return PDPJNI.getPDPTemperature(m_module);
  }

  /**
   * Query the current of a single channel of the PDP
   *$
   * @return The current of one of the PDP channels (channels 0-15) in Amperes
   */
  public double getCurrent(int channel) {
    double current = PDPJNI.getPDPChannelCurrent((byte) channel, m_module);

    checkPDPChannel(channel);

    return current;
  }

  /**
   * Query the current of all monitored PDP channels (0-15)
   *$
   * @return The current of all the channels in Amperes
   */
  public double getTotalCurrent() {
    return PDPJNI.getPDPTotalCurrent(m_module);
  }

  /**
   * Query the total power drawn from the monitored PDP channels
   *$
   * @return the total power in Watts
   */
  public double getTotalPower() {
    return PDPJNI.getPDPTotalPower(m_module);
  }

  /**
   * Query the total energy drawn from the monitored PDP channels
   *$
   * @return the total energy in Joules
   */
  public double getTotalEnergy() {
    return PDPJNI.getPDPTotalEnergy(m_module);
  }

  /**
   * Reset the total energy to 0
   */
  public void resetTotalEnergy() {
    PDPJNI.resetPDPTotalEnergy(m_module);
  }

  /**
   * Clear all PDP sticky faults
   */
  public void clearStickyFaults() {
    PDPJNI.clearPDPStickyFaults(m_module);
  }

  public String getSmartDashboardType() {
    return "PowerDistributionPanel";
  }

  /*
   * Live Window code, only does anything if live window is activated.
   */
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
  public ITable getTable() {
    return m_table;
  }

  /**
   * {@inheritDoc}
   */
  public void updateTable() {
    if (m_table != null) {
      m_table.putNumber("Chan0", getCurrent(0));
      m_table.putNumber("Chan1", getCurrent(1));
      m_table.putNumber("Chan2", getCurrent(2));
      m_table.putNumber("Chan3", getCurrent(3));
      m_table.putNumber("Chan4", getCurrent(4));
      m_table.putNumber("Chan5", getCurrent(5));
      m_table.putNumber("Chan6", getCurrent(6));
      m_table.putNumber("Chan7", getCurrent(7));
      m_table.putNumber("Chan8", getCurrent(8));
      m_table.putNumber("Chan9", getCurrent(9));
      m_table.putNumber("Chan10", getCurrent(10));
      m_table.putNumber("Chan11", getCurrent(11));
      m_table.putNumber("Chan12", getCurrent(12));
      m_table.putNumber("Chan13", getCurrent(13));
      m_table.putNumber("Chan14", getCurrent(14));
      m_table.putNumber("Chan15", getCurrent(15));
      m_table.putNumber("Voltage", getVoltage());
      m_table.putNumber("TotalCurrent", getTotalCurrent());
    }
  }

  /**
   * PDP doesn't have to do anything special when entering the LiveWindow.
   * {@inheritDoc}
   */
  public void startLiveWindowMode() {}

  /**
   * PDP doesn't have to do anything special when exiting the LiveWindow.
   * {@inheritDoc}
   */
  public void stopLiveWindowMode() {}

}
