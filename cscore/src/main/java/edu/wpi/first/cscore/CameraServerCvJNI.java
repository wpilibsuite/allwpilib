// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

import edu.wpi.first.util.RuntimeLoader;
import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;
import org.opencv.core.Core;

public class CameraServerCvJNI {
  static boolean libraryLoaded = false;

  static RuntimeLoader<Core> loader = null;

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

  public static native int createCvSink(String name);
  // public static native int createCvSinkCallback(String name,
  //                            void (*processFrame)(long time));

  public static native long grabSinkFrame(int sink, long imageNativeObj);

  public static native long grabSinkFrameTimeout(int sink, long imageNativeObj, double timeout);
}
