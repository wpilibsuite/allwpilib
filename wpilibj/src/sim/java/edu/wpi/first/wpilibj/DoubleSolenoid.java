/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.livewindow.LiveWindowSendable;
import edu.wpi.first.wpilibj.simulation.SimSpeedController;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;

/**
 * DoubleSolenoid class for running 2 channels of high voltage Digital Output.
 *
 * The DoubleSolenoid class is typically used for pneumatics solenoids that have two positions
 * controlled by two separate channels.
 */
public class DoubleSolenoid implements LiveWindowSendable {

  /**
   * Possible values for a DoubleSolenoid
   */
  public static class Value {

    public final int value;
    public static final int kOff_val = 0;
    public static final int kForward_val = 1;
    public static final int kReverse_val = 2;
    public static final Value kOff = new Value(kOff_val);
    public static final Value kForward = new Value(kForward_val);
    public static final Value kReverse = new Value(kReverse_val);

    private Value(int value) {
      this.value = value;
    }
  }

  private int m_forwardChannel; ///< The forward channel on the module to control.
  private int m_reverseChannel; ///< The reverse channel on the module to control.
  private int m_moduleNumber;
  private boolean m_reverseDirection;
  private SimSpeedController m_impl;
  private Value m_value;

  /**
   * Common function to implement constructor behavior.
   */
  private synchronized void initSolenoid(int moduleNumber, int forwardChannel, int reverseChannel) {
    m_forwardChannel = forwardChannel;
    m_reverseChannel = reverseChannel;
    m_moduleNumber = moduleNumber;
//      LiveWindow.addActuator("DoubleSolenoid", m_moduleNumber, m_forwardChannel, this);

    if (reverseChannel < forwardChannel) { // Swap ports
      int channel = forwardChannel;
      forwardChannel = reverseChannel;
      reverseChannel = channel;
      m_reverseDirection = true;
    }
    m_impl = new SimSpeedController("simulator/pneumatic/" + moduleNumber + "/" + forwardChannel
        + "/" + moduleNumber + "/" + reverseChannel);
  }

  /**
   * Constructor.
   *
   * @param forwardChannel The forward channel on the module to control.
   * @param reverseChannel The reverse channel on the module to control.
   */
  public DoubleSolenoid(final int forwardChannel, final int reverseChannel) {
    initSolenoid(1, forwardChannel, reverseChannel);
  }

  /**
   * Constructor.
   *
   * @param moduleNumber   The module number of the solenoid module to use.
   * @param forwardChannel The forward channel on the module to control.
   * @param reverseChannel The reverse channel on the module to control.
   */
  public DoubleSolenoid(final int moduleNumber, final int forwardChannel, final int
      reverseChannel) {
    initSolenoid(moduleNumber, forwardChannel, reverseChannel);
  }

  /**
   * Destructor.
   */
  public synchronized void free() {
  }

  /**
   * Set the value of a solenoid.
   *
   * @param value Move the solenoid to forward, reverse, or don't move it.
   */
  public void set(final Value value) {
    m_value = value;
    switch (value.value) {
      case Value.kOff_val:
        m_impl.set(0);
        break;
      case Value.kForward_val:
        m_impl.set(m_reverseDirection ? -1 : 1);
        break;
      case Value.kReverse_val:
        m_impl.set(m_reverseDirection ? 1 : -1);
        break;
    }
  }

  /**
   * Read the current value of the solenoid.
   *
   * @return The current value of the solenoid.
   */
  public Value get() {
    return m_value;
  }

  /*
   * Live Window code, only does anything if live window is activated.
   */
  public String getSmartDashboardType() {
    return "Double Solenoid";
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
      //TODO: this is bad
      m_table.putString("Value", (get() == Value.kForward ? "Forward" : (get() == Value.kReverse
          ? "Reverse" : "Off")));
    }
  }

  /**
   * {@inheritDoc}
   */
  public void startLiveWindowMode() {
    set(Value.kOff); // Stop for safety
    m_table_listener = new ITableListener() {
      public void valueChanged(ITable itable, String key, Object value, boolean bln) {
        //TODO: this is bad also
        if (value.toString().equals("Reverse"))
          set(Value.kReverse);
        else if (value.toString().equals("Forward"))
          set(Value.kForward);
        else
          set(Value.kOff);
      }
    };
    m_table.addTableListener("Value", m_table_listener, true);
  }

  /**
   * {@inheritDoc}
   */
  public void stopLiveWindowMode() {
    set(Value.kOff); // Stop for safety
    // TODO: Broken, should only remove the listener from "Value" only.
    m_table.removeTableListener(m_table_listener);
  }
}
