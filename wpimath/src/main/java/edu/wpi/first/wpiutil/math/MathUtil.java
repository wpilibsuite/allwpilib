/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil.math;

public final class MathUtil {
  private MathUtil() {
    throw new AssertionError("utility class");
  }

  /**
   * Returns value clamped between low and high boundaries.
   *
   * @param value Value to clamp.
   * @param low   The lower boundary to which to clamp value.
   * @param high  The higher boundary to which to clamp value.
   */
  public static int clamp(int value, int low, int high) {
    return Math.max(low, Math.min(value, high));
  }

  /**
   * Returns value clamped between low and high boundaries.
   *
   * @param value Value to clamp.
   * @param low   The lower boundary to which to clamp value.
   * @param high  The higher boundary to which to clamp value.
   */
  public static double clamp(double value, double low, double high) {
    return Math.max(low, Math.min(value, high));
  }

  /**
   * Constrains theta to within the range (-pi, pi].
   *
   * @param theta The angle to normalize.
   * @return The normalized angle.
   */
  @SuppressWarnings("LocalVariableName")
  public static double normalizeAngle(double theta) {
    // Constraint theta to within (-3pi, pi)
    int nPiPos = (int) ((theta + Math.PI) / 2.0 / Math.PI);
    theta -= nPiPos * 2.0 * Math.PI;

    // Cut off the bottom half of the above range to constrain within
    // (-pi, pi]
    int nPiNeg = (int) ((theta - Math.PI) / 2.0 / Math.PI);
    theta -= nPiNeg * 2.0 * Math.PI;

    return theta;
  }
}
