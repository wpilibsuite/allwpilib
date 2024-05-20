// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * Digital Glitch Filter JNI functions.
 *
 * @see "hal/DIO.h"
 */
public class DigitalGlitchFilterJNI extends JNIWrapper {
  /**
   * Writes the filter index from the FPGA.
   *
   * <p>Set the filter index used to filter out short pulses.
   *
   * @param digitalPortHandle the digital port handle
   * @param filterIndex the filter index (Must be in the range 0 - 3, where 0 means "none" and 1 - 3
   *     means filter # filterIndex - 1)
   * @see "HAL_SetFilterSelect"
   */
  public static native void setFilterSelect(int digitalPortHandle, int filterIndex);

  /**
   * Reads the filter index from the FPGA.
   *
   * <p>Gets the filter index used to filter out short pulses.
   *
   * @param digitalPortHandle the digital port handle
   * @return the filter index (Must be in the range 0 - 3, where 0 means "none" and 1 - 3 means
   *     filter # filterIndex - 1)
   * @see "HAL_GetFilterSelect"
   */
  public static native int getFilterSelect(int digitalPortHandle);

  /**
   * Sets the filter period for the specified filter index.
   *
   * <p>Sets the filter period in FPGA cycles. Even though there are 2 different filter index
   * domains (MXP vs HDR), ignore that distinction for now since it complicates the interface. That
   * can be changed later.
   *
   * @param filterIndex the filter index, 0 - 2
   * @param fpgaCycles the number of cycles that the signal must not transition to be counted as a
   *     transition.
   * @see "HAL_SetFilterPeriod"
   */
  public static native void setFilterPeriod(int filterIndex, int fpgaCycles);

  /**
   * Gets the filter period for the specified filter index.
   *
   * <p>Gets the filter period in FPGA cycles. Even though there are 2 different filter index
   * domains (MXP vs HDR), ignore that distinction for now since it complicates the interface.
   *
   * @param filterIndex the filter index, 0 - 2
   * @return The number of FPGA cycles of the filter period.
   * @see "HAL_GetFilterPeriod"
   */
  public static native int getFilterPeriod(int filterIndex);

  /** Utility class. */
  private DigitalGlitchFilterJNI() {}
}
