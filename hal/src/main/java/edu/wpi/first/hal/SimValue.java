// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/** A wrapper around a simulator value handle. */
public class SimValue {
  /**
   * Wraps a simulated value handle as returned by SimDeviceJNI.createSimValue().
   *
   * @param handle simulated value handle
   */
  public SimValue(int handle) {
    m_handle = handle;
  }

  /**
   * Get the internal device handle.
   *
   * @return internal handle
   */
  public int getNativeHandle() {
    return m_handle;
  }

  /**
   * Gets the simulated value.
   *
   * @return The current value
   */
  public HALValue getValue() {
    return SimDeviceJNI.getSimValue(m_handle);
  }

  /**
   * Sets the simulated value.
   *
   * @param value the value to set
   */
  public void setValue(HALValue value) {
    SimDeviceJNI.setSimValue(m_handle, value);
  }

  /** The native handle for this SimValue. */
  protected final int m_handle;
}
