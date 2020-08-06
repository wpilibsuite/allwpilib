/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.math;

public interface MathShared {
  /**
   * Report an error.
   *
   * @param error the error to set
   */
  void reportError(String error, StackTraceElement[] stackTrace);

  /**
   * Report usage.
   *
   * @param id the usage id
   * @param count the usage count
   */
  void reportUsage(MathUsageId id, int count);
}
