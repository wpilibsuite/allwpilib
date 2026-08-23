// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.accelerometer;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import org.wpilib.hardware.discrete.AnalogInput;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.telemetry.TelemetryLoggable;
import org.wpilib.telemetry.TelemetryTable;

/**
 * Handle operation of an analog accelerometer. The accelerometer reads acceleration directly
 * through the sensor. Many sensors have multiple axis and can be treated as multiple devices. Each
 * is calibrated by finding the center value over a period of time.
 */
public class AnalogAccelerometer implements TelemetryLoggable, AutoCloseable {
  private AnalogInput m_analogChannel;
  private double m_voltsPerG = 1.0;
  private double m_zeroGVoltage = 2.5;
  private final boolean m_allocatedChannel;

  /** Common initialization. */
  private void initAccelerometer() {
    HAL.reportUsage("IO", m_analogChannel.getChannel(), "Accelerometer");
  }

  /**
   * Create a new instance of an accelerometer.
   *
   * <p>The constructor allocates desired analog channel.
   *
   * @param channel The channel number for the analog input the accelerometer is connected to
   */
  public AnalogAccelerometer(final int channel) {
    this(new AnalogInput(channel), true);
  }

  /**
   * Create a new instance of Accelerometer from an existing AnalogChannel. Make a new instance of
   * accelerometer given an AnalogChannel. This is particularly useful if the port is going to be
   * read as an analog channel as well as through the Accelerometer class.
   *
   * @param channel The existing AnalogInput object for the analog input the accelerometer is
   *     connected to
   */
  public AnalogAccelerometer(final AnalogInput channel) {
    this(channel, false);
  }

  private AnalogAccelerometer(final AnalogInput channel, final boolean allocatedChannel) {
    requireNonNullParam(channel, "channel", "AnalogAccelerometer");
    m_allocatedChannel = allocatedChannel;
    m_analogChannel = channel;
    initAccelerometer();
  }

  /** Delete the analog components used for the accelerometer. */
  @Override
  public void close() {
    if (m_analogChannel != null && m_allocatedChannel) {
      m_analogChannel.close();
    }
    m_analogChannel = null;
  }

  /**
   * Return the acceleration in Gs.
   *
   * <p>The acceleration is returned units of Gs.
   *
   * @return The current acceleration of the sensor in Gs.
   */
  public double getAcceleration() {
    if (m_analogChannel == null) {
      return 0.0;
    }
    return (m_analogChannel.getVoltage() - m_zeroGVoltage) / m_voltsPerG;
  }

  /**
   * Set the accelerometer sensitivity.
   *
   * <p>This sets the sensitivity of the accelerometer used for calculating the acceleration. The
   * sensitivity varies by accelerometer model.
   *
   * @param sensitivity The sensitivity of accelerometer in Volts per G.
   */
  public void setSensitivity(double sensitivity) {
    m_voltsPerG = sensitivity;
  }

  /**
   * Set the voltage that corresponds to 0 G.
   *
   * <p>The zero G voltage varies by accelerometer model.
   *
   * @param zero The zero G voltage.
   */
  public void setZero(double zero) {
    m_zeroGVoltage = zero;
  }

  @Override
  public void logTo(TelemetryTable table) {
    table.log("Value", getAcceleration());
  }

  @Override
  public String getTelemetryType() {
    return "Accelerometer";
  }
}
