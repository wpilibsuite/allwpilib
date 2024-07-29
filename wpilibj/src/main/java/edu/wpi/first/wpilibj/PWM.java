// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.PWMConfigDataResult;
import edu.wpi.first.hal.PWMJNI;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * Class implements the PWM generation in the FPGA.
 *
 * <p>The values supplied as arguments for PWM outputs range from -1.0 to 1.0. They are mapped to
 * the microseconds to keep the pulse high, with a range of 0 (off) to 4096. Changes are immediately
 * sent to the FPGA, and the update occurs at the next FPGA cycle (5.05ms). There is no delay.
 */
public class PWM implements Sendable, AutoCloseable {
  /** Represents the amount to multiply the minimum servo-pulse pwm period by. */
  public enum PeriodMultiplier {
    /** Period Multiplier: don't skip pulses. PWM pulses occur every 5.05 ms */
    k1X,
    /** Period Multiplier: skip every other pulse. PWM pulses occur every 10.10 ms */
    k2X,
    /** Period Multiplier: skip three out of four pulses. PWM pulses occur every 20.20 ms */
    k4X
  }

  private final int m_channel;

  private int m_handle;

  /**
   * Allocate a PWM given a channel.
   *
   * <p>Checks channel value range and allocates the appropriate channel. The allocation is only
   * done to help users ensure that they don't double assign channels.
   *
   * <p>By default, adds itself to SendableRegistry and LiveWindow.
   *
   * @param channel The PWM channel number. 0-9 are on-board, 10-19 are on the MXP port
   */
  public PWM(final int channel) {
    this(channel, true);
  }

  /**
   * Allocate a PWM given a channel.
   *
   * @param channel The PWM channel number. 0-9 are on-board, 10-19 are on the MXP port
   * @param registerSendable If true, adds this instance to SendableRegistry and LiveWindow
   */
  @SuppressWarnings("this-escape")
  public PWM(final int channel, final boolean registerSendable) {
    SensorUtil.checkPWMChannel(channel);
    m_channel = channel;

    m_handle = PWMJNI.initializePWMPort(HAL.getPort((byte) channel));

    setDisabled();

    PWMJNI.setPWMEliminateDeadband(m_handle, false);

    HAL.report(tResourceType.kResourceType_PWM, channel + 1);
    if (registerSendable) {
      SendableRegistry.addLW(this, "PWM", channel);
    }
  }

  /** Free the resource associated with the PWM channel and set the value to 0. */
  @Override
  public void close() {
    SendableRegistry.remove(this);
    if (m_handle == 0) {
      return;
    }
    setDisabled();
    PWMJNI.freePWMPort(m_handle);
    m_handle = 0;
  }

  /**
   * Optionally eliminate the deadband from a motor controller.
   *
   * @param eliminateDeadband If true, set the motor curve for the motor controller to eliminate the
   *     deadband in the middle of the range. Otherwise, keep the full range without modifying any
   *     values.
   */
  public void enableDeadbandElimination(boolean eliminateDeadband) {
    PWMJNI.setPWMEliminateDeadband(m_handle, eliminateDeadband);
  }

  /**
   * Set the bounds on the PWM pulse widths. This sets the bounds on the PWM values for a particular
   * type of controller. The values determine the upper and lower speeds as well as the deadband
   * bracket.
   *
   * @param max The max PWM pulse width in us
   * @param deadbandMax The high end of the deadband range pulse width in us
   * @param center The center (off) pulse width in us
   * @param deadbandMin The low end of the deadband pulse width in us
   * @param min The minimum pulse width in us
   */
  public void setBoundsMicroseconds(
      int max, int deadbandMax, int center, int deadbandMin, int min) {
    PWMJNI.setPWMConfigMicroseconds(m_handle, max, deadbandMax, center, deadbandMin, min);
  }

