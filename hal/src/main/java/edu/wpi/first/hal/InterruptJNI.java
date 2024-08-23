// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * Interrupt HAL JNI functions.
 *
 * @see "hal/Interrupts.h"
 */
public class InterruptJNI extends JNIWrapper {
  /** Invalid handle value. */
  public static final int HalInvalidHandle = 0;

  /**
   * Initializes an interrupt.
   *
   * @return the created interrupt handle
   * @see "HAL_InitializeInterrupts"
   */
  public static native int initializeInterrupts();

  /**
   * Frees an interrupt.
   *
   * @param interruptHandle the interrupt handle
   * @see "HAL_CleanInterrupts"
   */
  public static native void cleanInterrupts(int interruptHandle);

  /**
   * Waits for the defined interrupt to occur.
   *
   * @param interruptHandle the interrupt handle
   * @param timeout timeout in seconds
   * @param ignorePrevious if true, ignore interrupts that happened before waitForInterrupt was
   *     called
   * @return the mask of interrupts that fired
   * @see "HAL_WaitForInterrupt"
   */
  public static native long waitForInterrupt(
      int interruptHandle, double timeout, boolean ignorePrevious);

  /**
   * Waits for any interrupt covered by the mask to occur.
   *
   * @param interruptHandle the interrupt handle to use for the context
   * @param mask the mask of interrupts to wait for
   * @param timeout timeout in seconds
   * @param ignorePrevious if true, ignore interrupts that happened before waitForInterrupt was
   *     called
   * @return the mask of interrupts that fired
   * @see "HAL_WaitForMultipleInterrupts"
   */
  public static native long waitForMultipleInterrupts(
      int interruptHandle, long mask, double timeout, boolean ignorePrevious);

  /**
   * Returns the timestamp for the rising interrupt that occurred most recently.
   *
   * <p>This is in the same time domain as getFPGATime(). It only contains the bottom 32 bits of the
   * timestamp. If your robot has been running for over 1 hour, you will need to fill in the upper
   * 32 bits yourself.
   *
   * @param interruptHandle the interrupt handle
   * @return timestamp in microseconds since FPGA Initialization
   */
  public static native long readInterruptRisingTimestamp(int interruptHandle);

  /**
   * Returns the timestamp for the falling interrupt that occurred most recently.
   *
   * <p>This is in the same time domain as getFPGATime(). It only contains the bottom 32 bits of the
   * timestamp. If your robot has been running for over 1 hour, you will need to fill in the upper
   * 32 bits yourself.
   *
   * @param interruptHandle the interrupt handle
   * @return timestamp in microseconds since FPGA Initialization
   */
  public static native long readInterruptFallingTimestamp(int interruptHandle);

  /**
   * Requests interrupts on a specific digital source.
   *
   * @param interruptHandle the interrupt handle
   * @param digitalSourceHandle the digital source handle (either a HAL_AnalogTriggerHandle or a
   *     HAL_DigitalHandle)
   * @param analogTriggerType the trigger type if the source is an AnalogTrigger
   * @see "HAL_RequestInterrupts"
   */
  public static native void requestInterrupts(
      int interruptHandle, int digitalSourceHandle, int analogTriggerType);

  /**
   * Sets the edges to trigger the interrupt on.
   *
   * <p>Note that both edges triggered is a valid configuration.
   *
   * @param interruptHandle the interrupt handle
   * @param risingEdge true for triggering on rising edge
   * @param fallingEdge true for triggering on falling edge
   * @see "HAL_SetInterruptUpSourceEdge"
   */
  public static native void setInterruptUpSourceEdge(
      int interruptHandle, boolean risingEdge, boolean fallingEdge);

  /**
   * Releases a waiting interrupt.
   *
   * <p>This will release both rising and falling waiters.
   *
   * @param interruptHandle the interrupt handle to release
   * @see "HAL_ReleaseWaitingInterrupt"
   */
  public static native void releaseWaitingInterrupt(int interruptHandle);

  /** Utility class. */
  private InterruptJNI() {}
}
