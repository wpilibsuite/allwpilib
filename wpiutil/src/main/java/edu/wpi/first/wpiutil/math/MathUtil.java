/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                             */
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
  * Applies a deadband to a value.
  * 
  * @param value  The value to apply the deadband to.
  * @param deadband The deadband to apply.
  * @return If the value's offset from zero is smaller than the deadband, 0. 
  * Else, will return the entered value.
  */
  public static double applyDeadband(double value, double deadband) {
    return (Math.abs(value) > deadband)? value : 0;
  }
}
