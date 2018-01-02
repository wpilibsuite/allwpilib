/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil;

import java.io.File;
import java.io.IOException;
import java.io.BufferedReader;
import java.io.InputStreamReader;

public class RuntimeDetector {
  private static String filePrefix;
  private static String fileExtension;
  private static String filePath;

  private static synchronized void computePlatform() {
    if (fileExtension != null && filePath != null && filePrefix != null) {
      return;
    }


    boolean intel32 = is32BitIntel();
    boolean intel64 = is64BitIntel();

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
      if (intel32) {
        filePath = "/osx/x86";
      } else {
        filePath = "/osx/x86-64/";
      }
    } else if (isLinux()) {
      filePrefix = "lib";
      fileExtension = ".so";
      if (intel32) {
        filePath = "/linux/x86/";
      } else if (intel64) {
        filePath = "/linux/x86-64/";
      } else if (isAthena()) {
        filePath = "/linux/athena/";
      } else {
        filePath = "/linux/nativearm/";
      }
    } else {
      throw new RuntimeException("Failed to determine OS");
    }
  }

  public static synchronized String getFilePrefix() {
    computePlatform();

    return filePrefix;
  }

  public static synchronized String getFileExtension() {
    computePlatform();

    return fileExtension;
  }

  public static synchronized String getPlatformPath() {
    computePlatform();

    return filePath;
  }

  public static synchronized String getLibraryResource(String libName) {
    computePlatform();

    return filePath + filePrefix + libName + fileExtension;
  }

  public static boolean isAthena() {
    File runRobotFile = new File("/usr/local/frc/bin/frcRunRobot.sh");
    return runRobotFile.exists();
  }

  public static boolean isLinux() {
    return System.getProperty("os.name").startsWith("Linux");
  }

  public static boolean isWindows() {
    return System.getProperty("os.name").startsWith("Windows");
  }

  public static boolean isMac() {
    return System.getProperty("os.name").startsWith("Mac");
  }

  public static boolean is32BitIntel() {
    String arch = System.getProperty("os.arch");
    return arch.equals("x86") || arch.equals("i386");
  }

  public static boolean is64BitIntel() {
    String arch = System.getProperty("os.arch");
    return arch.equals("amd64") || arch.equals("x86_64");
  }
}
