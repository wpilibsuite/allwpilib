/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
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
 * Standard hobby style servo.
 *
 * The range parameters default to the appropriate values for the Hitec HS-322HD servo provided in
 * the FIRST Kit of Parts in 2008.
 */
public class Servo implements SpeedController, LiveWindowSendable {

  private static final double kMaxServoAngle = 180.0;
  private static final double kMinServoAngle = 0.0;

  protected static final double kDefaultMaxServoPWM = 2.4;
  protected static final double kDefaultMinServoPWM = .6;

  private SimSpeedController impl;
  private int channel;

  /**
   * Common initialization code called by all constructors.
   *
   * InitServo() assigns defaults for the period multiplier for the servo PWM control signal, as
   * well as the minimum and maximum PWM values supported by the servo.
   */
  private void initServo() {
    LiveWindow.addActuator("Servo", channel, this);
    impl = new SimSpeedController("simulator/pwm/" + channel);
  }

  /**
   * Set the PWM value.
   *
   * @param speed     The speed to set.  Value should be between -1.0 and 1.0.
   * @param syncGroup The update group to add this Set() to, pending UpdateSyncGroup().  If 0,
   *                  update immediately.
   * @deprecated The PWM value is set using a range of -1.0 to 1.0, appropriately scaling the value
   * for the FPGA.
   */
  public void set(double speed, byte syncGroup) {
    impl.set(speed, syncGroup);
  }

  /**
   * Write out the PID value as seen in the PIDOutput base object.
   *
   * @param output Write out the PWM value as was found in the PIDController
   */
  public void pidWrite(double output) {
    impl.pidWrite(output);
  }

  /**
   * Constructor.<br>
   *
   * By default {@value #kDefaultMaxServoPWM} ms is used as the maxPWM value<br> By default {@value
   * #kDefaultMinServoPWM} ms is used as the minPWM value<br>
   *
   * @param channel The PWM channel to which the servo is attached. 0-9 are on-board, 10-19 are on
   *                the MXP port
   */
  public Servo(final int channel) {
    this.channel = channel;
    initServo();
  }

  private double getServoAngleRange() {
    return kMaxServoAngle - kMinServoAngle;
  }

  /**
   * Set the servo position.
   *
   * Servo values range from -1.0 to 1.0 corresponding to the range of full left to full right.
   *
   * @param value Position from -1.0 to 1.0.
   */
  public void set(double value) {
    impl.set(value);
  }

  /**
   * Get the servo position.
   *
   * Servo values range from -1.0 to 1.0 corresponding to the range of full left to full right.
   *
   * @return Position from -1.0 to 1.0.
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
   * Set the servo angle.
   *
   * Assume that the servo angle is linear with respect to the PWM value (big assumption, need to
   * test).
   *
   * Servo angles that are out of the supported range of the servo simply "saturate" in that
   * direction In other words, if the servo has a range of (X degrees to Y degrees) than angles of
   * less than X result in an angle of X being set and angles of more than Y degrees result in an
   * angle of Y being set.
   *
   * @param degrees The angle in degrees to set the servo.
   */
  public void setAngle(double degrees) {
    if (degrees < kMinServoAngle) {
      degrees = kMinServoAngle;
    } else if (degrees > kMaxServoAngle) {
      degrees = kMaxServoAngle;
    }

    set((degrees - kMinServoAngle) / getServoAngleRange());
  }

  /**
   * Get the servo angle.
   *
   * Assume that the servo angle is linear with respect to the PWM value (big assumption, need to
   * test).
   *
   * @return The angle in degrees to which the servo is set.
   */
  public double getAngle() {
    return impl.get() * getServoAngleRange() + kMinServoAngle;
  }

  /**
   * {@inheritDoc}
   */
  @Override
  public ITable getTable() {
    return m_table;
  }

  /*
   * Live Window code, only does anything if live window is activated.
   */
  public String getSmartDashboardType() {
    return "Servo";
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
  public void updateTable() {
    if (m_table != null) {
      m_table.putNumber("Value", get());
    }
  }

  /**
   * {@inheritDoc}
   */
  public void startLiveWindowMode() {
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
  public void stopLiveWindowMode() {
    // TODO: Broken, should only remove the listener from "Value" only.
    m_table.removeTableListener(m_table_listener);
  }
}
