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
        filePath = "/mac/x86";
      } else {
        filePath = "/mac/x86-64/";
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
      } else if (isRaspbian()) {
        filePath = "/linux/raspbian/";
      } else if (isArmV7()) {
        filePath = "/linux/armhf/";
      } else if (isAarch64()) {
        filePath = "/linux/aarch/";
      } else {
        filePath = "/linux/arm/";
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

  public static boolean isRaspbian() {
    try {
      Process p = Runtime.getRuntime().exec("uname -a");
      BufferedReader in = new BufferedReader(
                          new InputStreamReader(p.getInputStream()));
      return in.readLine().contains("raspberrypi");
    } catch (IOException e) {
      return false;
    }
  }

  public static boolean isArmV7() {
    try {
      Process p = Runtime.getRuntime().exec("uname -a");
      BufferedReader in = new BufferedReader(
                          new InputStreamReader(p.getInputStream()));
      return in.readLine().contains("armv7");
    } catch (IOException e) {
      return false;
    }
  }

  public static boolean isAarch64() {
    return false;
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
