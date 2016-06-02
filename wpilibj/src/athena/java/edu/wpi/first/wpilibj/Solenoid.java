/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;
import edu.wpi.first.wpilibj.hal.SolenoidJNI;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;
import edu.wpi.first.wpilibj.util.AllocationException;
import edu.wpi.first.wpilibj.util.CheckedAllocationException;

/**
 * Solenoid class for running high voltage Digital Output.
 *
 * <p>The Solenoid class is typically used for pneumatics solenoids, but could be used for any
 * device within the current spec of the PCM.
 */
public class Solenoid extends SolenoidBase implements LiveWindowSendable {

  private final int m_channel; // /< The channel to control.
  private long m_solenoidPort;

  /**
   * Constructor using the default PCM ID (0)
   *
   * @param channel The channel on the PCM to control (0..7).
   */
  public Solenoid(final int channel) {
    this(getDefaultSolenoidModule(), channel);
  }

  /**
   * Constructor.
   *
   * @param moduleNumber The CAN ID of the PCM the solenoid is attached to.
   * @param channel      The channel on the PCM to control (0..7).
   */
  public Solenoid(final int moduleNumber, final int channel) {
    super(moduleNumber);
    m_channel = channel;

    checkSolenoidModule(m_moduleNumber);
    checkSolenoidChannel(m_channel);

    try {
      allocated.allocate(m_moduleNumber * kSolenoidChannels + m_channel);
    } catch (CheckedAllocationException ex) {
      throw new AllocationException("Solenoid channel " + m_channel + " on module "
        + m_moduleNumber + " is already allocated");
    }

    int portHandle = SolenoidJNI.getPortWithModule((byte) m_moduleNumber, (byte) m_channel);
    m_solenoidPort = SolenoidJNI.initializeSolenoidPort(portHandle);

    LiveWindow.addActuator("Solenoid", m_moduleNumber, m_channel, this);
    UsageReporting.report(tResourceType.kResourceType_Solenoid, m_channel, m_moduleNumber);
  }

  /**
   * Destructor.
   */
  public synchronized void free() {
    allocated.free(m_moduleNumber * kSolenoidChannels + m_channel);
    SolenoidJNI.freeSolenoidPort(m_solenoidPort);
    m_solenoidPort = 0;
    super.free();
  }

  /**
   * Set the value of a solenoid.
   *
   * @param on Turn the solenoid output off or on.
   */
  public void set(boolean on) {
    byte value = (byte) (on ? 0xFF : 0x00);
    byte mask = (byte) (1 << m_channel);

    set(value, mask);
  }

  /**
   * Read the current value of the solenoid.
   *
   * @return The current value of the solenoid.
   */
  public boolean get() {
    int value = getAll() & (1 << m_channel);
    return (value != 0);
  }

  /**
   * Check if solenoid is blacklisted. If a solenoid is shorted, it is added to the blacklist and
   * disabled until power cycle, or until faults are cleared.
   *
   * @return If solenoid is disabled due to short.
   * @see #clearAllPCMStickyFaults()
   */
  public boolean isBlackListed() {
    int value = getPCMSolenoidBlackList() & (1 << m_channel);
    return (value != 0);
  }

  /*
   * Live Window code, only does anything if live window is activated.
   */
  public String getSmartDashboardType() {
    return "Solenoid";
  }

  private ITable m_table;
  private ITableListener m_tableListener;

  @Override
  public void initTable(ITable subtable) {
    m_table = subtable;
    updateTable();
  }

  @Override
  public ITable getTable() {
    return m_table;
  }

  @Override
  public void updateTable() {
    if (m_table != null) {
      m_table.putBoolean("Value", get());
    }
  }


  @Override
  public void startLiveWindowMode() {
    set(false); // Stop for safety
    m_tableListener = (itable, key, value, bln) -> set((Boolean) value);
    m_table.addTableListener("Value", m_tableListener, true);
  }

  @Override
  public void stopLiveWindowMode() {
    set(false); // Stop for safety
    // TODO: Broken, should only remove the listener from "Value" only.
    m_table.removeTableListener(m_tableListener);
  }
}
