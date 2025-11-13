// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/**
 * Threads HAL JNI Functions.
 *
 * @see "Threads.h"
 */
public class ThreadsJNI extends JNIWrapper {
  /**
   * Gets the thread priority for the current thread.
   *
   * @return The current thread priority. For real-time, this is 1-99 with 99 being highest. For
   *     non-real-time, this is 0. See "man 7 sched" for details.
   * @see "HAL_GetCurrentThreadPriority"
   */
  public static native int getCurrentThreadPriority();

  /**
   * Gets the real-time status for the current thread.
   *
   * @return Set to true if thread is real-time, otherwise false.
   * @see "HAL_GetCurrentThreadPriority"
   */
  public static native boolean getCurrentThreadIsRealTime();

  /**
   * Sets the thread priority for the current thread.
   *
   * @param realTime Set to true to set a real-time priority, false for standard priority.
   * @param priority Priority to set the thread to. For real-time, this is 1-99 with 99 being
   *     highest. For non-real-time, this is forced to 0. See "man 7 sched" for more details.
   * @return True on success.
   * @see "HAL_SetCurrentThreadPriority"
   */
  public static native boolean setCurrentThreadPriority(boolean realTime, int priority);

  /** Utility class. */
  private ThreadsJNI() {}
}
