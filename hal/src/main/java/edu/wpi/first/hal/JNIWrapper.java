// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

import edu.wpi.first.util.RuntimeLoader;
import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;

/** Base class for all JNI wrappers. */
public class JNIWrapper {
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
        RuntimeLoader.loadLibrary("wpiHaljni");
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
    RuntimeLoader.loadLibrary("wpiHaljni");
    libraryLoaded = true;
  }

  /**
   * Dummy function to suppress unused variable warnings.
   *
   * @param object variable to suppress
   */
  public static void suppressUnused(Object object) {}

  /** Utility class. */
  public JNIWrapper() {}
}
