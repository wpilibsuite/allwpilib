// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

public final class MathSharedStore {
  private static MathShared mathShared;

  private MathSharedStore() {
  }

  /**
   * get the MathShared object.
   */
  public static synchronized MathShared getMathShared() {
    if (mathShared == null) {
      mathShared = new MathShared() {
        @Override
        public void reportError(String error, StackTraceElement[] stackTrace) {
        }

        @Override
        public void reportUsage(MathUsageId id, int count) {
        }
      };
    }
    return mathShared;
  }

  /**
   * set the MathShared object.
   */
  public static synchronized void setMathShared(MathShared shared) {
    mathShared = shared;
  }

  /**
   * Report an error.
   *
   * @param error the error to set
   */
  public static void reportError(String error, StackTraceElement[] stackTrace) {
    getMathShared().reportError(error, stackTrace);
  }

  /**
   * Report usage.
   *
   * @param id the usage id
   * @param count the usage count
   */
  public static void reportUsage(MathUsageId id, int count) {
    getMathShared().reportUsage(id, count);
  }
}
