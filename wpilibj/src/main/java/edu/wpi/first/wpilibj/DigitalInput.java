// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.DIOJNI;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * Class to read a digital input. This class will read digital inputs and return the current value
 * on the channel. Other devices such as encoders, gear tooth sensors, etc. that are implemented
 * elsewhere will automatically allocate digital inputs and outputs as required. This class is only
 * for devices like switches etc. that aren't implemented anywhere else.
 */
public class DigitalInput extends DigitalSource implements Sendable {
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

    m_handle = DIOJNI.initializeDIOPort(HAL.getPort((byte) channel), true);

    HAL.report(tResourceType.kResourceType_DigitalInput, channel + 1);
    SendableRegistry.addLW(this, "DigitalInput", channel);
  }

  @Override
  public void close() {
    super.close();
    SendableRegistry.remove(this);
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
  @Override
  public int getChannel() {
    return m_channel;
  }

  /**
   * Get the analog trigger type.
   *
   * @return false
   */
  @Override
  public int getAnalogTriggerTypeForRouting() {
    return 0;
  }

  /**
   * Is this an analog trigger.
   *
   * @return true if this is an analog trigger
   */
  @Override
  public boolean isAnalogTrigger() {
    return false;
  }

  /**
   * Get the HAL Port Handle.
   *
   * @return The HAL Handle to the specified source.
   */
  @Override
  public int getPortHandleForRouting() {
    return m_handle;
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
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Digital Input");
    builder.addBooleanProperty("Value", this::get, null);
  }
}
