// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.AnalogJNI;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/** Analog output class. */
public class AnalogOutput implements Sendable, AutoCloseable {
  private int m_port;
  private int m_channel;

  /**
   * Construct an analog output on a specified MXP channel.
   *
   * @param channel The channel number to represent.
   */
  @SuppressWarnings("this-escape")
  public AnalogOutput(final int channel) {
    SensorUtil.checkAnalogOutputChannel(channel);
    m_channel = channel;

    final int portHandle = HAL.getPort((byte) channel);
    m_port = AnalogJNI.initializeAnalogOutputPort(portHandle);

    HAL.report(tResourceType.kResourceType_AnalogOutput, channel + 1);
    SendableRegistry.addLW(this, "AnalogOutput", channel);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
    AnalogJNI.freeAnalogOutputPort(m_port);
    m_port = 0;
    m_channel = 0;
  }

  /**
   * Get the channel of this AnalogOutput.
   *
   * @return The channel of this AnalogOutput.
   */
  public int getChannel() {
    return m_channel;
  }

  /**
   * Set the value of the analog output.
   *
   * @param voltage The output value in Volts, from 0.0 to +5.0.
   */
  public void setVoltage(double voltage) {
    AnalogJNI.setAnalogOutput(m_port, voltage);
  }

  /**
   * Get the voltage of the analog output.
   *
   * @return The value in Volts, from 0.0 to +5.0.
   */
  public double getVoltage() {
    return AnalogJNI.getAnalogOutput(m_port);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Analog Output");
    builder.addDoubleProperty("Value", this::getVoltage, this::setVoltage);
  }
}
