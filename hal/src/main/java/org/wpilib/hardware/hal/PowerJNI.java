// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/**
 * Power HAL JNI Functions.
 *
 * @see "Power.h"
 */
public class PowerJNI extends JNIWrapper {
  /**
   * Gets the roboRIO input voltage.
   *
   * @return the input voltage (volts)
   * @see "HAL_GetVinVoltage"
   */
  public static native double getVinVoltage();

  /**
   * Gets the 3V3 rail voltage.
   *
   * @return the 3V3 rail voltage (volts)
   * @see "HAL_GetUserVoltage3V3"
   */
  public static native double getUserVoltage3V3();

  /**
   * Gets the 3V3 rail current.
   *
   * @return the 3V3 rail current (amps)
   * @see "HAL_GetUserCurrent3V3"
   */
  public static native double getUserCurrent3V3();

  /**
   * Enables or disables the 3V3 rail.
   *
   * @param enabled whether the rail should be enabled
   */
  public static native void setUserEnabled3V3(boolean enabled);

  /**
   * Gets the active state of the 3V3 rail.
   *
   * @return true if the rail is active, otherwise false
   * @see "HAL_GetUserActive3V3"
   */
  public static native boolean getUserActive3V3();

  /**
   * Gets the fault count for the 3V3 rail.
   *
   * @return the number of 3V3 fault counts
   * @see "HAL_GetUserCurrentFaults3V3"
   */
  public static native int getUserCurrentFaults3V3();

  /** Resets the overcurrent fault counters for all user rails to 0. */
  public static native void resetUserCurrentFaults();

  /**
   * Set the voltage the roboRIO will brownout and disable all outputs.
   *
   * <p>Note that this only does anything on the roboRIO 2. On the roboRIO it is a no-op.
   *
   * @param voltage The brownout voltage
   * @see "HAL_SetBrownoutVoltage"
   */
  public static native void setBrownoutVoltage(double voltage);

  /**
   * Get the current brownout voltage setting.
   *
   * @return The brownout voltage
   * @see "HAL_GetBrownoutVoltage"
   */
  public static native double getBrownoutVoltage();

  /**
   * Get the current CPU temperature in degrees Celsius.
   *
   * @return current CPU temperature in degrees Celsius
   */
  public static native double getCPUTemp();

  /** Utility class. */
  private PowerJNI() {}
}
