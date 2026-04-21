// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.discrete;

import org.wpilib.hardware.hal.AnalogInputJNI;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.SimDevice;
import org.wpilib.util.sendable.Sendable;
import org.wpilib.util.sendable.SendableBuilder;
import org.wpilib.util.sendable.SendableRegistry;

/**
 * Analog channel class.
 *
 * <p>Each analog channel is read from hardware as a 12-bit number representing 0V to 3.3V.
 */
public class AnalogInput implements Sendable, AutoCloseable {
  int m_port; // explicit no modifier, private and package accessible.
  private int m_channel;

  /**
   * Construct an analog channel.
   *
   * @param channel The SmartIO channel for the analog input.
   */
  @SuppressWarnings("this-escape")
  public AnalogInput(final int channel) {
    AnalogInputJNI.checkAnalogInputChannel(channel);
    m_channel = channel;

    m_port = AnalogInputJNI.initializeAnalogInputPort(channel);

    HAL.reportUsage("IO", channel, "AnalogInput");
    SendableRegistry.add(this, "AnalogInput", channel);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
    AnalogInputJNI.freeAnalogInputPort(m_port);
    m_port = 0;
    m_channel = 0;
  }

  /**
   * Get a sample straight from this channel. The sample is a 12-bit value representing the 0V to
   * 3.3V range of the A/D converter. The units are in A/D converter codes. Use GetVoltage() to get
   * the analog value in calibrated units.
   *
   * @return A sample straight from this channel.
   */
  public int getValue() {
    return AnalogInputJNI.getAnalogValue(m_port);
  }

  /**
   * Get a scaled sample straight from this channel. The value is scaled to units of Volts.
   *
   * @return A scaled sample straight from this channel.
   */
  public double getVoltage() {
    return AnalogInputJNI.getAnalogVoltage(m_port);
  }

  /**
   * Get the channel number.
   *
   * @return The channel number.
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
    AnalogInputJNI.setAnalogInputSimDevice(m_port, device.getNativeHandle());
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Analog Input");
    builder.addDoubleProperty("Value", this::getVoltage, null);
  }
}
