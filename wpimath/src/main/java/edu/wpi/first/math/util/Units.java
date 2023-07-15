// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.util;

/** Utility class that converts between commonly used units in FRC. */
public final class Units {
  private static final double kInchesPerFoot = 12.0;
  private static final double kMetersPerInch = 0.0254;
  private static final double kSecondsPerMinute = 60;
  private static final double kMillisecondsPerSecond = 1000;
  private static final double kKilogramsPerLb = 0.453592;

  /** Utility class, so constructor is private. */
  private Units() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Converts given meters to feet.
   *
   * @param meters The meters to convert to feet.
   * @return Feet converted from meters.
   */
  public static double metersToFeet(double meters) {
    return metersToInches(meters) / kInchesPerFoot;
  }

  /**
   * Converts given feet to meters.
   *
   * @param feet The feet to convert to meters.
   * @return Meters converted from feet.
   */
  public static double feetToMeters(double feet) {
    return inchesToMeters(feet * kInchesPerFoot);
  }

  /**
   * Converts given meters to inches.
   *
   * @param meters The meters to convert to inches.
   * @return Inches converted from meters.
   */
  public static double metersToInches(double meters) {
    return meters / kMetersPerInch;
  }

  /**
   * Converts given inches to meters.
   *
   * @param inches The inches to convert to meters.
   * @return Meters converted from inches.
   */
  public static double inchesToMeters(double inches) {
    return inches * kMetersPerInch;
  }

  /**
   * Converts given degrees to radians.
   *
   * @param degrees The degrees to convert to radians.
   * @return Radians converted from degrees.
   */
  public static double degreesToRadians(double degrees) {
    return Math.toRadians(degrees);
  }

  /**
   * Converts given radians to degrees.
   *
   * @param radians The radians to convert to degrees.
   * @return Degrees converted from radians.
   */
  public static double radiansToDegrees(double radians) {
    return Math.toDegrees(radians);
  }

  /**
   * Converts given radians to rotations.
   *
   * @param radians The radians to convert.
   * @return rotations Converted from radians.
   */
  public static double radiansToRotations(double radians) {
    return radians / (Math.PI * 2);
  }

  /**
   * Converts given degrees to rotations.
   *
   * @param degrees The degrees to convert.
   * @return rotations Converted from degrees.
   */
  public static double degreesToRotations(double degrees) {
    return degrees / 360;
  }

  /**
   * Converts given rotations to degrees.
   *
   * @param rotations The rotations to convert.
   * @return degrees Converted from rotations.
   */
  public static double rotationsToDegrees(double rotations) {
    return rotations * 360;
  }

  /**
   * Converts given rotations to radians.
   *
   * @param rotations The rotations to convert.
   * @return radians Converted from rotations.
   */
  public static double rotationsToRadians(double rotations) {
    return rotations * 2 * Math.PI;
  }

  /**
   * Converts rotations per minute to radians per second.
   *
   * @param rpm The rotations per minute to convert to radians per second.
   * @return Radians per second converted from rotations per minute.
   */
  public static double rotationsPerMinuteToRadiansPerSecond(double rpm) {
    return rpm * Math.PI / (kSecondsPerMinute / 2);
  }

  /**
   * Converts radians per second to rotations per minute.
   *
   * @param radiansPerSecond The radians per second to convert to from rotations per minute.
   * @return Rotations per minute converted from radians per second.
   */
  public static double radiansPerSecondToRotationsPerMinute(double radiansPerSecond) {
    return radiansPerSecond * (kSecondsPerMinute / 2) / Math.PI;
  }

  /**
   * Converts given milliseconds to seconds.
   *
   * @param milliseconds The milliseconds to convert to seconds.
   * @return Seconds converted from milliseconds.
   */
  public static double millisecondsToSeconds(double milliseconds) {
    return milliseconds / kMillisecondsPerSecond;
  }

  /**
   * Converts given seconds to milliseconds.
   *
   * @param seconds The seconds to convert to milliseconds.
   * @return Milliseconds converted from seconds.
   */
  public static double secondsToMilliseconds(double seconds) {
    return seconds * kMillisecondsPerSecond;
  }

  /**
   * Converts kilograms into lbs (pound-mass).
   *
   * @param kilograms The kilograms to convert to lbs (pound-mass).
   * @return Lbs (pound-mass) converted from kilograms.
   */
  public static double kilogramsToLbs(double kilograms) {
    return kilograms / kKilogramsPerLb;
  }

  /**
   * Converts lbs (pound-mass) into kilograms.
   *
   * @param lbs The lbs (pound-mass) to convert to kilograms.
   * @return Kilograms converted from lbs (pound-mass).
   */
  public static double lbsToKilograms(double lbs) {
    return lbs * kKilogramsPerLb;
  }
}
