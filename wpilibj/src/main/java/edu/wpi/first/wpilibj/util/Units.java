/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.util;

/**
 * Utility class that converts between commonly used units in FRC.
 */ 
public class Units {

  /**
   * Converts given meters to feet.
   *
   * @param meters The meters to convert to feet.
   * @return Inches The inches converted from meters.
   */
  public static double metersToFeet(double meters) {
    return metersToInches(meters) / 12.0;
  }

  /**
   * Converts given feet to meters.
   *
   * @param feet The feet to convert to meters.
   * @return Meters The meters converted from feet.
   */
  public static double feetToMeters(double feet) {
    return inchesToMeters(feet * 12.0);
  }

  /**
   * Converts given meters to inches.
   *
   * @param meters The meters to convert to inches.
   * @return Inches The inches converted from meters.
   */
  public static double metersToInches(double meters) {
    return meters / 0.0254;
  }

  /**
   * Converts given inches to meters.
   *
   * @param inches The inches to convert to meters.
   * @return Meters The meters converted from inches.
   */
  public static double inchesToMeters(double inches) {
    return inches * 0.0254;
  }

  /**
   * Converts given degrees to radians.
   *
   * @param degrees The degrees to convert to radians.
   * @return Radians The radians converted from degrees.
   */
  public static double degreesToRadian(double degrees) {
    return Math.toRadians(degrees);
  }

  /**
   * Converts given radians to degrees.
   *
   * @param radians The radians to convert to degrees.
   * @return Degrees The degrees converted from radians.
   */
  public static double radiansToDegrees(double radians) {
    return Math.toDegrees(radians);
  }

  /**
   * Converts rotations per minute to radians per second.
   *
   * @param rpm The rotations per minute to convert to radians per second.
   * @return Radians per second The radians per second converted from rotations per minute.
   */
  public static double rotationsPerMinuteToRadiansPerSecond(double rpm) {
    return (rpm * Math.PI) / 30.0;
  }

  /**
   * Converts radians per second to rotations per minute.
   *
   * @param radiansPerSecond The radians per second to convert to from rotations per minute.
   * @return Rotations per minute The rotations per minute converted from radians per second.
   */
  public static double radiansPerSecondToRotationsPerMinute(double radiansPerSecond) {
    return (radiansPerSecond * 30.0) / Math.PI;
  }
}