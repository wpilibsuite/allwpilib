package edu.wpi.first.wpilibj.hal;

import java.nio.ByteBuffer;
import java.nio.IntBuffer;
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
  public static native ByteBuffer initializeNotifier(Runnable func, IntBuffer status);

  /**
   * Initializes the JVM for use by the callback. Should be called before
   * anything else.
   */
  public static native void initializeNotifierJVM(IntBuffer status);

  /**
   * Deletes the notifier object when we are done with it.
   */
  public static native void cleanNotifier(ByteBuffer notifierPtr, IntBuffer status);

  /**
   * Sets the notifier to call the callback in another triggerTime microseconds.
   */
  public static native void updateNotifierAlarm(ByteBuffer notifierPtr, int triggerTime,
      IntBuffer status);
}
