/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.simulation.SimSpeedController;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;

/**
 * Solenoid class for running high voltage Digital Output.
 *
 * The Solenoid class is typically used for pneumatics solenoids, but could be used for any device
 * within the current spec of the PCM.
 */
public class Solenoid implements LiveWindowSendable {

  private int m_moduleNumber, m_channel; ///< The channel on the module to control.
  private boolean m_value;
  private SimSpeedController m_impl;

  /**
   * Common function to implement constructor behavior.
   */
  private synchronized void initSolenoid(int slot, int channel) {
    m_moduleNumber = slot;
    m_channel = channel;
    m_impl = new SimSpeedController("simulator/pneumatic/" + slot + "/" + channel);

    LiveWindow.addActuator("Solenoid", m_moduleNumber, m_channel, this);
  }

  /**
   * Constructor.
   *
   * @param channel The channel on the module to control.
   */
  public Solenoid(final int channel) {
    initSolenoid(1, channel);
  }

  /**
   * Constructor.
   *
   * @param moduleNumber The module number of the solenoid module to use.
   * @param channel      The channel on the module to control.
   */
  public Solenoid(final int moduleNumber, final int channel) {
    initSolenoid(moduleNumber, channel);
  }

  /**
   * Destructor.
   */
  public synchronized void free() {
  }

  /**
   * Set the value of a solenoid.
   *
   * @param on Turn the solenoid output off or on.
   */
  public void set(boolean on) {
    m_value = on;
    if (on) {
      m_impl.set(1);
    } else {
      m_impl.set(-1);
    }
  }

  /**
   * Read the current value of the solenoid.
   *
   * @return The current value of the solenoid.
   */
  public boolean get() {
    return m_value;
  }

  /*
   * Live Window code, only does anything if live window is activated.
   */
  public String getSmartDashboardType() {
    return "Solenoid";
  }

  private ITable m_table;
  private ITableListener m_table_listener;

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
      m_table.putBoolean("Value", get());
    }
  }


  /**
   * {@inheritDoc}
   */
  public void startLiveWindowMode() {
    set(false); // Stop for safety
    m_table_listener = new ITableListener() {
      public void valueChanged(ITable itable, String key, Object value, boolean bln) {
        set(((Boolean) value).booleanValue());
      }
    };
    m_table.addTableListener("Value", m_table_listener, true);
  }

  /**
   * {@inheritDoc}
   */
  public void stopLiveWindowMode() {
    set(false); // Stop for safety
    // TODO: Broken, should only remove the listener from "Value" only.
    m_table.removeTableListener(m_table_listener);
  }
}
