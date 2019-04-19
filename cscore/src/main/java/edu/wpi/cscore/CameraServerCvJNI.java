/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.cscore;

import java.io.IOException;

import org.opencv.core.Core;

import edu.wpi.first.wpiutil.RuntimeLoader;

public class CameraServerCvJNI {
  static boolean cvLibraryLoaded = false;

  static RuntimeLoader<Core> cvLoader = null;

  static {
    String opencvName = Core.NATIVE_LIBRARY_NAME;
    if (!cvLibraryLoaded) {
      try {
        cvLoader = new RuntimeLoader<>(opencvName, RuntimeLoader.getDefaultExtractionRoot(), Core.class);
        cvLoader.loadLibraryHashed();
      } catch (IOException ex) {
        ex.printStackTrace();
        System.exit(1);
      }
      cvLibraryLoaded = true;
    }
  }

  public static void forceLoad() {}

  public static native int createCvSource(String name, int pixelFormat, int width, int height, int fps);

  public static native void putSourceFrame(int source, long imageNativeObj);

  public static native int createCvSink(String name);
  //public static native int createCvSinkCallback(String name,
  //                            void (*processFrame)(long time));

  public static native long grabSinkFrame(int sink, long imageNativeObj);
  public static native long grabSinkFrameTimeout(int sink, long imageNativeObj, double timeout);
}
