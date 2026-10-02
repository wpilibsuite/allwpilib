// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/**
 * Counter HAL JNI functions.
 *
 * @see "wpi/hal/Counter.h"
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
   * Sets the counter to detect rising or falling edges.
   *
   * @param counterHandle the counter handle
   * @param risingEdge true to count rising edges, false to count falling
   * @see "HAL_SetCounterEdgeConfiguration"
   */
  public static native void setCounterEdgeConfiguration(int counterHandle, boolean risingEdge);

  /**
   * Sets the time window used to calculate the counter rate.
   *
   * @param counterHandle the counter handle
   * @param windowMilliseconds the rate calculation window in milliseconds; valid values are 5
   *     through 255
   * @see "HAL_SetCounterRateWindow"
   */
  public static native void setCounterRateWindow(int counterHandle, int windowMilliseconds);

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
   * Gets the rate of the counter.
   *
   * @param counterHandle the counter handle
   * @return the counter rate in counts per second
   * @see "HAL_GetCounterRate"
   */
  public static native double getCounterRate(int counterHandle);

  /**
   * Determines if the clock is stopped.
   *
   * <p>Determines if the counter's current rate is zero.
   *
   * @param counterHandle the counter handle
   * @return true if the counter's current rate is zero
   * @see "HAL_GetCounterStopped"
   */
  public static native boolean getCounterStopped(int counterHandle);

  /** Utility class. */
  private CounterJNI() {}
}
