// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.ThreadsJNI;

public final class Threads {
  /**
   * Get the thread priority for the current thread.
   *
   * @return The current thread priority. For real-time, this is 1-99 with 99 being highest. For
   *     non-real-time, this is 0. See "man 7 sched" for details.
   */
  public static int getCurrentThreadPriority() {
    return ThreadsJNI.getCurrentThreadPriority();
  }

  /**
   * Get if the current thread is real-time.
   *
   * @return If the current thread is real-time.
   */
  public static boolean getCurrentThreadIsRealTime() {
    return ThreadsJNI.getCurrentThreadIsRealTime();
  }

  /**
   * Sets the thread priority for the current thread.
   *
   * @param realTime Set to true to set a real-time priority, false for standard priority.
   * @param priority Priority to set the thread to. For real-time, this is 1-99 with 99 being
   *     highest. For non-real-time, this is forced to 0. See "man 7 sched" for details.
   * @return True on success.
   */
  public static boolean setCurrentThreadPriority(boolean realTime, int priority) {
    return ThreadsJNI.setCurrentThreadPriority(realTime, priority);
  }

  private Threads() {}
}
