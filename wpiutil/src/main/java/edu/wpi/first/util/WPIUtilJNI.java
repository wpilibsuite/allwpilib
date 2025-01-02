// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.concurrent.atomic.AtomicBoolean;

/** WPIUtil JNI. */
public class WPIUtilJNI {
  static boolean libraryLoaded = false;

  /** Sets whether JNI should be loaded in the static block. */
  public static class Helper {
    private static AtomicBoolean extractOnStaticLoad = new AtomicBoolean(true);

    /**
     * Returns true if the JNI should be loaded in the static block.
     *
     * @return True if the JNI should be loaded in the static block.
     */
    public static boolean getExtractOnStaticLoad() {
      return extractOnStaticLoad.get();
    }

    /**
     * Sets whether the JNI should be loaded in the static block.
     *
     * @param load Whether the JNI should be loaded in the static block.
     */
    public static void setExtractOnStaticLoad(boolean load) {
      extractOnStaticLoad.set(load);
    }

    /** Utility class. */
    private Helper() {}
  }

  static {
    if (Helper.getExtractOnStaticLoad()) {
      try {
        RuntimeLoader.loadLibrary("wpiutiljni");
      } catch (Exception ex) {
        ex.printStackTrace();
        System.exit(1);
      }
      libraryLoaded = true;
    }
  }

  /**
   * Force load the library.
   *
   * @throws IOException if the library failed to load
   */
  public static synchronized void forceLoad() throws IOException {
    if (libraryLoaded) {
      return;
    }
    RuntimeLoader.loadLibrary("wpiutiljni");
    libraryLoaded = true;
  }

  /** Checks if the MSVC runtime is valid. Throws a runtime exception if not. */
  public static native void checkMsvcRuntime();

  /**
   * Write the given string to stderr.
   *
   * @param str String to write.
   */
  public static native void writeStderr(String str);

  /** Enable mock time. */
  public static native void enableMockTime();

  /** Disable mock time. */
  public static native void disableMockTime();

  /**
   * Set mock time.
   *
   * @param time The desired time in microseconds.
   */
  public static native void setMockTime(long time);

  /**
   * Returns the time.
   *
   * @return The time.
   */
  public static native long now();

  /**
   * Returns the system time.
   *
   * @return The system time.
   */
  public static native long getSystemTime();

  /**
   * Creates an event. Events have binary state (signaled or not signaled) and may be either
   * automatically reset or manually reset. Automatic-reset events go to non-signaled state when a
   * WaitForObject is woken up by the event; manual-reset events require ResetEvent() to be called
   * to set the event to non-signaled state; if ResetEvent() is not called, any waiter on that event
   * will immediately wake when called.
   *
   * @param manualReset true for manual reset, false for automatic reset
   * @param initialState true to make the event initially in signaled state
   * @return Event handle
   */
  public static native int createEvent(boolean manualReset, boolean initialState);

  /**
   * Destroys an event. Destruction wakes up any waiters.
   *
   * @param eventHandle event handle
   */
  public static native void destroyEvent(int eventHandle);

  /**
   * Sets an event to signaled state.
   *
   * @param eventHandle event handle
   */
  public static native void setEvent(int eventHandle);

  /**
   * Sets an event to non-signaled state.
   *
   * @param eventHandle event handle
   */
  public static native void resetEvent(int eventHandle);

  /**
   * Creates a semaphore. Semaphores keep an internal counter. Releasing the semaphore increases the
   * count. A semaphore with a non-zero count is considered signaled. When a waiter wakes up it
   * atomically decrements the count by 1. This is generally useful in a single-supplier,
   * multiple-consumer scenario.
   *
   * @param initialCount initial value for the semaphore's internal counter
   * @param maximumCount maximum value for the semaphore's internal counter
   * @return Semaphore handle
   */
  public static native int createSemaphore(int initialCount, int maximumCount);

  /**
   * Destroys a semaphore. Destruction wakes up any waiters.
   *
   * @param semHandle semaphore handle
   */
  public static native void destroySemaphore(int semHandle);

  /**
   * Releases N counts of a semaphore.
   *
   * @param semHandle semaphore handle
   * @param releaseCount amount to add to semaphore's internal counter; must be positive
   * @return True on successful release, false on failure (e.g. release count would exceed maximum
   *     value, or handle invalid)
   */
  public static native boolean releaseSemaphore(int semHandle, int releaseCount);

  static native long allocateRawFrame();

  static native void freeRawFrame(long frame);

  static native long getRawFrameDataPtr(long frame);

  static native void setRawFrameData(
      long frame, ByteBuffer data, int size, int width, int height, int stride, int pixelFormat);

  static native void setRawFrameInfo(
      long frame, int size, int width, int height, int stride, int pixelFormat);

  static native void setRawFrameTime(long frame, long timestamp, int timeSource);

  /**
   * Waits for a handle to be signaled.
   *
   * @param handle handle to wait on
   * @throws InterruptedException on failure (e.g. object was destroyed)
   */
  public static native void waitForObject(int handle) throws InterruptedException;

  /**
   * Waits for a handle to be signaled, with timeout.
   *
   * @param handle handle to wait on
   * @param timeout timeout in seconds
   * @return True if timeout reached without handle being signaled
   * @throws InterruptedException on failure (e.g. object was destroyed)
   */
  public static native boolean waitForObjectTimeout(int handle, double timeout)
      throws InterruptedException;

  /**
   * Waits for one or more handles to be signaled.
   *
   * <p>Invalid handles are treated as signaled; the returned array will have the handle error bit
   * set for any invalid handles.
   *
   * @param handles array of handles to wait on
   * @return array of signaled handles
   * @throws InterruptedException on failure (e.g. no objects were signaled)
   */
  public static native int[] waitForObjects(int[] handles) throws InterruptedException;

  /**
   * Waits for one or more handles to be signaled, with timeout.
   *
   * <p>Invalid handles are treated as signaled; the returned array will have the handle error bit
   * set for any invalid handles.
   *
   * @param handles array of handles to wait on
   * @param timeout timeout in seconds
   * @return array of signaled handles; empty if timeout reached without any handle being signaled
   * @throws InterruptedException on failure (e.g. no objects were signaled and no timeout)
   */
  public static native int[] waitForObjectsTimeout(int[] handles, double timeout)
      throws InterruptedException;

  /**
   * Create a native FileLogger. When the specified file is modified, appended data will be appended
   * to the specified data log.
   *
   * @param file path to the file
   * @param log data log implementation handle
   * @param key log key to append data to
   * @return The FileLogger handle.
   */
  public static native long createFileLogger(String file, long log, String key);

  /**
   * Free a native FileLogger. This causes the FileLogger to stop appending data to the log.
   *
   * @param fileTail The FileLogger handle.
   */
  public static native void freeFileLogger(long fileTail);

  /** Utility class. */
  protected WPIUtilJNI() {}
}
