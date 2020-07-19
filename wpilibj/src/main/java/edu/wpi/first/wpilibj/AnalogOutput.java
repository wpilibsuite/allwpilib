/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.AnalogJNI;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;
import edu.wpi.first.wpilibj.smartdashboard.SendableRegistry;

/**
 * Analog output class.
 */
public class AnalogOutput implements Sendable, AutoCloseable {
  private int m_port;
  private int m_channel;

  /**
   * Construct an analog output on a specified MXP channel.
   *
   * @param channel The channel number to represent.
   */
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
   */
  public int getChannel() {
    return m_channel;
  }

  public void setVoltage(double voltage) {
    AnalogJNI.setAnalogOutput(m_port, voltage);
  }

  public double getVoltage() {
    return AnalogJNI.getAnalogOutput(m_port);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Analog Output");
    builder.addDoubleProperty("Value", this::getVoltage, this::setVoltage);
  }
}
