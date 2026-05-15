// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.driverstation;

import org.wpilib.driverstation.internal.DriverStationBackend;

/** Provides access to error and warning reporting functionality to the Driver Station. */
public final class DriverStationErrors {
  private DriverStationErrors() {}

  /**
   * Report error to Driver Station. Optionally appends Stack trace to error message.
   *
   * @param error The error to report.
   * @param printTrace If true, append stack trace to error string
   */
  public static void reportError(String error, boolean printTrace) {
    DriverStationBackend.reportError(error, printTrace);
  }

  /**
   * Report error to Driver Station. Appends provided stack trace to error message.
   *
   * @param error The error to report.
   * @param stackTrace The stack trace to append
   */
  public static void reportError(String error, StackTraceElement[] stackTrace) {
    DriverStationBackend.reportError(error, stackTrace);
  }

  /**
   * Report warning to Driver Station. Optionally appends Stack trace to warning message.
   *
   * @param warning The warning to report.
   * @param printTrace If true, append stack trace to warning string
   */
  public static void reportWarning(String warning, boolean printTrace) {
    DriverStationBackend.reportWarning(warning, printTrace);
  }

  /**
   * Report warning to Driver Station. Appends provided stack trace to warning message.
   *
   * @param warning The warning to report.
   * @param stackTrace The stack trace to append
   */
  public static void reportWarning(String warning, StackTraceElement[] stackTrace) {
    DriverStationBackend.reportWarning(warning, stackTrace);
  }
}
