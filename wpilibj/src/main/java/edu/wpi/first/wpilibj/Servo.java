// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.SimDevice.Direction;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;
import edu.wpi.first.wpilibj.PWM.OutputPeriod;

/**
 * Standard hobby style servo.
 *
 * <p>The range parameters default to the appropriate values for the Hitec HS-322HD servo provided
 * in the FIRST Kit of Parts in 2008.
 */
public class Servo implements Sendable, AutoCloseable {
  private static final double kMaxServoAngle = 180.0;
  private static final double kMinServoAngle = 0.0;

  private static final int kDefaultMaxServoPWM = 2400;
  private static final int kDefaultMinServoPWM = 600;

  private final PWM m_pwm;

  private SimDevice m_simDevice;
  private SimDouble m_simPosition;

  private static final int m_minPwm = kDefaultMinServoPWM;
  private static final int m_maxPwm = kDefaultMaxServoPWM;

  /**
   * Constructor.
   *
   * <p>By default, {@value #kDefaultMaxServoPWM} ms is used as the max PWM value and {@value
   * #kDefaultMinServoPWM} ms is used as the minPWM value.
   *
   * @param channel The PWM channel to which the servo is attached. 0-9 are on-board, 10-19 are on
   *     the MXP port
   */
  @SuppressWarnings("this-escape")
  public Servo(final int channel) {
    m_pwm = new PWM(channel, false);
    SendableRegistry.add(this, "Servo", channel);

    m_pwm.setOutputPeriod(OutputPeriod.k20Ms);

    HAL.reportUsage("IO", channel, "Servo");

    m_simDevice = SimDevice.create("Servo", channel);
    if (m_simDevice != null) {
      m_simPosition = m_simDevice.createDouble("Position", Direction.kOutput, 0.0);
      m_pwm.setSimDevice(m_simDevice);
    }
  }

  /** Free the resource associated with the PWM channel and set the value to 0. */
  @Override
  public void close() {
    SendableRegistry.remove(this);
    m_pwm.close();

    if (m_simDevice != null) {
      m_simDevice.close();
      m_simDevice = null;
      m_simPosition = null;
    }
  }

  /**
   * Set the servo position.
   *
   * <p>Servo values range from 0.0 to 1.0 corresponding to the range of full left to full right.
   *
   * @param value Position from 0.0 to 1.0.
   */
  public void set(double value) {
    value = MathUtil.clamp(value, 0.0, 1.0);

    if (m_simPosition != null) {
      m_simPosition.set(value);
    }

    int rawValue = (int) ((value * getFullRangeScaleFactor()) + m_minPwm);

    m_pwm.setPulseTimeMicroseconds(rawValue);
  }

  /**
   * Get the servo position.
   *
   * <p>Servo values range from 0.0 to 1.0 corresponding to the range of full left to full right.
   * This returns the commanded position, not the position that the servo is actually at, as the
   * servo does not report its own position.
   *
   * @return Position from 0.0 to 1.0.
   */
  public double get() {
    int rawValue = m_pwm.getPulseTimeMicroseconds();

    if (rawValue < m_minPwm) {
      return 0.0;
    } else if (rawValue > m_maxPwm) {
      return 1.0;
    }
    return (rawValue - m_minPwm) / getFullRangeScaleFactor();
  }

  /**
   * Set the servo angle.
   *
   * <p>The angles are based on the HS-322HD Servo, and have a range of 0 to 180 degrees.
   *
   * <p>Servo angles that are out of the supported range of the servo simply "saturate" in that
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
   * <p>This returns the commanded angle, not the angle that the servo is actually at, as the servo
   * does not report its own angle.
   *
   * @return The angle in degrees to which the servo is set.
   */
  public double getAngle() {
    return get() * getServoAngleRange() + kMinServoAngle;
  }

  /**
   * Gets the backing PWM handle.
   *
   * @return The pwm handle.
   */
  public int getPwmHandle() {
    return m_pwm.getHandle();
  }

  /**
   * Gets the PWM channel number.
   *
   * @return The channel number.
   */
  public int getChannel() {
    return m_pwm.getChannel();
  }

  private double getFullRangeScaleFactor() {
    return m_maxPwm - m_minPwm;
  }

  private double getServoAngleRange() {
    return kMaxServoAngle - kMinServoAngle;
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Servo");
    builder.addDoubleProperty("Value", this::get, this::set);
  }
}
