// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.wpilibj.ADXL345_I2C;
import edu.wpi.first.wpilibj.ADXL345_SPI;
import java.util.Objects;

public class ADXL345Sim {
  protected SimDouble m_simX;
  protected SimDouble m_simY;
  protected SimDouble m_simZ;

  /**
   * Constructor.
   *
   * @param device The device to simulate
   */
  public ADXL345Sim(ADXL345_SPI device) {
    SimDeviceSim simDevice = new SimDeviceSim("Accel:ADXL345_SPI" + "[" + device.getPort() + "]");
    initSim(simDevice);
  }

  /**
   * Constructor.
   *
   * @param device The device to simulate
   */
  public ADXL345Sim(ADXL345_I2C device) {
    SimDeviceSim simDevice =
        new SimDeviceSim(
            "Accel:ADXL345_I2C" + "[" + device.getPort() + "," + device.getDeviceAddress() + "]");
    initSim(simDevice);
  }

  private void initSim(SimDeviceSim simDevice) {
    Objects.requireNonNull(simDevice);

    m_simX = simDevice.getDouble("x");
    m_simY = simDevice.getDouble("y");
    m_simZ = simDevice.getDouble("z");

    Objects.requireNonNull(m_simX);
    Objects.requireNonNull(m_simY);
    Objects.requireNonNull(m_simZ);
  }

  public void setX(double x) {
    m_simX.set(x);
  }

  public void setY(double y) {
    m_simY.set(y);
  }

  public void setZ(double z) {
    m_simZ.set(z);
  }
}
