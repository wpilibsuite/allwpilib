// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

import edu.wpi.first.util.RuntimeLoader;
import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;
import org.opencv.core.Core;

/** CameraServer CV JNI. */
public class CameraServerCvJNI {
  static boolean libraryLoaded = false;

  static RuntimeLoader<Core> loader = null;

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
    String opencvName = Core.NATIVE_LIBRARY_NAME;
    if (Helper.getExtractOnStaticLoad()) {
      try {
        CameraServerJNI.forceLoad();
        loader =
            new RuntimeLoader<>(opencvName, RuntimeLoader.getDefaultExtractionRoot(), Core.class);
        loader.loadLibraryHashed();
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
   * @throws IOException if library load failed
   */
  public static synchronized void forceLoad() throws IOException {
    if (libraryLoaded) {
      return;
    }
    CameraServerJNI.forceLoad();
    loader =
        new RuntimeLoader<>(
            Core.NATIVE_LIBRARY_NAME, RuntimeLoader.getDefaultExtractionRoot(), Core.class);
    loader.loadLibrary();
    libraryLoaded = true;
  }

  public static native int createCvSource(
      String name, int pixelFormat, int width, int height, int fps);

  public static native void putSourceFrame(int source, long imageNativeObj);

  public static native int createCvSink(String name, int pixelFormat);

  // public static native int createCvSinkCallback(String name,
  //                            void (*processFrame)(long time));

  public static native long grabSinkFrame(int sink, long imageNativeObj);

  public static native long grabSinkFrameTimeout(int sink, long imageNativeObj, double timeout);

  /** Utility class. */
  private CameraServerCvJNI() {}
}
