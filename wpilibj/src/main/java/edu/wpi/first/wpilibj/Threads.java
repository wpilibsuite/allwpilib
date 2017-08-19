/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.ThreadsJNI;

public class Threads {

  /**
  * Get the thread priority for the current thread.
  * @return The current thread priority. Scaled 1-99.
  */
  public static int getCurrentThreadPriority() {
    return ThreadsJNI.getCurrentThreadPriority();
  }

  /**
  * Get if the current thread is realtime.
  * @return If the current thread is realtime
  */
  public static boolean getCurrentThreadIsRealTime() {
    return ThreadsJNI.getCurrentThreadIsRealTime();
  }

  /**
  * Sets the thread priority for the current thread.
  *
  * @param realTime Set to true to set a realtime priority, false for standard
  *     priority
  * @param priority Priority to set the thread to. Scaled 1-99, with 1 being
  *     highest. On RoboRIO, priority is ignored for non realtime setting
  *
  * @return The success state of setting the priority
  */
  public static boolean setCurrentThreadPriority(boolean realTime, int priority) {
    return ThreadsJNI.setCurrentThreadPriority(realTime, priority);
  }
}
