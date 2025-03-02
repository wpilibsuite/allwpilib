// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.AnalogJNI;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * Analog channel class.
 *
 * <p>Each analog channel is read from hardware as a 12-bit number representing 0V to 5V.
 *
 * <p>Connected to each analog channel is an averaging and oversampling engine. This engine
 * accumulates the specified ( by setAverageBits() and setOversampleBits() ) number of samples
 * before returning a new value. This is not a sliding window average. The only difference between
 * the oversampled samples and the averaged samples is that the oversampled samples are simply
 * accumulated effectively increasing the resolution, while the averaged samples are divided by the
 * number of samples to retain the resolution, but get more stable values.
 */
public class AnalogInput implements Sendable, AutoCloseable {
  int m_port; // explicit no modifier, private and package accessible.
  private int m_channel;

  /**
   * Construct an analog channel.
   *
   * @param channel The channel number to represent. 0-3 are on-board 4-7 are on the MXP port.
   */
  @SuppressWarnings("this-escape")
  public AnalogInput(final int channel) {
    AnalogJNI.checkAnalogInputChannel(channel);
    m_channel = channel;

    m_port = AnalogJNI.initializeAnalogInputPort(channel);

    HAL.reportUsage("IO", channel, "AnalogInput");
    SendableRegistry.add(this, "AnalogInput", channel);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
    AnalogJNI.freeAnalogInputPort(m_port);
    m_port = 0;
    m_channel = 0;
  }

  /**
   * Get a sample straight from this channel. The sample is a 12-bit value representing the 0V to 5V
   * range of the A/D converter. The units are in A/D converter codes. Use GetVoltage() to get the
   * analog value in calibrated units.
   *
   * @return A sample straight from this channel.
   */
  public int getValue() {
    return AnalogJNI.getAnalogValue(m_port);
  }

  /**
   * Get a scaled sample straight from this channel. The value is scaled to units of Volts using the
   * calibrated scaling data from getLSBWeight() and getOffset().
   *
   * @return A scaled sample straight from this channel.
   */
  public double getVoltage() {
    return AnalogJNI.getAnalogVoltage(m_port);
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
    AnalogJNI.setAnalogInputSimDevice(m_port, device.getNativeHandle());
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Analog Input");
    builder.addDoubleProperty("Value", this::getVoltage, null);
  }
}
