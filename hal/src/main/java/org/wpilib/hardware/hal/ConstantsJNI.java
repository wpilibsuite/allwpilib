// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/**
 * Constants HAL JNI functions.
 *
 * @see "hal/Constants.h"
 */
public class ConstantsJNI extends JNIWrapper {
  /**
   * Gets the number of FPGA system clock ticks per microsecond.
   *
   * @return the number of clock ticks per microsecond
   * @see "HAL_GetSystemClockTicksPerMicrosecond"
   */
  public static native int getSystemClockTicksPerMicrosecond();

  /** Utility class. */
  private ConstantsJNI() {}
}
