// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.PWMJNI;
import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.telemetry.TelemetryLoggable;
import edu.wpi.first.telemetry.TelemetryTable;

/**
 * Class implements the PWM generation in the FPGA.
 *
 * <p>The values supplied as arguments for PWM outputs range from -1.0 to 1.0. They are mapped to
 * the microseconds to keep the pulse high, with a range of 0 (off) to 4096. Changes are immediately
 * sent to the FPGA, and the update occurs at the next FPGA cycle (5.05ms). There is no delay.
 */
public class PWM implements TelemetryLoggable, AutoCloseable {
  /** Represents the output period in microseconds. */
  public enum OutputPeriod {
    /** Pulse every 5ms. */
    k5Ms,
    /** Pulse every 10ms. */
    k10Ms,
    /** Pulse every 20ms. */
    k20Ms
  }

  private final int m_channel;

  private int m_handle;

  /**
   * Allocate a PWM given a channel.
   *
   * <p>Checks channel value range and allocates the appropriate channel. The allocation is only
   * done to help users ensure that they don't double assign channels.
   *
   * @param channel The PWM channel number. 0-9 are on-board, 10-19 are on the MXP port
   */
  public PWM(final int channel) {
    SensorUtil.checkPWMChannel(channel);
    m_channel = channel;

    m_handle = PWMJNI.initializePWMPort(channel);

    setDisabled();

    HAL.reportUsage("IO", channel, "PWM");
  }

  /** Free the resource associated with the PWM channel and set the value to 0. */
  @Override
  public void close() {
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
  public final void setPulseTimeMicroseconds(int microsecondPulseTime) {
    PWMJNI.setPulseTimeMicroseconds(m_handle, microsecondPulseTime);
  }

  /**
   * Get the PWM value directly from the hardware.
   *
   * <p>Read a raw value from a PWM channel.
   *
   * @return Microsecond pulse PWM control value. Range: 0 - 4096.
   */
  public final int getPulseTimeMicroseconds() {
    return PWMJNI.getPulseTimeMicroseconds(m_handle);
  }

  /** Temporarily disables the PWM output. The next set call will re-enable the output. */
  public final void setDisabled() {
    setPulseTimeMicroseconds(0);
  }

  /**
   * Sets the PWM output period.
   *
   * @param mult The output period to apply to this channel
   */
  public void setOutputPeriod(OutputPeriod mult) {
    int scale =
        switch (mult) {
          case k20Ms -> 3;
          case k10Ms -> 1;
          case k5Ms -> 0;
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
  public void updateTelemetry(TelemetryTable table) {
    table.log("Value", getPulseTimeMicroseconds());
  }

  @Override
  public String getTelemetryType() {
    return "PWM";
  }
}
