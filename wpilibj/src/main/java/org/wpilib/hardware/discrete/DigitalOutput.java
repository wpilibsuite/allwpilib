// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.discrete;

import org.wpilib.hardware.hal.DIOJNI;
import org.wpilib.hardware.hal.SimDevice;
import org.wpilib.telemetry.TelemetryLoggable;
import org.wpilib.telemetry.TelemetryTable;
import org.wpilib.util.UsageReporting;

/**
 * Class to write digital outputs. This class will write digital outputs. Other devices that are
 * implemented elsewhere will automatically allocate digital inputs and outputs as required.
 */
public class DigitalOutput implements AutoCloseable, TelemetryLoggable {
  private final int m_channel;
  private int m_handle;

  /**
   * Create an instance of a digital output. Create an instance of a digital output given a channel.
   *
   * @param channel the SmartIO channel to use for the digital output.
   */
  public DigitalOutput(int channel) {
    m_channel = channel;

    m_handle = DIOJNI.initializeDIOPort(channel, false);

    UsageReporting.reportUsage("IO", channel, "DigitalOutput");
  }

  @Override
  public void close() {
    DIOJNI.freeDIOPort(m_handle);
    m_handle = 0;
  }

  /**
   * Set the value of a digital output.
   *
   * @param value true is on, off is false
   */
  public void set(boolean value) {
    DIOJNI.setDIO(m_handle, value);
  }

  /**
   * Gets the value being output from the Digital Output.
   *
   * @return the state of the digital output.
   */
  public boolean get() {
    return DIOJNI.getDIO(m_handle);
  }

  /**
   * Get the GPIO channel number that this object represents.
   *
   * @return The GPIO channel number.
   */
  public int getChannel() {
    return m_channel;
  }

  /**
   * Indicates this input is used by a simulated device.
   *
   * @param device simulated device handle
   */
  public void setSimDevice(SimDevice device) {
    DIOJNI.setDIOSimDevice(m_handle, device.getNativeHandle());
  }

  @Override
  public void logTo(TelemetryTable table) {
    table.log("Value", get());
  }

  @Override
  public String getTelemetryType() {
    return "Digital Output";
  }
}
