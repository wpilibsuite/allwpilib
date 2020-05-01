/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal;

/**
 * A wrapper around a simulator double value handle.
 */
public class SimDouble extends SimValue {
  /**
   * Wraps a simulated value handle as returned by SimDeviceJNI.createSimValueDouble().
   *
   * @param handle simulated value handle
   */
  public SimDouble(int handle) {
    super(handle);
  }

  /**
   * Gets the simulated value.
   *
   * @return The current value
   */
  public double get() {
    return SimDeviceJNI.getSimValueDouble(m_handle);
  }

  /**
   * Sets the simulated value.
   *
   * @param value the value to set
   */
  public void set(double value) {
    SimDeviceJNI.setSimValueDouble(m_handle, value);
  }
}
