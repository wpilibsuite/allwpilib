// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * The NotifierJNI class directly wraps the C++ HAL Notifier.
 *
 * <p>This class is not meant for direct use by teams. Instead, the edu.wpi.first.wpilibj.Notifier
 * class, which corresponds to the C++ Notifier class, should be used.
 */
public class NotifierJNI extends JNIWrapper {
  /**
   * Initializes the notifier.
   *
   * @return True on success.
   */
  public static native int initializeNotifier();

  /**
   * Sets the HAL notifier thread priority.
   *
   * @param realTime Set to true to set a real-time priority, false for standard priority.
   * @param priority Priority to set the thread to. For real-time, this is 1-99 with 99 being
   *     highest. For non-real-time, this is forced to 0. See "man 7 sched" for more details.
   * @return True on success.
   */
  public static native boolean setHALThreadPriority(boolean realTime, int priority);

  /**
   * Sets the name of the notifier.
   *
   * @param notifierHandle Notifier handle.
   * @param name Notifier name.
   */
  public static native void setNotifierName(int notifierHandle, String name);

  /**
   * Wakes up the waiter with time=0. Note: after this function is called, all calls to
   * waitForNotifierAlarm() will immediately start returning 0.
   *
   * @param notifierHandle Notifier handle.
   */
  public static native void stopNotifier(int notifierHandle);

  /**
   * Deletes the notifier object when we are done with it.
   *
   * @param notifierHandle Notifier handle.
   */
  public static native void cleanNotifier(int notifierHandle);

  /**
   * Sets the notifier to wake up the waiter at triggerTime microseconds.
   *
   * @param notifierHandle Notifier handle.
   * @param triggerTime Trigger time in microseconds.
   */
  public static native void updateNotifierAlarm(int notifierHandle, long triggerTime);

  /**
   * Cancels any pending wakeups set by updateNotifierAlarm(). Does NOT wake up any waiters.
   *
   * @param notifierHandle Notifier handle.
   */
  public static native void cancelNotifierAlarm(int notifierHandle);

  /**
   * Block until woken up by an alarm (or stop).
   *
   * @param notifierHandle Notifier handle.
   * @return Time when woken up.
   */
  public static native long waitForNotifierAlarm(int notifierHandle);
}
