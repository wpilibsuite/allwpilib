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
 * VEX Robotics Victor Speed Controller.
 */
public class Victor implements SpeedController, PIDOutput, MotorSafety, LiveWindowSendable {
  private int m_channel;
  private SimSpeedController m_impl;

  /**
   * Common initialization code called by all constructors.
   *
   * <p>Note that the Victor uses the following bounds for PWM values.  These values were determined
   * empirically and optimized for the Victor 888. These values should work reasonably well for
   * Victor 884 controllers also but if users experience issues such as asymmetric behavior around
   * the deadband or inability to saturate the controller in either direction, calibration is
   * recommended. The calibration procedure can be found in the Victor 884 User Manual available
   * from VEX Robotics: http://content.vexrobotics.com/docs/ifi-v884-users-manual-9-25-06.pdf
   *
   * <p>- 2.027ms = full "forward" - 1.525ms = the "high end" of the deadband range - 1.507ms = center
   * of the deadband range (off) - 1.49ms = the "low end" of the deadband range - 1.026ms = full
   * "reverse"
   */
  private void initVictor(final int channel) {
    this.m_channel = channel;
    m_impl = new SimSpeedController("simulator/pwm/" + channel);

    m_safetyHelper = new MotorSafetyHelper(this);
    m_safetyHelper.setExpiration(0.0);
    m_safetyHelper.setSafetyEnabled(false);

    LiveWindow.addActuator("Victor", channel, this);
    // UsageReporting.report(tResourceType.kResourceType_Talon, getChannel(), getModuleNumber()-1);
  }

  /**
   * Constructor.
   *
   * @param channel The PWM channel that the Victor is attached to.
   */
  public Victor(final int channel) {
    initVictor(channel);
  }

  /**
   * Set the PWM value.
   *
   * <p>The PWM value is set using a range of -1.0 to 1.0, appropriately scaling the value for the
   * FPGA.
   *
   * @param speed     The speed to set.  Value should be between -1.0 and 1.0.
   * @param syncGroup The update group to add this Set() to, pending UpdateSyncGroup().  If 0,
   *                  update immediately.
   * @deprecated For compatibility with CANJaguar.
   */
  public void set(double speed, byte syncGroup) {
    m_impl.set(speed, syncGroup);
  }

  /**
   * Set the PWM value.
   *
   * <p>The PWM value is set using a range of -1.0 to 1.0, appropriately scaling the value for the
   * FPGA.
   *
   * @param speed The speed value between -1.0 and 1.0 to set.
   */
  public void set(double speed) {
    m_impl.set(speed);
  }

  /**
   * Get the recently set value of the PWM.
   *
   * @return The most recently set value for the PWM between -1.0 and 1.0.
   */
  public double get() {
    return m_impl.get();
  }

  /**
   * Disable the speed controller.
   */
  public void disable() {
    m_impl.set(0);
  }

  /**
   * Write out the PID value as seen in the PIDOutput base object.
   *
   * @param output Write out the PWM value as was found in the PIDController
   */
  public void pidWrite(double output) {
    m_impl.pidWrite(output);
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
    return "PWM " + m_channel + " on module 1";
  }

  //// LiveWindow Methods
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
  public String getSmartDashboardType() {
    return "Speed Controller";
  }

  @Override
  public void updateTable() {
    if (m_table != null) {
      m_table.putNumber("Value", get());
    }
  }

  @Override
  public void startLiveWindowMode() {
    set(0.0); // Stop for safety
    m_tableListener = new ITableListener() {
      public void valueChanged(ITable itable, String key, Object value, boolean bln) {
        set(((Double) value).doubleValue());
      }
    };
    m_table.addTableListener("Value", m_tableListener, true);
  }

  @Override
  public void stopLiveWindowMode() {
    set(0); // Stop for safety
    // TODO: Broken, should only remove the listener from "Value" only.
    m_table.removeTableListener(m_tableListener);
  }
}
