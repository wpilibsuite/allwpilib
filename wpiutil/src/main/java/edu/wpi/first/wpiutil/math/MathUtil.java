/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
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
   * Returns value clamped between low and high boundaries.
   * This overload clamps the value between the positive and negative values of bounds, 
   * therefore {@code clamp(val, bound)} is equivalent to {@code clamp(val, -bound, bound)}.
   *
   * @param value   Value to clamp.
   * @param bounds  The absolute value of the boundaries to which to clamp value 
   */
  public static double clamp(double value, double bounds){
    return clamp(value, -bounds, bounds);
  }
  
  /**
   * Returns value clamped between low and high boundaries.
   * This overload clamps the value between the positive and negative values of bounds, 
   * therefore {@code clamp(val, bound)} is equivalent to {@code clamp(val, -bound, bound)}.
   *
   * @param value   Value to clamp.
   * @param bounds  The absolute value of the boundaries to which to clamp value 
   */
  public static int clamp(int value, int bounds){
    return clamp(value, -bounds, bounds);
  }
}
