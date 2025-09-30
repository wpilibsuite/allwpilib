// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static edu.wpi.first.units.Units.MetersPerSecond;

import edu.wpi.first.math.interpolation.Interpolatable;
import edu.wpi.first.math.kinematics.proto.MecanumDriveWheelSpeedsProto;
import edu.wpi.first.math.kinematics.struct.MecanumDriveWheelSpeedsStruct;
import edu.wpi.first.units.measure.LinearVelocity;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;

/** Represents the wheel speeds for a mecanum drive drivetrain. */
public class MecanumDriveWheelSpeeds
    implements Interpolatable<MecanumDriveWheelSpeeds>, ProtobufSerializable, StructSerializable {
  /** Speed of the front left wheel in meters per second. */
  public double frontLeft;

  /** Speed of the front right wheel in meters per second. */
  public double frontRight;

  /** Speed of the rear left wheel in meters per second. */
  public double rearLeft;

  /** Speed of the rear right wheel in meters per second. */
  public double rearRight;

  /** MecanumDriveWheelSpeeds protobuf for serialization. */
  public static final MecanumDriveWheelSpeedsProto proto = new MecanumDriveWheelSpeedsProto();

  /** MecanumDriveWheelSpeeds struct for serialization. */
  public static final MecanumDriveWheelSpeedsStruct struct = new MecanumDriveWheelSpeedsStruct();

  /** Constructs a MecanumDriveWheelSpeeds with zeros for all member fields. */
  public MecanumDriveWheelSpeeds() {}

  /**
   * Constructs a MecanumDriveWheelSpeeds.
   *
   * @param frontLeft Speed of the front left wheel in meters per second.
   * @param frontRight Speed of the front right wheel in meters per second.
   * @param rearLeft Speed of the rear left wheel in meters per second.
   * @param rearRight Speed of the rear right wheel in meters per second.
   */
  public MecanumDriveWheelSpeeds(
      double frontLeft, double frontRight, double rearLeft, double rearRight) {
    this.frontLeft = frontLeft;
    this.frontRight = frontRight;
    this.rearLeft = rearLeft;
    this.rearRight = rearRight;
  }

  /**
   * Constructs a MecanumDriveWheelSpeeds.
   *
   * @param frontLeft Speed of the front left wheel in meters per second.
   * @param frontRight Speed of the front right wheel in meters per second.
   * @param rearLeft Speed of the rear left wheel in meters per second.
   * @param rearRight Speed of the rear right wheel in meters per second.
   */
  public MecanumDriveWheelSpeeds(
      LinearVelocity frontLeft,
      LinearVelocity frontRight,
      LinearVelocity rearLeft,
      LinearVelocity rearRight) {
    this(
        frontLeft.in(MetersPerSecond),
        frontRight.in(MetersPerSecond),
        rearLeft.in(MetersPerSecond),
        rearRight.in(MetersPerSecond));
  }

  /**
   * Renormalizes the wheel speeds if any individual speed is above the specified maximum.
   *
   * <p>Sometimes, after inverse kinematics, the requested speed from one or more wheels may be
   * above the max attainable speed for the driving motor on that wheel. To fix this issue, one can
   * reduce all the wheel speeds to make sure that all requested module speeds are at-or-below the
   * absolute threshold, while maintaining the ratio of speeds between wheels.
   *
   * @param attainableMaxSpeed The absolute max speed in meters per second that a wheel can reach.
   * @return Desaturated MecanumDriveWheelSpeeds.
   */
  public MecanumDriveWheelSpeeds desaturate(double attainableMaxSpeed) {
    double realMaxSpeed = Math.max(Math.abs(frontLeft), Math.abs(frontRight));
    realMaxSpeed = Math.max(realMaxSpeed, Math.abs(rearLeft));
    realMaxSpeed = Math.max(realMaxSpeed, Math.abs(rearRight));

    if (realMaxSpeed > attainableMaxSpeed) {
      return new MecanumDriveWheelSpeeds(
          frontLeft / realMaxSpeed * attainableMaxSpeed,
          frontRight / realMaxSpeed * attainableMaxSpeed,
          rearLeft / realMaxSpeed * attainableMaxSpeed,
          rearRight / realMaxSpeed * attainableMaxSpeed);
    }

    return new MecanumDriveWheelSpeeds(frontLeft, frontRight, rearLeft, rearRight);
  }

  /**
   * Renormalizes the wheel speeds if any individual speed is above the specified maximum.
   *
   * <p>Sometimes, after inverse kinematics, the requested speed from one or more wheels may be
   * above the max attainable speed for the driving motor on that wheel. To fix this issue, one can
   * reduce all the wheel speeds to make sure that all requested module speeds are at-or-below the
   * absolute threshold, while maintaining the ratio of speeds between wheels.
   *
   * @param attainableMaxSpeed The absolute max speed that a wheel can reach.
   * @return Desaturated MecanumDriveWheelSpeeds.
   */
  public MecanumDriveWheelSpeeds desaturate(LinearVelocity attainableMaxSpeed) {
    return desaturate(attainableMaxSpeed.in(MetersPerSecond));
  }

  /**
   * Adds two MecanumDriveWheelSpeeds and returns the sum.
   *
   * <p>For example, MecanumDriveWheelSpeeds{1.0, 0.5, 2.0, 1.5} + MecanumDriveWheelSpeeds{2.0, 1.5,
   * 0.5, 1.0} = MecanumDriveWheelSpeeds{3.0, 2.0, 2.5, 2.5}
   *
   * @param other The MecanumDriveWheelSpeeds to add.
   * @return The sum of the MecanumDriveWheelSpeeds.
   */
  public MecanumDriveWheelSpeeds plus(MecanumDriveWheelSpeeds other) {
    return new MecanumDriveWheelSpeeds(
        frontLeft + other.frontLeft,
        frontRight + other.frontRight,
        rearLeft + other.rearLeft,
        rearRight + other.rearRight);
  }

  /**
   * Subtracts the other MecanumDriveWheelSpeeds from the current MecanumDriveWheelSpeeds and
   * returns the difference.
   *
   * <p>For example, MecanumDriveWheelSpeeds{5.0, 4.0, 6.0, 2.5} - MecanumDriveWheelSpeeds{1.0, 2.0,
   * 3.0, 0.5} = MecanumDriveWheelSpeeds{4.0, 2.0, 3.0, 2.0}
   *
   * @param other The MecanumDriveWheelSpeeds to subtract.
   * @return The difference between the two MecanumDriveWheelSpeeds.
   */
  public MecanumDriveWheelSpeeds minus(MecanumDriveWheelSpeeds other) {
    return new MecanumDriveWheelSpeeds(
        frontLeft - other.frontLeft,
        frontRight - other.frontRight,
        rearLeft - other.rearLeft,
        rearRight - other.rearRight);
  }

  /**
   * Returns the inverse of the current MecanumDriveWheelSpeeds. This is equivalent to negating all
   * components of the MecanumDriveWheelSpeeds.
   *
   * @return The inverse of the current MecanumDriveWheelSpeeds.
   */
  public MecanumDriveWheelSpeeds unaryMinus() {
    return new MecanumDriveWheelSpeeds(-frontLeft, -frontRight, -rearLeft, -rearRight);
  }

  /**
   * Multiplies the MecanumDriveWheelSpeeds by a scalar and returns the new MecanumDriveWheelSpeeds.
   *
   * <p>For example, MecanumDriveWheelSpeeds{2.0, 2.5, 3.0, 3.5} * 2 = MecanumDriveWheelSpeeds{4.0,
   * 5.0, 6.0, 7.0}
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled MecanumDriveWheelSpeeds.
   */
  public MecanumDriveWheelSpeeds times(double scalar) {
    return new MecanumDriveWheelSpeeds(
        frontLeft * scalar, frontRight * scalar, rearLeft * scalar, rearRight * scalar);
  }

  /**
   * Divides the MecanumDriveWheelSpeeds by a scalar and returns the new MecanumDriveWheelSpeeds.
   *
   * <p>For example, MecanumDriveWheelSpeeds{2.0, 2.5, 1.5, 1.0} / 2 = MecanumDriveWheelSpeeds{1.0,
   * 1.25, 0.75, 0.5}
   *
   * @param scalar The scalar to divide by.
   * @return The scaled MecanumDriveWheelSpeeds.
   */
  public MecanumDriveWheelSpeeds div(double scalar) {
    return new MecanumDriveWheelSpeeds(
        frontLeft / scalar, frontRight / scalar, rearLeft / scalar, rearRight / scalar);
  }

  /**
   * Returns the linear interpolation of this MecanumDriveWheelSpeeds and another.
   *
   * @param endValue The end value for the interpolation.
   * @param t How far between the two values to interpolate. This is clamped to [0, 1].
   * @return The interpolated value.
   */
  @Override
  public MecanumDriveWheelSpeeds interpolate(MecanumDriveWheelSpeeds endValue, double t) {
    // Clamp t to [0, 1]
    t = Math.max(0.0, Math.min(1.0, t));

    return new MecanumDriveWheelSpeeds(
        frontLeft + t * (endValue.frontLeft - frontLeft),
        frontRight + t * (endValue.frontRight - frontRight),
        rearLeft + t * (endValue.rearLeft - rearLeft),
        rearRight + t * (endValue.rearRight - rearRight));
  }

  @Override
  public String toString() {
    return String.format(
        "MecanumDriveWheelSpeeds(Front Left: %.2f m/s, Front Right: %.2f m/s, "
            + "Rear Left: %.2f m/s, Rear Right: %.2f m/s)",
        frontLeft, frontRight, rearLeft, rearRight);
  }
}
