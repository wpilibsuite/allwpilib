// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.util;

/** WPIMath utility functions. */
public interface MathShared {
  /**
   * Report an error.
   *
   * @param error the error to set
   * @param stackTrace array of stacktrace elements
   */
  void reportError(String error, StackTraceElement[] stackTrace);

  /**
   * Report usage.
   *
   * @param resource the resource name
   * @param data arbitrary string data
   */
  void reportUsage(String resource, String data);

  /**
   * Get the current time.
   *
   * @return Time in seconds
   */
  double getTimestamp();
}