  /**
   * Gets the bounds on the PWM pulse widths. This gets the bounds on the PWM values for a
   * particular type of controller. The values determine the upper and lower speeds as well as the
   * deadband bracket.
   *
   * @return The bounds on the PWM pulse widths.
   */
  public PWMConfigDataResult getBoundsMicroseconds() {
    return PWMJNI.getPWMConfigMicroseconds(m_handle);
  }

  /**
   * Gets the channel number associated with the PWM Object.
   *
   * @return The channel number.
   */
  public int getChannel() {
    return m_channel;
  }

  /**
   * Set the PWM value based on a position.
   *
   * <p>This is intended to be used by servos.
   *
   * @param pos The position to set the servo between 0.0 and 1.0.
   * @pre setBoundsMicroseconds() called.
   */
  public void setPosition(double pos) {
    PWMJNI.setPWMPosition(m_handle, pos);
  }

  /**
   * Get the PWM value in terms of a position.
   *
   * <p>This is intended to be used by servos.
   *
   * @return The position the servo is set to between 0.0 and 1.0.
   * @pre setBoundsMicroseconds() called.
   */
  public double getPosition() {
    return PWMJNI.getPWMPosition(m_handle);
  }

  /**
   * Set the PWM value based on a speed.
   *
   * <p>This is intended to be used by motor controllers.
   *
   * @param speed The speed to set the motor controller between -1.0 and 1.0.
   * @pre setBoundsMicroseconds() called.
   */
  public void setSpeed(double speed) {
    PWMJNI.setPWMSpeed(m_handle, speed);
  }

  /**
   * Get the PWM value in terms of speed.
   *
   * <p>This is intended to be used by motor controllers.
   *
   * @return The most recently set speed between -1.0 and 1.0.
   * @pre setBoundsMicroseconds() called.
   */
  public double getSpeed() {
    return PWMJNI.getPWMSpeed(m_handle);
  }

  /**
   * Set the PWM value directly to the hardware.
   *
   * <p>Write a microsecond pulse value to a PWM channel.
   *
   * @param microsecondPulseTime Microsecond pulse PWM value. Range 0 - 4096.
   */
  public void setPulseTimeMicroseconds(int microsecondPulseTime) {
    PWMJNI.setPulseTimeMicroseconds(m_handle, microsecondPulseTime);
  }

  /**
   * Get the PWM value directly from the hardware.
   *
   * <p>Read a raw value from a PWM channel.
   *
   * @return Microsecond pulse PWM control value. Range: 0 - 4096.
   */
  public int getPulseTimeMicroseconds() {
    return PWMJNI.getPulseTimeMicroseconds(m_handle);
  }

  /** Temporarily disables the PWM output. The next set call will re-enable the output. */
  public final void setDisabled() {
    PWMJNI.setPWMDisabled(m_handle);
  }

  /**
   * Slow down the PWM signal for old devices.
   *
   * @param mult The period multiplier to apply to this channel
   */
  public void setPeriodMultiplier(PeriodMultiplier mult) {
    int scale =
        switch (mult) {
          case k4X -> 3; // Squelch 3 out of 4 outputs
          case k2X -> 1; // Squelch 1 out of 2 outputs
          case k1X -> 0; // Don't squelch any outputs
        };

    PWMJNI.setPWMPeriodScale(m_handle, scale);
  }

  /** Latches PWM to zero. */
  public void setZeroLatch() {
    PWMJNI.latchPWMZero(m_handle);
  }

  /** Sets the PWM output to be a continuous high signal while enabled. */
  public void setAlwaysHighMode() {
    PWMJNI.setAlwaysHighMode(m_handle);
  }

  /**
   * Get the underlying handle.
   *
   * @return Underlying PWM handle
   */
  public int getHandle() {
    return m_handle;
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("PWM");
    builder.setActuator(true);
    builder.setSafeState(this::setDisabled);
    builder.addDoubleProperty(
        "Value", this::getPulseTimeMicroseconds, value -> setPulseTimeMicroseconds((int) value));
    builder.addDoubleProperty("Speed", this::getSpeed, this::setSpeed);
    builder.addDoubleProperty("Position", this::getPosition, this::setPosition);
  }
}
