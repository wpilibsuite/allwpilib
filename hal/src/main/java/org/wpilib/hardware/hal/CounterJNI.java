// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/**
 * Counter HAL JNI functions.
 *
 * @see "hal/Counter.h"
 */
public class CounterJNI extends JNIWrapper {
  /**
   * Initializes a counter.
   *
   * @param channel the DIO channel
   * @param risingEdge true to trigger on rising
   * @return the created handle
   * @see "HAL_InitializeCounter"
   */
  public static native int initializeCounter(int channel, boolean risingEdge);

  /**
   * Frees a counter.
   *
   * @param counterHandle the counter handle
   * @see "HAL_FreeCounter"
   */
  public static native void freeCounter(int counterHandle);

  /**
   * Sets the up source to either detect rising edges or falling edges.
   *
   * <p>Note that both are allowed to be set true at the same time without issues.
   *
   * @param counterHandle the counter handle
   * @param risingEdge true to trigger on rising
   * @see "HAL_SetCounterUpSourceEdge"
   */
  public static native void setCounterEdgeConfiguration(int counterHandle, boolean risingEdge);

  /**
   * Resets the Counter to zero.
   *
   * <p>Sets the counter value to zero. This does not effect the running state of the counter, just
   * sets the current value to zero.
   *
   * @param counterHandle the counter handle
   * @see "HAL_ResetCounter"
   */
  public static native void resetCounter(int counterHandle);

  /**
   * Reads the current counter value.
   *
   * <p>Reads the value at this instant. It may still be running, so it reflects the current value.
   * Next time it is read, it might have a different value.
   *
   * @param counterHandle the counter handle
   * @return the current counter value
   * @see "HAL_GetCounter"
   */
  public static native int getCounter(int counterHandle);

  /**
   * Gets the Period of the most recent count.
   *
   * <p>Returns the time interval of the most recent count. This can be used for velocity
   * calculations to determine shaft velocity.
   *
   * @param counterHandle the counter handle
   * @return the period of the last two pulses in units of seconds
   * @see "HAL_GetCounterPeriod"
   */
  public static native double getCounterPeriod(int counterHandle);

  /**
   * Sets the maximum period where the device is still considered "moving".
   *
   * <p>Sets the maximum period where the device is considered moving. This value is used to
   * determine the "stopped" state of the counter using the HAL_GetCounterStopped method.
   *
   * @param counterHandle the counter handle
   * @param maxPeriod the maximum period where the counted device is considered moving in seconds
   * @see "HAL_SetCounterMaxPeriod"
   */
  public static native void setCounterMaxPeriod(int counterHandle, double maxPeriod);

  /**
   * Determines if the clock is stopped.
   *
   * <p>Determine if the clocked input is stopped based on the MaxPeriod value set using the
   * SetMaxPeriod method. If the clock exceeds the MaxPeriod, then the device (and counter) are
   * assumed to be stopped and it returns true.
   *
   * @param counterHandle the counter handle
   * @return true if the most recent counter period exceeds the MaxPeriod value set by SetMaxPeriod
   * @see "HAL_GetCounterStopped"
   */
  public static native boolean getCounterStopped(int counterHandle);

  /** Utility class. */
  private CounterJNI() {}
}
