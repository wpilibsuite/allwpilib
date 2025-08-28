// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.util;

import edu.wpi.first.util.WPIUtilJNI;

/** Storage for MathShared object. */
public final class MathSharedStore {
  private static MathShared mathShared;

  private MathSharedStore() {}

  /**
   * Get the MathShared object.
   *
   * @return The MathShared object.
   */
  public static synchronized MathShared getMathShared() {
    if (mathShared == null) {
      mathShared =
          new MathShared() {
            @Override
            public void reportError(String error, StackTraceElement[] stackTrace) {}

            @Override
            public void reportUsage(String resource, String data) {}

            @Override
            public double getTimestamp() {
              return WPIUtilJNI.now() * 1.0e-6;
            }
          };
    }
    return mathShared;
  }

  /**
   * Set the MathShared object.
   *
   * @param shared The MathShared object.
   */
  public static synchronized void setMathShared(MathShared shared) {
    mathShared = shared;
  }

  /**
   * Report an error.
   *
   * @param error the error to set
   * @param stackTrace array of stacktrace elements
   */
  public static void reportError(String error, StackTraceElement[] stackTrace) {
    getMathShared().reportError(error, stackTrace);
  }

  /**
   * Report usage.
   *
   * @param resource the resource name
   * @param data arbitrary string data
   */
  public static void reportUsage(String resource, String data) {
    getMathShared().reportUsage(resource, data);
  }

  /**
   * Get the time.
   *
   * @return The time in seconds.
   */
  public static double getTimestamp() {
    return getMathShared().getTimestamp();
  }
}
