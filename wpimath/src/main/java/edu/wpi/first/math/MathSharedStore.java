/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
