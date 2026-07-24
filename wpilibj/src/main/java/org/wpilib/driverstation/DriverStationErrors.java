// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import org.wpilib.hardware.hal.DriverStationJNI;
import org.wpilib.hardware.hal.HAL;

/** Provides access to error and warning reporting functionality to the Driver Station. */
public final class DriverStationErrors {
  static {
    HAL.initialize(500, 0);
  }

  private DriverStationErrors() {}

  /**
   * Report error to Driver Station. Optionally appends Stack trace to error message.
   *
   * @param error The error to report.
   * @param printTrace If true, append stack trace to error string
   */
  public static void reportError(String error, boolean printTrace) {
    reportErrorImpl(true, 1, error, printTrace);
  }

  /**
   * Report error to Driver Station. Appends provided stack trace to error message.
   *
   * @param error The error to report.
   * @param stackTrace The stack trace to append
   */
  public static void reportError(String error, StackTraceElement[] stackTrace) {
    reportErrorImpl(true, 1, error, stackTrace);
  }

  /**
   * Report warning to Driver Station. Optionally appends Stack trace to warning message.
   *
   * @param warning The warning to report.
   * @param printTrace If true, append stack trace to warning string
   */
  public static void reportWarning(String warning, boolean printTrace) {
    reportErrorImpl(false, 1, warning, printTrace);
  }

  /**
   * Report warning to Driver Station. Appends provided stack trace to warning message.
   *
   * @param warning The warning to report.
   * @param stackTrace The stack trace to append
   */
  public static void reportWarning(String warning, StackTraceElement[] stackTrace) {
    reportErrorImpl(false, 1, warning, stackTrace);
  }

  private static void reportErrorImpl(boolean isError, int code, String error, boolean printTrace) {
    reportErrorImpl(isError, code, error, printTrace, Thread.currentThread().getStackTrace(), 3);
  }

  private static void reportErrorImpl(
      boolean isError, int code, String error, StackTraceElement[] stackTrace) {
    reportErrorImpl(isError, code, error, true, stackTrace, 0);
  }

  private static void reportErrorImpl(
      boolean isError,
      int code,
      String error,
      boolean printTrace,
      StackTraceElement[] stackTrace,
      int stackTraceFirst) {
    String locString;
    if (stackTrace.length >= stackTraceFirst + 1) {
      locString = stackTrace[stackTraceFirst].toString();
    } else {
      locString = "";
    }
    StringBuilder traceString = new StringBuilder();
    if (printTrace) {
      boolean haveLoc = false;
      for (int i = stackTraceFirst; i < stackTrace.length; i++) {
        String loc = stackTrace[i].toString();
        traceString.append("\tat ").append(loc).append('\n');
        // get first user function
        if (!haveLoc && !loc.startsWith("org.wpilib")) {
          locString = loc;
          haveLoc = true;
        }
      }
    }
    DriverStationJNI.sendError(isError, code, error, locString, traceString.toString(), true);
  }

  /**
   * Report crash to Driver Station. Appends provided stack trace to crash message.
   *
   * @param error The error message
   * @param stackTrace The stack trace to append
   */
  public static void reportCrash(String error, StackTraceElement[] stackTrace) {
    reportCrashImpl(error, stackTrace, 0);
  }

  private static void reportCrashImpl(
      String error, StackTraceElement[] stackTrace, int stackTraceFirst) {
    String locString;
    if (stackTrace.length >= stackTraceFirst + 1) {
      locString = stackTrace[stackTraceFirst].toString();
    } else {
      locString = "";
    }
    StringBuilder traceString = new StringBuilder();
    boolean haveLoc = false;
    for (int i = stackTraceFirst; i < stackTrace.length; i++) {
      String loc = stackTrace[i].toString();
      traceString.append("\tat ").append(loc).append('\n');
      // get first user function
      if (!haveLoc && !loc.startsWith("org.wpilib")) {
        locString = loc;
        haveLoc = true;
      }
    }
    DriverStationJNI.sendProgramCrash(error, locString, traceString.toString());
    // Sleep to ensure message is sent before crash
    try {
      Thread.sleep(100);
    } catch (InterruptedException e) {
      Thread.currentThread().interrupt();
    }
  }
}
