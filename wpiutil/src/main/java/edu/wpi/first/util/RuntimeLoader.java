// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

import java.io.File;
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
    String jvmLocation = ProcessHandle.current().info().command().orElse("Unknown");
    StringBuilder msg = new StringBuilder(512);
    msg.append(libraryName)
        .append(" could not be loaded from path.\n" + "\tattempted to load for platform ")
        .append(getPlatform())
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

  /**
   * Returns platform name.
   *
   * @return The current platform name.
   * @throws IllegalStateException Thrown if the operating system is unknown.
   */
  public static String getPlatform() {
    String filePath;
    String arch = System.getProperty("os.arch");

    boolean intel32 = "x86".equals(arch) || "i386".equals(arch);
    boolean intel64 = "amd64".equals(arch) || "x86_64".equals(arch);

    if (System.getProperty("os.name").startsWith("Windows")) {
      if (intel32) {
        filePath = "windows-x86";
      } else {
        filePath = "windows-x64";
      }
    } else if (System.getProperty("os.name").startsWith("Mac")) {
      filePath = "osx-universal";
    } else if (System.getProperty("os.name").startsWith("Linux")) {
      if (intel32) {
        filePath = "linux-x86";
      } else if (intel64) {
        filePath = "linux-x64";
      } else if (new File("/usr/local/frc/bin/frcRunRobot.sh").exists()) {
        filePath = "linux-athena";
      } else if ("arm".equals(arch) || "arm32".equals(arch)) {
        filePath = "linux-arm32";
      } else if ("aarch64".equals(arch) || "arm64".equals(arch)) {
        filePath = "linux-arm64";
      } else {
        filePath = "linux-nativearm";
      }
    } else {
      throw new IllegalStateException();
    }

    return filePath;
  }

  private RuntimeLoader() {
    throw new IllegalStateException("This class shouldn't be instantiated");
  }
}
