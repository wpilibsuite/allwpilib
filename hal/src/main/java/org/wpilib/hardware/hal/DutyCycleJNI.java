// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/**
 * DutyCycle HAL JNI functions.
 *
 * @see "DutyCycle.h"
 */
public class DutyCycleJNI extends JNIWrapper {
  /**
   * Initialize a DutyCycle input.
   *
   * @param channel the smartio channel
   * @return the created duty cycle handle
   * @see "HAL_InitializeDutyCycle"
   */
  public static native int initialize(int channel);

  /**
   * Free a DutyCycle.
   *
   * @param handle the duty cycle handle
   * @see "HAL_FreeDutyCycle"
   */
  public static native void free(int handle);

  /**
   * Get the frequency of the duty cycle signal.
   *
   * @param handle the duty cycle handle
   * @return frequency in Hertz
   * @see "HAL_GetDutyCycleFrequency"
   */
  public static native double getFrequency(int handle);

  /**
   * Get the output ratio of the duty cycle signal.
   *
   * <p>0 means always low, 1 means always high.
   *
   * @param handle the duty cycle handle
   * @return output ratio between 0 and 1
   * @see "HAL_GetDutyCycleOutput"
   */
  public static native double getOutput(int handle);

  /**
   * Get the raw high time of the duty cycle signal.
   *
   * @param handle the duty cycle handle
   * @return high time of last pulse in nanoseconds
   * @see "HAL_GetDutyCycleHighTime"
   */
  public static native int getHighTime(int handle);

  /** Utility class. */
  private DutyCycleJNI() {}
}
