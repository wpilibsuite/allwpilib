// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * SystemServer JNI Functions.
 *
 * @see "SystemServer.h"
 */
public class SystemServerJNI extends JNIWrapper {
  /**
   * Gets the system server NT handle.
   *
   * @return The system server NT handle
   * @see "HAL_GetSystemServerHandle"
   */
  public static native int getSystemServerHandle();

  /** Utility class. */
  private SystemServerJNI() {}
}
