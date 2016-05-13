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
 * VEX Robotics Jaguar Speed Control
 */
public class Jaguar implements SpeedController, PIDOutput, MotorSafety, LiveWindowSendable {
  private int channel;
  private SimSpeedController impl;

  /**
   * Common initialization code called by all constructors.
   */
  private void initJaguar(final int channel) {
    this.channel = channel;
    impl = new SimSpeedController("simulator/pwm/" + channel);

    m_safetyHelper = new MotorSafetyHelper(this);
    m_safetyHelper.setExpiration(0.0);
    m_safetyHelper.setSafetyEnabled(false);

    LiveWindow.addActuator("Jaguar", channel, this);
  }

  /**
   * Constructor.
   *
   * @param channel The PWM channel that the Jaguar is attached to.
   */
  public Jaguar(final int channel) {
    initJaguar(channel);
  }

  /**
   * Set the PWM value.
   *
   * @param speed     The speed to set.  Value should be between -1.0 and 1.0.
   * @param syncGroup The update group to add this Set() to, pending UpdateSyncGroup().  If 0,
   *                  update immediately.
   * @deprecated For compatibility with CANJaguar
   *
   * The PWM value is set using a range of -1.0 to 1.0, appropriately scaling the value for the
   * FPGA.
   */
  public void set(double speed, byte syncGroup) {
    impl.set(speed, syncGroup);
  }

  /**
   * Set the PWM value.
   *
   * The PWM value is set using a range of -1.0 to 1.0, appropriately scaling the value for the
   * FPGA.
   *
   * @param speed The speed value between -1.0 and 1.0 to set.
   */
  public void set(double speed) {
    impl.set(speed);
  }

  /**
   * Get the recently set value of the PWM.
   *
   * @return The most recently set value for the PWM between -1.0 and 1.0.
   */
  public double get() {
    return impl.get();
  }

  /**
   * Disable the speed controller
   */
  public void disable() {
    impl.set(0);
  }

  /**
   * Write out the PID value as seen in the PIDOutput base object.
   *
   * @param output Write out the PWM value as was found in the PIDController
   */
  public void pidWrite(double output) {
    impl.pidWrite(output);
  }

  //// MotorSafety Methods
  private MotorSafetyHelper m_safetyHelper;

  @Override
  public void setExpiration(double timeout) {
    m_safetyHelper.setExpiration(timeout);
  }

  @Override
  public double getExpiration() {
    return m_safetyHelper.getExpiration();
  }

  @Override
  public boolean isAlive() {
    return m_safetyHelper.isAlive();
  }

  @Override
  public void stopMotor() {
    disable();
  }

  @Override
  public void setSafetyEnabled(boolean enabled) {
    m_safetyHelper.setSafetyEnabled(enabled);
  }

  @Override
  public boolean isSafetyEnabled() {
    return m_safetyHelper.isSafetyEnabled();
  }

  @Override
  public String getDescription() {
    return "PWM " + channel + " on module 1";
  }

  //// LiveWindow Methods
  private ITable m_table;
  private ITableListener m_table_listener;

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
  public ITable getTable() {
    return m_table;
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public String getSmartDashboardType() {
    return "Speed Controller";
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void updateTable() {
    if (m_table != null) {
      m_table.putNumber("Value", get());
    }
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void startLiveWindowMode() {
    set(0.0); // Stop for safety
    m_table_listener = new ITableListener() {
      public void valueChanged(ITable itable, String key, Object value, boolean bln) {
        set(((Double) value).doubleValue());
      }
    };
    m_table.addTableListener("Value", m_table_listener, true);
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public void stopLiveWindowMode() {
    set(0); // Stop for safety
    // TODO: Broken, should only remove the listener from "Value" only.
    m_table.removeTableListener(m_table_listener);
  }
}
