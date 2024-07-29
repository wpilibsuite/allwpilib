// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/** A wrapper around a simulator enum value handle. */
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
