/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.wpilibj.hal.PDPJNI;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;

/**
 * Class for getting voltage, current, temperature, power and energy from the Power Distribution
 * Panel over CAN.
 */
public class PowerDistributionPanel extends SensorBase implements LiveWindowSendable {

  private final int m_module;

  /**
   * Constructor.
   *
   * @param module The CAN ID of the PDP
   */
  public PowerDistributionPanel(int module) {
    m_module = module;
    checkPDPModule(module);
    PDPJNI.initializePDP(module);
  }

  /**
   * Constructor.  Uses the default CAN ID (0).
   */
  public PowerDistributionPanel() {
    this(0);
  }

  /**
   * Query the input voltage of the PDP.
   *
   * @return The voltage of the PDP in volts
   */
  public double getVoltage() {
    return PDPJNI.getPDPVoltage(m_module);
  }

  /**
   * Query the temperature of the PDP.
   *
   * @return The temperature of the PDP in degrees Celsius
   */
  public double getTemperature() {
    return PDPJNI.getPDPTemperature(m_module);
  }

  /**
   * Query the current of a single channel of the PDP.
   *
   * @return The current of one of the PDP channels (channels 0-15) in Amperes
   */
  public double getCurrent(int channel) {
    double current = PDPJNI.getPDPChannelCurrent((byte) channel, m_module);

    checkPDPChannel(channel);

    return current;
  }

  /**
   * Query the current of all monitored PDP channels (0-15).
   *
   * @return The current of all the channels in Amperes
   */
  public double getTotalCurrent() {
    return PDPJNI.getPDPTotalCurrent(m_module);
  }

  /**
   * Query the total power drawn from the monitored PDP channels.
   *
   * @return the total power in Watts
   */
  public double getTotalPower() {
    return PDPJNI.getPDPTotalPower(m_module);
  }

  /**
   * Query the total energy drawn from the monitored PDP channels.
   *
   * @return the total energy in Joules
   */
  public double getTotalEnergy() {
    return PDPJNI.getPDPTotalEnergy(m_module);
  }

  /**
   * Reset the total energy to 0.
   */
  public void resetTotalEnergy() {
    PDPJNI.resetPDPTotalEnergy(m_module);
  }

  /**
   * Clear all PDP sticky faults.
   */
  public void clearStickyFaults() {
    PDPJNI.clearPDPStickyFaults(m_module);
  }

  @Override
  public String getSmartDashboardType() {
    return "PowerDistributionPanel";
  }

  /*
   * Live Window code, only does anything if live window is activated.
   */
  private NetworkTable m_table;
  private NetworkTableEntry[] m_chanEntry;
  private NetworkTableEntry m_voltageEntry;
  private NetworkTableEntry m_totalCurrentEntry;

  @Override
  public void initTable(NetworkTable subtable) {
    m_table = subtable;
    if (m_table != null) {
      m_chanEntry = new NetworkTableEntry[16];
      for (int i = 0; i < m_chanEntry.length; i++) {
        m_chanEntry[i] = m_table.getEntry("Chan" + i);
      }
      m_voltageEntry = m_table.getEntry("Voltage");
      m_totalCurrentEntry = m_table.getEntry("TotalCurrent");
      updateTable();
    } else {
      m_chanEntry = null;
      m_voltageEntry = null;
      m_totalCurrentEntry = null;
    }
  }

  @Override
  public NetworkTable getTable() {
    return m_table;
  }

  @Override
  public void updateTable() {
    if (m_chanEntry != null) {
      for (int i = 0; i < m_chanEntry.length; i++) {
        m_chanEntry[i].setDouble(getCurrent(i));
      }
    }
    if (m_voltageEntry != null) {
      m_voltageEntry.setDouble(getVoltage());
    }
    if (m_totalCurrentEntry != null) {
      m_totalCurrentEntry.setDouble(getTotalCurrent());
    }
  }

  /**
   * PDP doesn't have to do anything special when entering the LiveWindow. {@inheritDoc}
   */
  @Override
  public void startLiveWindowMode() {
  }

  /**
   * PDP doesn't have to do anything special when exiting the LiveWindow. {@inheritDoc}
   */
  @Override
  public void stopLiveWindowMode() {
  }

}
