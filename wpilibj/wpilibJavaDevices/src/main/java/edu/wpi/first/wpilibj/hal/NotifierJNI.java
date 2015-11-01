package edu.wpi.first.wpilibj.hal;

import java.lang.Runtime;

/**
 * The NotifierJNI class directly wraps the C++ HAL Notifier.
 *
 * This class is not meant for direct use by teams. Instead, the
 * edu.wpi.first.wpilibj.Notifier class, which corresponds to the C++ Notifier
 * class, should be used.
 */
public class NotifierJNI extends JNIWrapper {
  /**
   * Initializes the notifier to call the run() function of a Runnable.
   *
   * Should be called after initializeNotifierJVM().
   */
  public static native long initializeNotifier(Runnable func);

  /**
   * Deletes the notifier object when we are done with it.
   */
  public static native void cleanNotifier(long notifierPtr);

  /**
   * Sets the notifier to call the callback in another triggerTime microseconds.
   */
  public static native void updateNotifierAlarm(long notifierPtr, int triggerTime);
}
