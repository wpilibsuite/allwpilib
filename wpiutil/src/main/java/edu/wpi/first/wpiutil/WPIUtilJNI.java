// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpiutil;

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

  /** Force load the library. */
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

  public static native long now();

  public static native void addPortForwarder(int port, String remoteHost, int remotePort);

  public static native void removePortForwarder(int port);
}
