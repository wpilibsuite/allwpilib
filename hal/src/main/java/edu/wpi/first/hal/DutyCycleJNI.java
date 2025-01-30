// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * DutyCycle HAL JNI functions.
 *
 * @see "DutyCycle.h"
 */
public class DutyCycleJNI extends JNIWrapper {
  /**
   * Initialize a DutyCycle input.
   *
   * @param digitalSourceHandle the digital source to use (either a Digital Handle or a
   *     AnalogTrigger Handle)
   * @param analogTriggerType the analog trigger type of the source if it is an analog trigger
   * @return the created duty cycle handle
   * @see "HAL_InitializeDutyCycle"
   */
  public static native int initialize(int digitalSourceHandle, int analogTriggerType);

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
   * <p><b>Warning: This will return inaccurate values for up to 2 seconds after the duty cycle
   * input is initialized.</b>
   *
   * @param handle the duty cycle handle
   * @return frequency in Hertz
   * @see "HAL_GetDutyCycleFrequency"
   */
  public static native int getFrequency(int handle);

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

  /**
   * Get the scale factor of the output.
   *
   * <p>An output equal to this value is always high, and then linearly scales down to 0. Divide a
   * raw result by this in order to get the percentage between 0 and 1. Used by DMA.
   *
   * @param handle the duty cycle handle
   * @return the output scale factor
   * @see "HAL_GetDutyCycleOutputScaleFactor"
   */
  public static native int getOutputScaleFactor(int handle);

  /**
   * Get the FPGA index for the DutyCycle.
   *
   * @param handle the duty cycle handle
   * @return the FPGA index
   * @see "HAL_GetDutyCycleFPGAIndex"
   */
  public static native int getFPGAIndex(int handle);

  /** Utility class. */
  private DutyCycleJNI() {}
}
