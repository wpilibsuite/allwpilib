// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.wpilib.units.Units.MetersPerSecond;

import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.kinematics.proto.MecanumDriveWheelVelocitiesProto;
import org.wpilib.math.kinematics.struct.MecanumDriveWheelVelocitiesStruct;
import org.wpilib.units.measure.LinearVelocity;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/** Represents the wheel velocities for a mecanum drive drivetrain. */
public class MecanumDriveWheelVelocities
    implements Interpolatable<MecanumDriveWheelVelocities>,
        ProtobufSerializable,
        StructSerializable {
  /** Velocity of the front left wheel in meters per second. */
  public double frontLeft;

  /** Velocity of the front right wheel in meters per second. */
  public double frontRight;

  /** Velocity of the rear left wheel in meters per second. */
  public double rearLeft;

  /** Velocity of the rear right wheel in meters per second. */
  public double rearRight;

  /** MecanumDriveWheelVelocities protobuf for serialization. */
  public static final MecanumDriveWheelVelocitiesProto proto =
      new MecanumDriveWheelVelocitiesProto();

  /** MecanumDriveWheelVelocities struct for serialization. */
  public static final MecanumDriveWheelVelocitiesStruct struct =
      new MecanumDriveWheelVelocitiesStruct();

  /** Constructs a MecanumDriveWheelVelocities with zeros for all member fields. */
  public MecanumDriveWheelVelocities() {}

  /**
   * Constructs a MecanumDriveWheelVelocities.
   *
   * @param frontLeft Velocity of the front left wheel in meters per second.
   * @param frontRight Velocity of the front right wheel in meters per second.
   * @param rearLeft Velocity of the rear left wheel in meters per second.
   * @param rearRight Velocity of the rear right wheel in meters per second.
   */
  public MecanumDriveWheelVelocities(
      double frontLeft, double frontRight, double rearLeft, double rearRight) {
    this.frontLeft = frontLeft;
    this.frontRight = frontRight;
    this.rearLeft = rearLeft;
    this.rearRight = rearRight;
  }

  /**
   * Constructs a MecanumDriveWheelVelocities.
   *
   * @param frontLeft Velocity of the front left wheel in meters per second.
   * @param frontRight Velocity of the front right wheel in meters per second.
   * @param rearLeft Velocity of the rear left wheel in meters per second.
   * @param rearRight Velocity of the rear right wheel in meters per second.
   */
  public MecanumDriveWheelVelocities(
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
   * Renormalizes the wheel velocities if any individual velocity is above the specified maximum.
   *
   * <p>Sometimes, after inverse kinematics, the requested velocity from one or more wheels may be
   * above the max attainable velocity for the driving motor on that wheel. To fix this issue, one
   * can reduce all the wheel velocities to make sure that all requested module velocities are
   * at-or-below the absolute threshold, while maintaining the ratio of velocities between wheels.
   *
   * @param attainableMaxVelocity The absolute max velocity in meters per second that a wheel can
   *     reach.
   * @return Desaturated MecanumDriveWheelVelocities.
   */
  public MecanumDriveWheelVelocities desaturate(double attainableMaxVelocity) {
    double realMaxVelocity = Math.max(Math.abs(frontLeft), Math.abs(frontRight));
    realMaxVelocity = Math.max(realMaxVelocity, Math.abs(rearLeft));
    realMaxVelocity = Math.max(realMaxVelocity, Math.abs(rearRight));

    if (realMaxVelocity > attainableMaxVelocity) {
      return new MecanumDriveWheelVelocities(
          frontLeft / realMaxVelocity * attainableMaxVelocity,
          frontRight / realMaxVelocity * attainableMaxVelocity,
          rearLeft / realMaxVelocity * attainableMaxVelocity,
          rearRight / realMaxVelocity * attainableMaxVelocity);
    }

    return new MecanumDriveWheelVelocities(frontLeft, frontRight, rearLeft, rearRight);
  }

  /**
   * Renormalizes the wheel velocities if any individual velocity is above the specified maximum.
   *
   * <p>Sometimes, after inverse kinematics, the requested velocity from one or more wheels may be
   * above the max attainable velocity for the driving motor on that wheel. To fix this issue, one
   * can reduce all the wheel velocities to make sure that all requested module velocities are
   * at-or-below the absolute threshold, while maintaining the ratio of velocities between wheels.
   *
   * @param attainableMaxVelocity The absolute max velocity that a wheel can reach.
   * @return Desaturated MecanumDriveWheelVelocities.
   */
  public MecanumDriveWheelVelocities desaturate(LinearVelocity attainableMaxVelocity) {
    return desaturate(attainableMaxVelocity.in(MetersPerSecond));
  }

  /**
   * Adds two MecanumDriveWheelVelocities and returns the sum.
   *
   * <p>For example, MecanumDriveWheelVelocities{1.0, 0.5, 2.0, 1.5} +
   * MecanumDriveWheelVelocities{2.0, 1.5, 0.5, 1.0} = MecanumDriveWheelVelocities{3.0, 2.0, 2.5,
   * 2.5}
   *
   * @param other The MecanumDriveWheelVelocities to add.
   * @return The sum of the MecanumDriveWheelVelocities.
   */
  public MecanumDriveWheelVelocities plus(MecanumDriveWheelVelocities other) {
    return new MecanumDriveWheelVelocities(
        frontLeft + other.frontLeft,
        frontRight + other.frontRight,
        rearLeft + other.rearLeft,
        rearRight + other.rearRight);
  }

  /**
   * Subtracts the other MecanumDriveWheelVelocities from the current MecanumDriveWheelVelocities
   * and returns the difference.
   *
   * <p>For example, MecanumDriveWheelVelocities{5.0, 4.0, 6.0, 2.5} -
   * MecanumDriveWheelVelocities{1.0, 2.0, 3.0, 0.5} = MecanumDriveWheelVelocities{4.0, 2.0, 3.0,
   * 2.0}
   *
   * @param other The MecanumDriveWheelVelocities to subtract.
   * @return The difference between the two MecanumDriveWheelVelocities.
   */
  public MecanumDriveWheelVelocities minus(MecanumDriveWheelVelocities other) {
    return new MecanumDriveWheelVelocities(
        frontLeft - other.frontLeft,
        frontRight - other.frontRight,
        rearLeft - other.rearLeft,
        rearRight - other.rearRight);
  }

  /**
   * Returns the inverse of the current MecanumDriveWheelVelocities. This is equivalent to negating
   * all components of the MecanumDriveWheelVelocities.
   *
   * @return The inverse of the current MecanumDriveWheelVelocities.
   */
  public MecanumDriveWheelVelocities unaryMinus() {
    return new MecanumDriveWheelVelocities(-frontLeft, -frontRight, -rearLeft, -rearRight);
  }

  /**
   * Multiplies the MecanumDriveWheelVelocities by a scalar and returns the new
   * MecanumDriveWheelVelocities.
   *
   * <p>For example, MecanumDriveWheelVelocities{2.0, 2.5, 3.0, 3.5} * 2 =
   * MecanumDriveWheelVelocities{4.0, 5.0, 6.0, 7.0}
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled MecanumDriveWheelVelocities.
   */
  public MecanumDriveWheelVelocities times(double scalar) {
    return new MecanumDriveWheelVelocities(
        frontLeft * scalar, frontRight * scalar, rearLeft * scalar, rearRight * scalar);
  }

  /**
   * Divides the MecanumDriveWheelVelocities by a scalar and returns the new
   * MecanumDriveWheelVelocities.
   *
   * <p>For example, MecanumDriveWheelVelocities{2.0, 2.5, 1.5, 1.0} / 2 =
   * MecanumDriveWheelVelocities{1.0, 1.25, 0.75, 0.5}
   *
   * @param scalar The scalar to divide by.
   * @return The scaled MecanumDriveWheelVelocities.
   */
  public MecanumDriveWheelVelocities div(double scalar) {
    return new MecanumDriveWheelVelocities(
        frontLeft / scalar, frontRight / scalar, rearLeft / scalar, rearRight / scalar);
  }

  /**
   * Returns the linear interpolation of this MecanumDriveWheelVelocities and another.
   *
   * @param endValue The end value for the interpolation.
   * @param t How far between the two values to interpolate. This is clamped to [0, 1].
   * @return The interpolated value.
   */
  @Override
  public MecanumDriveWheelVelocities interpolate(MecanumDriveWheelVelocities endValue, double t) {
    // Clamp t to [0, 1]
    t = Math.max(0.0, Math.min(1.0, t));

    return new MecanumDriveWheelVelocities(
        frontLeft + t * (endValue.frontLeft - frontLeft),
        frontRight + t * (endValue.frontRight - frontRight),
        rearLeft + t * (endValue.rearLeft - rearLeft),
        rearRight + t * (endValue.rearRight - rearRight));
  }

  @Override
  public String toString() {
    return String.format(
        "MecanumDriveWheelVelocities(Front Left: %.2f m/s, Front Right: %.2f m/s, "
            + "Rear Left: %.2f m/s, Rear Right: %.2f m/s)",
        frontLeft, frontRight, rearLeft, rearRight);
  }
}
