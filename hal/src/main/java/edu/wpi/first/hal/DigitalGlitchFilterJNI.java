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
   * Sets the filter frequency on a digital input.
   *
   * <p>Sets the filter frequency in hertz.
   *
   * @param dioPortHandle the digital port handle
   * @param frequencyHertz the frequency below which the signal must not be counted as a transition.
   * @see "HAL_SetFilterFrequency"
   */
  public static native void setFilterFrequency(int dioPortHandle, double frequencyHertz);

  /**
   * Gets the filter frequency for the specified filter.
   *
   * <p>Gets the filter period in FPGA cycles. Set status to NiFpga_Status_SoftwareFault if the
   * filter values mismatch.
   *
   * @param dioPortHandle the digital port handle
   * @return The frequency of the filter in hertz.
   * @see "HAL_GetFilterFrequency"
   */
  public static native double getFilterFrequency(int dioPortHandle);

  /**
   * Removes a DIO from being filtered.
   *
   * @param dioPortHandle the digital port handle
   * @see "HAL_DisableFilter"
   */
  public static native void disableFilter(int dioPortHandle);
}
