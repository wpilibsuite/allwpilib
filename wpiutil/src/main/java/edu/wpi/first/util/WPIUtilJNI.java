// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;

public class WPIUtilJNI {
  static boolean libraryLoaded = false;
  static RuntimeLoader<WPIUtilJNI> loader = null;

  public static class Helper {
    private static AtomicBoolean extractOnStaticLoad = new AtomicBoolean(true);

    public static boolean getExtractOnStaticLoad() {
      return extractOnStaticLoad.get();
    }

    public static void setExtractOnStaticLoad(boolean load) {
      extractOnStaticLoad.set(load);
    }
  }

  static {
    if (Helper.getExtractOnStaticLoad()) {
      try {
        loader =
            new RuntimeLoader<>(
                "wpiutiljni", RuntimeLoader.getDefaultExtractionRoot(), WPIUtilJNI.class);
        loader.loadLibrary();
      } catch (IOException ex) {
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
    loader =
        new RuntimeLoader<>(
            "wpiutiljni", RuntimeLoader.getDefaultExtractionRoot(), WPIUtilJNI.class);
    loader.loadLibrary();
    libraryLoaded = true;
  }

  public static native void writeStderr(String str);

  public static native void enableMockTime();

  public static native void disableMockTime();

  public static native void setMockTime(long time);

  public static native long now();

  public static native long getSystemTime();

  public static native int createEvent(boolean manualReset, boolean initialState);

  public static native void destroyEvent(int eventHandle);

  public static native void setEvent(int eventHandle);

  public static native void resetEvent(int eventHandle);

  public static native int createSemaphore(int initialCount, int maximumCount);

  public static native void destroySemaphore(int semHandle);

  public static native boolean releaseSemaphore(int semHandle, int releaseCount);

  /**
   * Waits for an handle to be signaled.
   *
   * @param handle handle to wait on
   * @throws InterruptedException on failure (e.g. object was destroyed)
   */
  public static native void waitForObject(int handle) throws InterruptedException;

  /**
   * Waits for an handle to be signaled, with timeout.
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
}
