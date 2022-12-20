// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.util;

public final class FalconUnitConversion {
  private static final double kFalconIntegratedEncoderResolution = 2048;

  /** Utility class, so constructor is private. */
  private FalconUnitConversion() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Convert Falcon500 integrated encoder velocity to angular velocity
   *
   * @param falconVelocity velocity reported by Falcon500. ticks per 100ms
   * @param gearRatio gear ratio between motor and rotating object. Number of input rotations /
   *     number of output rotations
   * @return angular velocity of rotating object in rad/s
   */
  public static double falcon500VelocityToRadiansPerSecond(
      double falconVelocity, double gearRatio) {
    return Units.rotationsPerMinuteToRadiansPerSecond(
            falcon500VelocityToRotationsPerMinute(falconVelocity))
        / gearRatio;
  }

  /**
   * Convert Falcon500 integrated encoder velocity to angular velocity. Assumes there is no gearbox
   *
   * @param falconVelocity velocity reported by Falcon500. ticks per 100ms
   * @return angular velocity of rotating object in rad/s
   */
  public static double falcon500VelocityToRadiansPerSecond(double falconVelocity) {
    return falcon500VelocityToRadiansPerSecond(falconVelocity, 1);
  }

  /**
   * Convert angular velocity of an object to Falcon500 native velocity units, ticks per 100ms
   *
   * @param angularVelocity angular velocity of object in rad/s
   * @param gearRatio gear ratio between motor and rotating object. Number of input rotations /
   *     number of output rotations
   * @return Falcon500 native velocity units, ticks per 100ms
   */
  public static double radiansPerSecondToFalcon500Velocity(
      double angularVelocity, double gearRatio) {
    return rotationsPerMinuteToFalcon500Velocity(
            Units.radiansPerSecondToRotationsPerMinute(angularVelocity))
        * gearRatio;
  }

  /**
   * Convert angular velocity of an object to Falcon500 native velocity units, ticks per 100ms.
   * Assumes there is no gearbox
   *
   * @param angularVelocity angular velocity of object in rad/s
   * @return Falcon500 native velocity units, ticks per 100ms
   */
  public static double radiansPerSecondToFalcon500Velocity(double angularVelocity) {
    return radiansPerSecondToFalcon500Velocity(angularVelocity, 1);
  }

  /**
   * Convert Falcon500 native velocity units, ticks per 100ms to linear velocity
   *
   * @param falconVelocity linear velocity in meters per second
   * @param radius radius of rotating object in meters
   * @param gearRatio gear ratio between motor and rotating object. Number of input rotations /
   *     number of output rotations
   * @return linear velocity of rotating object in meters per second
   */
  public static double falcon500VelocityToMetersPerSecond(
      double falconVelocity, double radius, double gearRatio) {
    return Units.rotationsPerMinuteToMetersPerSecond(
            falcon500VelocityToRotationsPerMinute(falconVelocity), radius)
        / gearRatio;
  }

  /**
   * Convert Falcon500 native velocity units, ticks per 100ms to linear velocity. Assumes there is
   * no gearbox
   *
   * @param falconVelocity linear velocity in meters per second
   * @param radius radius of rotating object in meters
   * @return linear velocity of rotating object in meters per second
   */
  public static double falcon500VelocityToMetersPerSecond(double falconVelocity, double radius) {
    return falcon500VelocityToMetersPerSecond(falconVelocity, radius, 1);
  }

  /**
   * Convert linear velocity to Falcon500 native velocity units, ticks per 100ms.
   *
   * @param linearVelocity of object in meters per second
   * @param radius radius of rotating object
   * @param gearRatio gear ratio between motor and rotating object. Number of input rotations /
   *     number of output rotations
   * @return Falcon500 native velocity units, ticks per 100ms
   */
  public static double metersPerSecondToFalcon500Velocity(
      double linearVelocity, double radius, double gearRatio) {
    return rotationsPerMinuteToFalcon500Velocity(
            Units.metersPerSecondToRotationPerMinute(linearVelocity, radius))
        * gearRatio;
  }

  /**
   * Convert linear velocity to Falcon500 native velocity units, ticks per 100ms. Assumes there is
   * no gearbox
   *
   * @param linearVelocity of object in meters per second
   * @param radius radius of rotating object
   * @return Falcon500 native velocity units, ticks per 100ms
   */
  public static double metersPerSecondToFalcon500Velocity(double linearVelocity, double radius) {
    return metersPerSecondToFalcon500Velocity(linearVelocity, radius, 1);
  }

  /**
   * Convert Falcon500 native velocity units, ticks per 100ms, to RPM
   *
   * @param falconVelocity Falcon500 native velocity units, ticks per 100ms, of rotating object
   * @return rotations per minute
   */
  public static double falcon500VelocityToRotationsPerMinute(double falconVelocity) {
    return falconVelocity * 600.0 / kFalconIntegratedEncoderResolution;
  }

  /**
   * Convert RPM to Falcon500 native velocity units, ticks per 100ms
   *
   * @param rpm rotations per minute
   * @return Falcon500 native velocity units, ticks per 100ms, of rotating object
   */
  public static double rotationsPerMinuteToFalcon500Velocity(double rpm) {
    return rpm * kFalconIntegratedEncoderResolution / 600.0;
  }

  /**
   * Convert the position reported by the Falcon500 integrated encoder position to degrees
   *
   * @param falconPosition Falcon500 native position units
   * @return Encoder position in degrees
   */
  public static double falcon500PositionToDegrees(double falconPosition) {
    return (falconPosition / kFalconIntegratedEncoderResolution) * 360;
  }

  /**
   * Convert the position reported by the Falcon500 integrated encoder position to radians
   *
   * @param falconPosition Falcon500 native position units
   * @return Encoder position in radians
   */
  public static double falcon500PositionToRadians(double falconPosition) {
    return Math.toRadians(falcon500PositionToDegrees(falconPosition));
  }

  /**
   * Convert position in degrees to Falcon500 native position units
   *
   * @param positionDegrees position in degrees
   * @return Falcon500 native position units
   */
  public static double degreesToFalcon500Position(double positionDegrees) {
    double optimizedPosition = positionDegrees % 360;
    return (optimizedPosition / 360.0) * kFalconIntegratedEncoderResolution;
  }

  /**
   * Convert position in radians to Falcon500 native position units
   *
   * @param positionRadians position in radians
   * @return Falcon500 native position units
   */
  public static double radiansToFalcon500Position(double positionRadians) {
    return degreesToFalcon500Position(Math.toDegrees(positionRadians));
  }
}
