/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal;

/**
 * A wrapper around a simulator enum value handle.
 */
public class SimEnum extends SimValue {
  /**
   * Wraps a simulated value handle as returned by SimDeviceJNI.createSimValueEnum().
   *
   * @param handle simulated value handle
   */
  public SimEnum(int handle) {
    super(handle);
  }

  /**
   * Gets the simulated value.
   *
   * @return The current value
   */
  public int get() {
    return SimDeviceJNI.getSimValueEnum(m_handle);
  }

  /**
   * Sets the simulated value.
   *
   * @param value the value to set
   */
  public void set(int value) {
    SimDeviceJNI.setSimValueEnum(m_handle, value);
  }
}
