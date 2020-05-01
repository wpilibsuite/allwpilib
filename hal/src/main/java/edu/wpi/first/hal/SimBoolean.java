/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal;

/**
 * A wrapper around a simulator boolean value handle.
 */
public class SimBoolean extends SimValue {
  /**
   * Wraps a simulated value handle as returned by SimDeviceJNI.createSimValueBoolean().
   *
   * @param handle simulated value handle
   */
  public SimBoolean(int handle) {
    super(handle);
  }

  /**
   * Gets the simulated value.
   *
   * @return The current value
   */
  public boolean get() {
    return SimDeviceJNI.getSimValueBoolean(m_handle);
  }

  /**
   * Sets the simulated value.
   *
   * @param value the value to set
   */
  public void set(boolean value) {
    SimDeviceJNI.setSimValueBoolean(m_handle, value);
  }
}
