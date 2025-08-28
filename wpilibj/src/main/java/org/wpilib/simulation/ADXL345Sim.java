// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import edu.wpi.first.hal.SimDouble;
import org.wpilib.ADXL345_I2C;
import java.util.Objects;

/** Class to control a simulated ADXL345. */
public class ADXL345Sim {
  private SimDouble m_simX;
  private SimDouble m_simY;
  private SimDouble m_simZ;

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

  /**
   * Sets the X acceleration.
   *
   * @param accel The X acceleration.
   */
  public void setX(double accel) {
    m_simX.set(accel);
  }

  /**
   * Sets the Y acceleration.
   *
   * @param accel The Y acceleration.
   */
  public void setY(double accel) {
    m_simY.set(accel);
  }

  /**
   * Sets the Z acceleration.
   *
   * @param accel The Z acceleration.
   */
  public void setZ(double accel) {
    m_simZ.set(accel);
  }
}
