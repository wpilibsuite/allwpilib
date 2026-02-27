// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util.runtime;

import java.io.IOException;

/** Loads a native library at runtime. */
public final class RuntimeLoader {
  /**
   * Returns platform name.
   *
   * @return The current platform name.
   * @throws IllegalStateException Thrown if the operating system is unknown.
   */
  public static String getPlatformName() {
    String arch = System.getProperty("os.arch");

    boolean intel64 = "amd64".equals(arch) || "x86_64".equals(arch);
    boolean arm64 = "aarch64".equals(arch) || "arm64".equals(arch);

    if (intel64) {
      arch = "x86_64";
    } else if (arm64) {
      arch = "arm64";
    } else {
      throw new IllegalStateException("Unsupported architecture: " + arch);
    }

    String osName = System.getProperty("os.name");

    if (osName.startsWith("Windows")) {
      osName = "windows";
    } else if (osName.startsWith("Mac")) {
      osName = "osx";
      arch = "universal";
    } else if (osName.startsWith("Linux")) {
      osName = "linux";
    } else {
      throw new IllegalStateException("Unsupported operating system: " + osName);
    }

    return osName + "-" + arch;
  }

  /**
   * Returns a load error message given the information in the provided UnsatisfiedLinkError.
   *
   * @param libraryName the name of the library that failed to load.
   * @param ule UnsatisfiedLinkError object.
   * @return A load error message.
   */
  private static String getLoadErrorMessage(String libraryName, UnsatisfiedLinkError ule) {
    String jvmLocation = ProcessHandle.current().info().command().orElse("Unknown");
    StringBuilder msg = new StringBuilder(512);
    msg.append(libraryName)
        .append(" could not be loaded from path.\n" + "\tattempted to load for platform ")
        .append(getPlatformName())
        .append("\nLast Load Error: \n")
        .append(ule.getMessage())
        .append('\n')
        .append(String.format("JVM Location: %s\n", jvmLocation));
    if (System.getProperty("os.name").startsWith("Windows")) {
      msg.append(
          "A common cause of this error is using a JVM with an incorrect MSVC runtime.\n"
              + "Ensure you are using the WPILib JVM (The current running JVM is listed above)\n"
              + "See https://wpilib.org/jvmruntime for more information\n");
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
