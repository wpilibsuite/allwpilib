// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.system;

import org.wpilib.hardware.hal.ThreadsJNI;

/** Thread utility functions. */
public final class Threads {
  /**
   * Gets the current thread's priority.
   *
   * <p>Priorities range from 0 to 99 where 0 is non-real-time, and 1-99 are real-time, and 99 is
   * highest priority. See "man 7 sched" for details.
   *
   * @return The current thread's priority.
   */
  public static int getCurrentThreadPriority() {
    return ThreadsJNI.getCurrentThreadPriority();
  }

  /**
   * Sets the current thread's priority.
   *
   * <p>Priorities range from 0 to 99 where 0 is non-real-time, 1-99 are real-time, and 99 is
   * highest priority. See "man 7 sched" for details.
   *
   * @param priority The priority.
   * @return True on success.
   * @deprecated Incorrect usage of real-time priority can lead to system lockups. Only use this
   *     function if you are trained in real-time software development.
   */
  @Deprecated
  public static boolean setCurrentThreadPriority(int priority) {
    return ThreadsJNI.setCurrentThreadPriority(priority);
  }

  private Threads() {}
}
