// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

import java.io.File;

/**
 * A utility class for detecting and providing platform-specific such as OS and CPU architecture.
 *
 * @deprecated platform detection is brittle and may be removed in the future.
 */
@Deprecated(since = "2025", forRemoval = true)
public final class RuntimeDetector {
  private static String filePrefix;
  private static String fileExtension;
  private static String filePath;

  private static synchronized void computePlatform() {
    if (fileExtension != null && filePath != null && filePrefix != null) {
      return;
    }

    boolean intel32 = is32BitIntel();
    boolean intel64 = is64BitIntel();
    boolean arm64 = isArm64();

    if (isWindows()) {
      filePrefix = "";
      fileExtension = ".dll";
      if (intel32) {
        filePath = "/windows/x86/";
      } else {
        filePath = "/windows/x86-64/";
      }
    } else if (isMac()) {
      filePrefix = "lib";
      fileExtension = ".dylib";
      filePath = "/osx/universal/";
    } else if (isLinux()) {
      filePrefix = "lib";
      fileExtension = ".so";
      if (intel32) {
        filePath = "/linux/x86/";
      } else if (intel64) {
        filePath = "/linux/x86-64/";
      } else if (isAthena()) {
        filePath = "/linux/athena/";
      } else if (isArm32()) {
        filePath = "/linux/arm32/";
      } else if (arm64) {
        filePath = "/linux/arm64/";
      } else {
        filePath = "/linux/nativearm/";
      }
    } else {
      throw new IllegalStateException("Failed to determine OS");
    }
  }

  /**
   * Get the file prefix for the current system.
   *
   * @return The file prefix.
   */
  public static synchronized String getFilePrefix() {
    computePlatform();

    return filePrefix;
  }

  /**
   * Get the file extension for the current system.
   *
   * @return The file extension.
   */
  public static synchronized String getFileExtension() {
    computePlatform();

    return fileExtension;
  }

  /**
   * Get the platform path for the current system.
   *
   * @return The platform path.
   */
  public static synchronized String getPlatformPath() {
    computePlatform();

    return filePath;
  }

  /**
   * Get the path to the requested resource.
   *
   * @param libName Library name.
   * @return The path to the requested resource.
   */
  public static synchronized String getLibraryResource(String libName) {
    computePlatform();

    return filePath + filePrefix + libName + fileExtension;
  }

  /**
   * Get the path to the hash to the requested resource.
   *
   * @param libName Library name.
   * @return The path to the hash to the requested resource.
   */
  public static synchronized String getHashLibraryResource(String libName) {
    computePlatform();

    return filePath + libName + ".hash";
  }

  /**
   * Check if hardware platform is Athena.
   *
   * @return True if hardware platform is Athena.
   */
  public static boolean isAthena() {
    File runRobotFile = new File("/usr/local/frc/bin/frcRunRobot.sh");
    return runRobotFile.exists();
  }

  /**
   * Check if OS is Arm32.
   *
   * @return True if OS is Arm32.
   */
  public static boolean isArm32() {
    String arch = System.getProperty("os.arch");
    return "arm".equals(arch) || "arm32".equals(arch);
  }

  /**
   * Check if architecture is Arm64.
   *
   * @return if architecture is Arm64.
   */
  public static boolean isArm64() {
    String arch = System.getProperty("os.arch");
    return "aarch64".equals(arch) || "arm64".equals(arch);
  }

  /**
   * Check if OS is Linux.
   *
   * @return if OS is Linux.
   */
  public static boolean isLinux() {
    return System.getProperty("os.name").startsWith("Linux");
  }

  /**
   * Check if OS is Windows.
   *
   * @return if OS is Windows.
   */
  public static boolean isWindows() {
    return System.getProperty("os.name").startsWith("Windows");
  }

  /**
   * Check if OS is Mac.
   *
   * @return if OS is Mac.
   */
  public static boolean isMac() {
    return System.getProperty("os.name").startsWith("Mac");
  }

  /**
   * Check if OS is 32bit Intel.
   *
   * @return if OS is 32bit Intel.
   */
  public static boolean is32BitIntel() {
    String arch = System.getProperty("os.arch");
    return "x86".equals(arch) || "i386".equals(arch);
  }

  /**
   * Check if OS is 64bit Intel.
   *
   * @return if OS is 64bit Intel.
   */
  public static boolean is64BitIntel() {
    String arch = System.getProperty("os.arch");
    return "amd64".equals(arch) || "x86_64".equals(arch);
  }

  private RuntimeDetector() {}
}
