// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.DIOJNI;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.telemetry.TelemetryLoggable;
import edu.wpi.first.telemetry.TelemetryTable;

/**
 * Class to read a digital input. This class will read digital inputs and return the current value
 * on the channel. Other devices such as encoders, gear tooth sensors, etc. that are implemented
 * elsewhere will automatically allocate digital inputs and outputs as required. This class is only
 * for devices like switches etc. that aren't implemented anywhere else.
 */
public class DigitalInput implements AutoCloseable, TelemetryLoggable {
  private final int m_channel;
  private int m_handle;

  /**
   * Create an instance of a Digital Input class. Creates a digital input given a channel.
   *
   * @param channel the DIO channel for the digital input 0-9 are on-board, 10-25 are on the MXP
   */
  public DigitalInput(int channel) {
    SensorUtil.checkDigitalChannel(channel);
    m_channel = channel;

    m_handle = DIOJNI.initializeDIOPort(channel, true);

    HAL.reportUsage("IO", channel, "DigitalInput");
  }

  @Override
  public void close() {
    DIOJNI.freeDIOPort(m_handle);
    m_handle = 0;
  }

  /**
   * Get the value from a digital input channel. Retrieve the value of a single digital input
   * channel from the FPGA.
   *
   * @return the status of the digital input
   */
  public boolean get() {
    return DIOJNI.getDIO(m_handle);
  }

  /**
   * Get the channel of the digital input.
   *
   * @return The GPIO channel number that this object represents.
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
  public void updateTelemetry(TelemetryTable table) {
    table.log("Value", get());
  }

  @Override
  public String getTelemetryType() {
    return "Digital Input";
  }
}
