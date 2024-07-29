// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.jni;

import edu.wpi.first.util.RuntimeLoader;
import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;

/** Base class for all WPIMath JNI wrappers. */
public class WPIMathJNI {
  private static boolean libraryLoaded = false;

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
        RuntimeLoader.loadLibrary("wpimathjni");
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
   * @throws IOException if the library load failed
   */
  public static synchronized void forceLoad() throws IOException {
    if (libraryLoaded) {
      return;
    }
    RuntimeLoader.loadLibrary("wpimathjni");
    libraryLoaded = true;
  }

  /** Utility class. */
  protected WPIMathJNI() {}
}
