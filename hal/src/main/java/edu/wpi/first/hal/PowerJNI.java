// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

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
   * Gets the roboRIO input current.
   *
   * @return the input current (amps)
   * @see "HAL_GetVinCurrent"
   */
  public static native double getVinCurrent();

  /**
   * Gets the 6V rail voltage.
   *
   * @return the 6V rail voltage (volts)
   * @see "HAL_GetUserVoltage6V"
   */
  public static native double getUserVoltage6V();

  /**
   * Gets the 6V rail current.
   *
   * @return the 6V rail current (amps)
   * @see "HAL_GetUserCurrent6V"
   */
  public static native double getUserCurrent6V();

  /**
   * Enables or disables the 6V rail.
   *
   * @param enabled whether the rail should be enabled
   */
  public static native void setUserEnabled6V(boolean enabled);

  /**
   * Gets the active state of the 6V rail.
   *
   * @return true if the rail is active, otherwise false
   * @see "HAL_GetUserActive6V"
   */
  public static native boolean getUserActive6V();

  /**
   * Gets the fault count for the 6V rail.
   *
   * @return the number of 6V fault counts
   * @see "HAL_GetUserCurrentFaults6V"
   */
  public static native int getUserCurrentFaults6V();

  /**
   * Gets the 5V rail voltage.
   *
   * @return the 5V rail voltage (volts)
   * @see "HAL_GetUserVoltage5V"
   */
  public static native double getUserVoltage5V();

  /**
   * Gets the 5V rail current.
   *
   * @return the 5V rail current (amps)
   * @see "HAL_GetUserCurrent5V"
   */
  public static native double getUserCurrent5V();

  /**
   * Enables or disables the 5V rail.
   *
   * @param enabled whether the rail should be enabled
   */
  public static native void setUserEnabled5V(boolean enabled);

  /**
   * Gets the active state of the 5V rail.
   *
   * @return true if the rail is active, otherwise false
   * @see "HAL_GetUserActive5V"
   */
  public static native boolean getUserActive5V();

  /**
   * Gets the fault count for the 5V rail.
   *
   * @return the number of 5V fault counts
   * @see "HAL_GetUserCurrentFaults5V"
   */
  public static native int getUserCurrentFaults5V();

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
