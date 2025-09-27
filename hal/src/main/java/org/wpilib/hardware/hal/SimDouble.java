// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/** A wrapper around a simulator double value handle. */
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

  /**
   * Resets the simulated value to 0. Use this instead of Set(0) for resetting incremental sensor
   * values like encoder counts or gyro accumulated angle to ensure correct behavior in a
   * distributed system (e.g. WebSockets).
   */
  public void reset() {
    SimDeviceJNI.resetSimValue(m_handle);
  }
}
