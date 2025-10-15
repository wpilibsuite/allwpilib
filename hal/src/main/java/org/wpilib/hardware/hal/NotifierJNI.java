// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/**
 * The NotifierJNI class directly wraps the C++ HAL Notifier.
 *
 * <p>This class is not meant for direct use by teams. Instead, the org.wpilib.system.Notifier
 * class, which corresponds to the C++ Notifier class, should be used.
 *
 * @see "hal/Notifier.h"
 */
public class NotifierJNI extends JNIWrapper {
  /**
   * Initializes a notifier.
   *
   * <p>A notifier is an FPGA controller timer that triggers at requested intervals based on the
   * FPGA time. This can be used to make precise control loops.
   *
   * @return the created notifier
   * @see "HAL_InitializeNotifier"
   */
  public static native int initializeNotifier();

  /**
   * Sets the HAL notifier thread priority.
   *
   * <p>The HAL notifier thread is responsible for managing the FPGA's notifier interrupt and waking
   * up user's Notifiers when it's their time to run. Giving the HAL notifier thread real-time
   * priority helps ensure the user's real-time Notifiers, if any, are notified to run in a timely
   * manner.
   *
   * @param realTime Set to true to set a real-time priority, false for standard priority.
   * @param priority Priority to set the thread to. For real-time, this is 1-99 with 99 being
   *     highest. For non-real-time, this is forced to 0. See "man 7 sched" for more details.
   * @return True on success.
   * @see "HAL_SetNotifierThreadPriority"
   */
  public static native boolean setHALThreadPriority(boolean realTime, int priority);

  /**
   * Sets the name of the notifier.
   *
   * @param notifierHandle Notifier handle.
   * @param name Notifier name.
   * @see "HAL_SetNotifierName"
   */
  public static native void setNotifierName(int notifierHandle, String name);

  /**
   * Stops a notifier from running.
   *
   * <p>This will cause any call into waitForNotifierAlarm to return with time = 0.
   *
   * @param notifierHandle the notifier handle
   * @see "HAL_StopNotifier"
   */
  public static native void stopNotifier(int notifierHandle);

  /**
   * Cleans a notifier.
   *
   * <p>Note this also stops a notifier if it is already running.
   *
   * @param notifierHandle the notifier handle
   * @see "HAL_CleanNotifier"
   */
  public static native void cleanNotifier(int notifierHandle);

  /**
   * Updates the trigger time for a notifier.
   *
   * <p>Note that this time is an absolute time relative to getFPGATime()
   *
   * @param notifierHandle the notifier handle
   * @param triggerTime the updated trigger time
   * @see "HAL_UpdateNotifierAlarm"
   */
  public static native void updateNotifierAlarm(int notifierHandle, long triggerTime);

  /**
   * Cancels the next notifier alarm.
   *
   * <p>This does not cause waitForNotifierAlarm to return.
   *
   * @param notifierHandle the notifier handle
   * @see "HAL_CancelNotifierAlarm"
   */
  public static native void cancelNotifierAlarm(int notifierHandle);

  /**
   * Waits for the next alarm for the specific notifier.
   *
   * <p>This is a blocking call until either the time elapses or stopNotifier gets called. If the
   * latter occurs, this function will return zero and any loops using this function should exit.
   * Failing to do so can lead to use-after-frees.
   *
   * @param notifierHandle the notifier handle
   * @return the FPGA time the notifier returned
   */
  public static native long waitForNotifierAlarm(int notifierHandle);

  /** Utility class. */
  private NotifierJNI() {}
}
