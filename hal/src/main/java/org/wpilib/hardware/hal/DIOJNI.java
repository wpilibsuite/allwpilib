// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/**
 * Digital Input/Output (IO) JNI Functions.
 *
 * @see "wpi/hal/DIO.h"
 */
public class DIOJNI extends JNIWrapper {
  /**
   * Creates a new instance of a digital port.
   *
   * @param channel the smartio channel
   * @param input true for input, false for output
   * @return the created digital handle
   * @see "HAL_InitializeDIOPort"
   */
  public static native int initializeDIOPort(int channel, boolean input);

  /**
   * Checks if a DIO channel is valid.
   *
   * @param channel the channel number to check
   * @return true if the channel is valid, otherwise false
   * @see "HAL_CheckDIOChannel"
   */
  public static native boolean checkDIOChannel(int channel);

  /**
   * Frees a DIO port.
   *
   * @param dioPortHandle the DIO channel handle
   * @see "HAL_FreeDIOPort"
   */
  public static native void freeDIOPort(int dioPortHandle);

  /**
   * Indicates the DIO channel is used by a simulated device.
   *
   * @param handle the DIO channel handle
   * @param device simulated device handle
   * @see "HAL_SetDIOSimDevice"
   */
  public static native void setDIOSimDevice(int handle, int device);

  /**
   * Writes a digital value to a DIO channel.
   *
   * @param dioPortHandle the digital port handle
   * @param value the state to set the digital channel (if it is configured as an output)
   * @see "HAL_SetDIO"
   */
  public static native void setDIO(int dioPortHandle, boolean value);

  /**
   * Sets the direction of a DIO channel.
   *
   * @param dioPortHandle the digital port handle
   * @param input true to set input, false for output
   * @see "HAL_SetDIODirection"
   */
  public static native void setDIODirection(int dioPortHandle, boolean input);

  /**
   * Reads a digital value from a DIO channel.
   *
   * @param dioPortHandle the digital port handle
   * @return the state of the specified channel
   * @see "HAL_GetDIO"
   */
  public static native boolean getDIO(int dioPortHandle);

  /**
   * Reads the direction of a DIO channel.
   *
   * @param dioPortHandle the digital port handle
   * @return true for input, false for output
   * @see "HAL_GetDIODirection"
   */
  public static native boolean getDIODirection(int dioPortHandle);

  /** Utility class. */
  DIOJNI() {}
}
