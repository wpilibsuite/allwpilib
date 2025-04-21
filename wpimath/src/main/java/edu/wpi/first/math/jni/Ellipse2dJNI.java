// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.jni;

/** Ellipse2d JNI. */
public final class Ellipse2dJNI extends WPIMathJNI {
  /**
   * Returns the nearest point that is contained within the ellipse.
   *
   * <p>Constructs an Ellipse2d object and runs its nearest() method.
   *
   * @param centerX The x coordinate of the center of the ellipse in meters.
   * @param centerY The y coordinate of the center of the ellipse in meters.
   * @param centerHeading The ellipse's rotation in radians.
   * @param xSemiAxis The x semi-axis in meters.
   * @param ySemiAxis The y semi-axis in meters.
   * @param pointX The x coordinate of the point that this will find the nearest point to.
   * @param pointY The y coordinate of the point that this will find the nearest point to.
   * @param nearestPoint Array to store nearest point into.
   */
  public static native void nearest(
      double centerX,
      double centerY,
      double centerHeading,
      double xSemiAxis,
      double ySemiAxis,
      double pointX,
      double pointY,
      double[] nearestPoint);

  /** Utility class. */
  private Ellipse2dJNI() {}
}
