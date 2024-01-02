// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.wpilibj.ADXL362;
import java.util.Objects;

/** Class to control a simulated ADXL362. */
public class ADXL362Sim {
  private SimDouble m_simX;
  private SimDouble m_simY;
  private SimDouble m_simZ;

  /**
   * Constructor.
   *
   * @param device The device to simulate
   */
  public ADXL362Sim(ADXL362 device) {
    SimDeviceSim wrappedSimDevice =
        new SimDeviceSim("Accel:ADXL362" + "[" + device.getPort() + "]");
    initSim(wrappedSimDevice);
  }

  private void initSim(SimDeviceSim wrappedSimDevice) {
    m_simX = wrappedSimDevice.getDouble("x");
    m_simY = wrappedSimDevice.getDouble("y");
    m_simZ = wrappedSimDevice.getDouble("z");

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
