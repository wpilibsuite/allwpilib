// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/** A wrapper around a simulator boolean value handle. */
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
