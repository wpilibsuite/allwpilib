/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil;

import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;

public final class WPIUtilJNI {
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
        loader = new RuntimeLoader<>("wpiutiljni", RuntimeLoader.getDefaultExtractionRoot(), WPIUtilJNI.class);
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
   */
  public static synchronized void forceLoad() throws IOException {
    if (libraryLoaded) {
      return;
    }
    loader = new RuntimeLoader<>("wpiutiljni", RuntimeLoader.getDefaultExtractionRoot(), WPIUtilJNI.class);
    loader.loadLibrary();
    libraryLoaded = true;
  }

  public static native long now();

  public static native void addPortForwarder(int port, String remoteHost, int remotePort);
  public static native void removePortForwarder(int port);
}
