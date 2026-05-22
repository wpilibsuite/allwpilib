// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.discrete;

import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.PWMJNI;
import org.wpilib.hardware.hal.SimDevice;
import org.wpilib.util.sendable.Sendable;
import org.wpilib.util.sendable.SendableBuilder;
import org.wpilib.util.sendable.SendableRegistry;

/**
 * Class implements the PWM generation in the FPGA.
 *
 * <p>The values supplied as arguments for PWM outputs range from -1.0 to 1.0. They are mapped to
 * the microseconds to keep the pulse high, with a range of 0 (off) to 4096. Changes are immediately
 * sent to the FPGA, and the update occurs at the next FPGA cycle (5.05ms). There is no delay.
 */
public class PWM implements Sendable, AutoCloseable {
  private final int m_channel;

  private int m_handle;

  /**
   * Allocate a PWM given a channel.
   *
   * <p>Checks channel value range and allocates the appropriate channel. The allocation is only
   * done to help users ensure that they don't double assign channels.
   *
   * <p>By default, adds itself to SendableRegistry.
   *
   * @param channel The SmartIO channel number.
   */
  public PWM(final int channel) {
    this(channel, true);
  }

  /**
   * Allocate a PWM given a channel.
   *
   * @param channel The SmartIO channel number.
   * @param registerSendable If true, adds this instance to SendableRegistry
   */
  @SuppressWarnings("this-escape")
  public PWM(final int channel, final boolean registerSendable) {
    m_channel = channel;

    m_handle = PWMJNI.initializePWMPort(channel);

    setDisabled();

    HAL.reportUsage("IO", channel, "PWM");
    if (registerSendable) {
      SendableRegistry.add(this, "PWM", channel);
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
   * Gets the channel number associated with the PWM Object.
   *
   * @return The channel number.
   */
  public int getChannel() {
    return m_channel;
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
    setPulseTimeMicroseconds(0);
  }

  /**
   * Sets the PWM output period.
   *
   * @param millisecondPeriod The output period to apply to this channel, in milliseconds. Valid
   *     values are 5ms, 10ms, and 20ms. Default is 20 ms.
   */
  public void setOutputPeriod(int millisecondPeriod) {
    int scale =
        switch (millisecondPeriod) {
          case 5 -> 0;
          case 10 -> 1;
          case 20 -> 3;
          default -> 3; // default to 20ms if invalid value is given
        };

    PWMJNI.setPWMOutputPeriod(m_handle, scale);
  }

  /**
   * Get the underlying handle.
   *
   * @return Underlying PWM handle
   */
  public int getHandle() {
    return m_handle;
  }

  /**
   * Indicates this input is used by a simulated device.
   *
   * @param device simulated device handle
   */
  public void setSimDevice(SimDevice device) {
    PWMJNI.setPWMSimDevice(m_handle, device.getNativeHandle());
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("PWM");
    builder.setActuator(true);
    builder.addDoubleProperty(
        "Value", this::getPulseTimeMicroseconds, value -> setPulseTimeMicroseconds((int) value));
  }
}
