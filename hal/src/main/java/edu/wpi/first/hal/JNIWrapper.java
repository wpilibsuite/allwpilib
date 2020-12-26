// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;

import edu.wpi.first.wpiutil.RuntimeLoader;

/**
 * Base class for all JNI wrappers.
 */
public class JNIWrapper {
  static boolean libraryLoaded = false;
  static RuntimeLoader<JNIWrapper> loader = null;

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
        loader = new RuntimeLoader<>("wpiHaljni", RuntimeLoader.getDefaultExtractionRoot(), JNIWrapper.class);
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
    loader = new RuntimeLoader<>("wpiHaljni", RuntimeLoader.getDefaultExtractionRoot(), JNIWrapper.class);
    loader.loadLibrary();
    libraryLoaded = true;
  }
}
