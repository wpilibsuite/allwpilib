/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.networktables.EntryListenerFlags;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;
import edu.wpi.first.wpilibj.hal.SolenoidJNI;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;

/**
 * Solenoid class for running high voltage Digital Output on the PCM.
 *
 * <p>The Solenoid class is typically used for pneumatic solenoids, but could be used for any
 * device within the current spec of the PCM.
 */
public class Solenoid extends SolenoidBase implements LiveWindowSendable {

  private final int m_channel; // The channel to control.
  private int m_solenoidHandle;

  /**
   * Constructor using the default PCM ID (defaults to 0).
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

    int portHandle = SolenoidJNI.getPortWithModule((byte) m_moduleNumber, (byte) m_channel);
    m_solenoidHandle = SolenoidJNI.initializeSolenoidPort(portHandle);

    LiveWindow.addActuator("Solenoid", m_moduleNumber, m_channel, this);
    HAL.report(tResourceType.kResourceType_Solenoid, m_channel, m_moduleNumber);
  }

  /**
   * Destructor.
   */
  public synchronized void free() {
    SolenoidJNI.freeSolenoidPort(m_solenoidHandle);
    m_solenoidHandle = 0;
    if (m_valueEntry != null) {
      m_valueEntry.removeListener(m_valueListener);
    }
    super.free();
  }

  /**
   * Set the value of a solenoid.
   *
   * @param on True will turn the solenoid output on. False will turn the solenoid output off.
   */
  public void set(boolean on) {
    SolenoidJNI.setSolenoid(m_solenoidHandle, on);
  }

  /**
   * Read the current value of the solenoid.
   *
   * @return True if the solenoid output is on or false if the solenoid output is off.
   */
  public boolean get() {
    return SolenoidJNI.getSolenoid(m_solenoidHandle);
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
    return value != 0;
  }

  /*
   * Live Window code, only does anything if live window is activated.
   */
  public String getSmartDashboardType() {
    return "Solenoid";
  }

  private NetworkTable m_table;
  private NetworkTableEntry m_valueEntry;
  private int m_valueListener;

  @Override
  public void initTable(NetworkTable subtable) {
    m_table = subtable;
    if (m_table != null) {
      m_valueEntry = m_table.getEntry("Value");
      updateTable();
    } else {
      m_valueEntry = null;
    }
  }

  @Override
  public NetworkTable getTable() {
    return m_table;
  }

  @Override
  public void updateTable() {
    if (m_valueEntry != null) {
      m_valueEntry.setBoolean(get());
    }
  }

  @Override
  public void startLiveWindowMode() {
    set(false); // Stop for safety
    m_valueListener = m_valueEntry.addListener((event) -> set(event.value.getBoolean()),
        EntryListenerFlags.kImmediate | EntryListenerFlags.kNew | EntryListenerFlags.kUpdate);
  }

  @Override
  public void stopLiveWindowMode() {
    set(false); // Stop for safety
    m_valueEntry.removeListener(m_valueListener);
    m_valueListener = 0;
  }
}
