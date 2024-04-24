// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.cscore;

import edu.wpi.first.util.RuntimeLoader;
import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;
import org.opencv.core.Core;

/** OpenCV Native Loader. */
public final class OpenCvLoader {
  @SuppressWarnings("PMD.MutableStaticState")
  static boolean libraryLoaded;

  @SuppressWarnings("PMD.MutableStaticState")
  static RuntimeLoader<Core> loader;

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
   * Forces a static load.
   *
   * @return a garbage value
   */
  public static int forceStaticLoad() {
    return libraryLoaded ? 1 : 0;
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
    loader =
        new RuntimeLoader<>(
            Core.NATIVE_LIBRARY_NAME, RuntimeLoader.getDefaultExtractionRoot(), Core.class);
    loader.loadLibrary();
    libraryLoaded = true;
  }

  /** Utility class. */
  private OpenCvLoader() {}
}
