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
   * Creates a notifier.
   *
   * <p>A notifier is an timer that alarms at an initial and (optionally) repeated intervals. This
   * can be used to make precise control loops.
   *
   * @return the created notifier
   * @see "HAL_CreateNotifier"
   */
  public static native int createNotifier();

  /**
   * Sets the HAL notifier thread priority.
   *
   * <p>The HAL notifier thread is responsible for managing the system's notifier interrupt and
   * waking up user's Notifiers when it's their time to run. Giving the HAL notifier thread
   * real-time priority helps ensure the user's real-time Notifiers, if any, are notified to run in
   * a timely manner.
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
   * Destroys a notifier.
   *
   * <p>Destruction wakes up any waiters.
   *
   * @param notifierHandle the notifier handle
   * @see "HAL_DestroyNotifier"
   */
  public static native void destroyNotifier(int notifierHandle);

  /**
   * Updates the initial and interval alarm times for a notifier.
   *
   * <p>The alarmTime is an absolute time (using the WPI now() time base) if absolute is true, or
   * relative to the current time if absolute is false.
   *
   * <p>If intervalTime is non-zero, the notifier will alarm periodically following alarmTime at the
   * given interval.
   *
   * <p>If an absolute alarmTime is in the past, the notifier will alarm immediately.
   *
   * @param notifierHandle the notifier handle
   * @param alarmTime the first alarm time (in microseconds)
   * @param intervalTime the periodic interval time (in microseconds)
   * @param absolute true if the alarm time is absolute
   * @param ack true to acknowledge any prior alarm
   * @see "HAL_SetNotifierAlarm"
   */
  public static native void setNotifierAlarm(
      int notifierHandle, long alarmTime, long intervalTime, boolean absolute, boolean ack);

  /**
   * Cancels all future notifier alarms for a notifier.
   *
   * @param notifierHandle the notifier handle
   * @param ack true to acknowledge any prior alarm
   * @see "HAL_CancelNotifierAlarm"
   */
  public static native void cancelNotifierAlarm(int notifierHandle, boolean ack);

  /**
   * Indicates the notifier alarm has been serviced. Makes no change to future alarms.
   *
   * <p>One of setNotifierAlarm (with ack=true), cancelNotifierAlarm (with ack=true), or this
   * function must be called before waiting for the next alarm.
   *
   * @param notifierHandle the notifier handle
   * @see "HAL_AcknowledgeNotifierAlarm"
   */
  public static native void acknowledgeNotifierAlarm(int notifierHandle);

  /**
   * Gets the overrun count for a notifier.
   *
   * <p>An overrun occurs when a notifier's alarm is not serviced before the next scheduled alarm
   * time.
   *
   * @param notifierHandle the notifier handle
   * @return overrun count
   */
  public static native int getNotifierOverrun(int notifierHandle);

  /** Utility class. */
  private NotifierJNI() {}
}
