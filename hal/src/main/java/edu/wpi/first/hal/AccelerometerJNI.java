// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * Accelerometer HAL JNI methods.
 *
 * @see "hal/Accelerometer.h"
 */
public class AccelerometerJNI extends JNIWrapper {
  /**
   * Sets the accelerometer to active or standby mode.
   *
   * <p>It must be in standby mode to change any configuration.
   *
   * @see "HAL_SetAccelerometerActive"
   * @param active true to set to active, false for standby
   */
  public static native void setAccelerometerActive(boolean active);

  /**
   * Sets the range of values that can be measured (either 2, 4, or 8 g-forces).
   *
   * <p>The accelerometer should be in standby mode when this is called.
   *
   * @see "HAL_SetAccelerometerRange(int range)"
   * @param range the accelerometer range
   */
  public static native void setAccelerometerRange(int range);

  /**
   * Gets the x-axis acceleration.
   *
   * <p>This is a floating point value in units of 1 g-force.
   *
   * @see "HAL_GetAccelerometerX()"
   * @return the X acceleration
   */
  public static native double getAccelerometerX();

  /**
   * Gets the y-axis acceleration.
   *
   * <p>This is a floating point value in units of 1 g-force.
   *
   * @see "HAL_GetAccelerometerY()"
   * @return the Y acceleration
   */
  public static native double getAccelerometerY();

  /**
   * Gets the z-axis acceleration.
   *
   * <p>This is a floating point value in units of 1 g-force.
   *
   * @see "HAL_GetAccelerometerZ()"
   * @return the Z acceleration
   */
  public static native double getAccelerometerZ();

  /** Utility class. */
  private AccelerometerJNI() {}
}
