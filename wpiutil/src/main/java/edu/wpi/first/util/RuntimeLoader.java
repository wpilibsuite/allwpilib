// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

import java.io.IOException;

/** Loads a native library at runtime. */
public final class RuntimeLoader {
  /**
   * Returns a load error message given the information in the provided UnsatisfiedLinkError.
   *
   * @param libraryName the name of the library that failed to load.
   * @param ule UnsatisfiedLinkError object.
   * @return A load error message.
   */
  private static String getLoadErrorMessage(String libraryName, UnsatisfiedLinkError ule) {
    StringBuilder msg = new StringBuilder(512);
    msg.append(libraryName)
        .append(" could not be loaded from path.\n" + "\tattempted to load for platform ")
        .append(CombinedRuntimeLoader.getPlatformPath())
        .append("\nLast Load Error: \n")
        .append(ule.getMessage())
        .append('\n');
    if (System.getProperty("os.name").startsWith("Windows")) {
      msg.append(
          "A common cause of this error is missing the C++ runtime.\n"
              + "Download the latest at https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads\n");
    }
    return msg.toString();
  }

  /**
   * Loads a native library.
   *
   * @param libraryName the name of the library to load.
   * @throws IOException if the library fails to load
   */
  public static void loadLibrary(String libraryName) throws IOException {
    try {
      System.loadLibrary(libraryName);
    } catch (UnsatisfiedLinkError ule) {
      throw new IOException(getLoadErrorMessage(libraryName, ule));
    }
  }

  private RuntimeLoader() {
    throw new IllegalStateException("This class shouldn't be instantiated");
  }
}
