/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.BufferCallback;
import edu.wpi.first.hal.simulation.ConstBufferCallback;
import edu.wpi.first.hal.simulation.I2CDataJNI;
import edu.wpi.first.hal.simulation.NotifyCallback;

public class I2CSim implements AutoCloseable {
  private final int m_bus;
  private final int m_deviceAddress;

  public I2CSim(int bus, int deviceAddress, I2CDevice device) {
    m_bus = bus;
    m_deviceAddress = deviceAddress;
    I2CDataJNI.registerDevice(bus, deviceAddress, device);
  }

  @Override
  public void close() {
    I2CDataJNI.unregisterDevice(m_bus, m_deviceAddress);
  }
}
