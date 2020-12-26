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
   */
  public static native int initializeNotifier();

  /**
   * Sets the name of the notifier.
   */
  public static native void setNotifierName(int notifierHandle, String name);

  /**
   * Wakes up the waiter with time=0.  Note: after this function is called, all
   * calls to waitForNotifierAlarm() will immediately start returning 0.
   */
  public static native void stopNotifier(int notifierHandle);

  /**
   * Deletes the notifier object when we are done with it.
   */
  public static native void cleanNotifier(int notifierHandle);

  /**
   * Sets the notifier to wakeup the waiter in another triggerTime microseconds.
   */
  public static native void updateNotifierAlarm(int notifierHandle, long triggerTime);

  /**
   * Cancels any pending wakeups set by updateNotifierAlarm().  Does NOT wake
   * up any waiters.
   */
  public static native void cancelNotifierAlarm(int notifierHandle);

  /**
   * Block until woken up by an alarm (or stop).
   * @return Time when woken up.
   */
  public static native long waitForNotifierAlarm(int notifierHandle);
}
