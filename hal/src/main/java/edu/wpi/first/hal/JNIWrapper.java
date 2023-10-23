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
  static RuntimeLoader<JNIWrapper> loader = null;

  /**
   * Load the library.
   *
   * @throws IOException if the library load failed
   */
  public static synchronized void load() throws IOException {
    if (libraryLoaded) {
      return;
    }
    loader =
        new RuntimeLoader<>(
            "wpiHaljni", RuntimeLoader.getDefaultExtractionRoot(), JNIWrapper.class);
    loader.loadLibrary();
    libraryLoaded = true;
  }

  public static void suppressUnused(Object object) {}
}
