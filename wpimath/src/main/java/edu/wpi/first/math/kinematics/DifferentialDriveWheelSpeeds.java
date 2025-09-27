// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static edu.wpi.first.units.Units.MetersPerSecond;

import edu.wpi.first.math.interpolation.Interpolatable;
import edu.wpi.first.math.kinematics.proto.DifferentialDriveWheelSpeedsProto;
import edu.wpi.first.math.kinematics.struct.DifferentialDriveWheelSpeedsStruct;
import edu.wpi.first.units.measure.LinearVelocity;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;

/** Represents the wheel speeds for a differential drive drivetrain. */
public class DifferentialDriveWheelSpeeds
    implements Interpolatable<DifferentialDriveWheelSpeeds>,
        ProtobufSerializable,
        StructSerializable {
  /** Speed of the left side of the robot in meters per second. */
  public double left;

  /** Speed of the right side of the robot in meters per second. */
  public double right;

  /** DifferentialDriveWheelSpeeds protobuf for serialization. */
  public static final DifferentialDriveWheelSpeedsProto proto =
      new DifferentialDriveWheelSpeedsProto();

  /** DifferentialDriveWheelSpeeds struct for serialization. */
  public static final DifferentialDriveWheelSpeedsStruct struct =
      new DifferentialDriveWheelSpeedsStruct();

  /** Constructs a DifferentialDriveWheelSpeeds with zeros for left and right speeds. */
  public DifferentialDriveWheelSpeeds() {}

  /**
   * Constructs a DifferentialDriveWheelSpeeds.
   *
   * @param left The left speed in meters per second.
   * @param right The right speed in meters per second.
   */
  public DifferentialDriveWheelSpeeds(double left, double right) {
    this.left = left;
    this.right = right;
  }

  /**
   * Constructs a DifferentialDriveWheelSpeeds.
   *
   * @param left The left speed in meters per second.
   * @param right The right speed in meters per second.
   */
  public DifferentialDriveWheelSpeeds(LinearVelocity left, LinearVelocity right) {
    this(left.in(MetersPerSecond), right.in(MetersPerSecond));
  }

  /**
   * Renormalizes the wheel speeds if any either side is above the specified maximum.
   *
   * <p>Sometimes, after inverse kinematics, the requested speed from one or more wheels may be
   * above the max attainable speed for the driving motor on that wheel. To fix this issue, one can
   * reduce all the wheel speeds to make sure that all requested module speeds are at-or-below the
   * absolute threshold, while maintaining the ratio of speeds between wheels.
   *
   * @param attainableMaxSpeed The absolute max speed in meters per second that a wheel can reach.
   */
  public void desaturate(double attainableMaxSpeed) {
    double realMaxSpeed = Math.max(Math.abs(left), Math.abs(right));

    if (realMaxSpeed > attainableMaxSpeed) {
      left = left / realMaxSpeed * attainableMaxSpeed;
      right = right / realMaxSpeed * attainableMaxSpeed;
    }
  }

  /**
   * Renormalizes the wheel speeds if any either side is above the specified maximum.
   *
   * <p>Sometimes, after inverse kinematics, the requested speed from one or more wheels may be
   * above the max attainable speed for the driving motor on that wheel. To fix this issue, one can
   * reduce all the wheel speeds to make sure that all requested module speeds are at-or-below the
   * absolute threshold, while maintaining the ratio of speeds between wheels.
   *
   * @param attainableMaxSpeed The absolute max speed in meters per second that a wheel can reach.
   */
  public void desaturate(LinearVelocity attainableMaxSpeed) {
    desaturate(attainableMaxSpeed.in(MetersPerSecond));
  }

  /**
   * Adds two DifferentialDriveWheelSpeeds and returns the sum.
   *
   * <p>For example, DifferentialDriveWheelSpeeds{1.0, 0.5} + DifferentialDriveWheelSpeeds{2.0, 1.5}
   * = DifferentialDriveWheelSpeeds{3.0, 2.0}
   *
   * @param other The DifferentialDriveWheelSpeeds to add.
   * @return The sum of the DifferentialDriveWheelSpeeds.
   */
  public DifferentialDriveWheelSpeeds plus(DifferentialDriveWheelSpeeds other) {
    return new DifferentialDriveWheelSpeeds(left + other.left, right + other.right);
  }

  /**
   * Subtracts the other DifferentialDriveWheelSpeeds from the current DifferentialDriveWheelSpeeds
   * and returns the difference.
   *
   * <p>For example, DifferentialDriveWheelSpeeds{5.0, 4.0} - DifferentialDriveWheelSpeeds{1.0, 2.0}
   * = DifferentialDriveWheelSpeeds{4.0, 2.0}
   *
   * @param other The DifferentialDriveWheelSpeeds to subtract.
   * @return The difference between the two DifferentialDriveWheelSpeeds.
   */
  public DifferentialDriveWheelSpeeds minus(DifferentialDriveWheelSpeeds other) {
    return new DifferentialDriveWheelSpeeds(left - other.left, right - other.right);
  }

  /**
   * Returns the inverse of the current DifferentialDriveWheelSpeeds. This is equivalent to negating
   * all components of the DifferentialDriveWheelSpeeds.
   *
   * @return The inverse of the current DifferentialDriveWheelSpeeds.
   */
  public DifferentialDriveWheelSpeeds unaryMinus() {
    return new DifferentialDriveWheelSpeeds(-left, -right);
  }

  /**
   * Multiplies the DifferentialDriveWheelSpeeds by a scalar and returns the new
   * DifferentialDriveWheelSpeeds.
   *
   * <p>For example, DifferentialDriveWheelSpeeds{2.0, 2.5} * 2 = DifferentialDriveWheelSpeeds{4.0,
   * 5.0}
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled DifferentialDriveWheelSpeeds.
   */
  public DifferentialDriveWheelSpeeds times(double scalar) {
    return new DifferentialDriveWheelSpeeds(left * scalar, right * scalar);
  }

  /**
   * Divides the DifferentialDriveWheelSpeeds by a scalar and returns the new
   * DifferentialDriveWheelSpeeds.
   *
   * <p>For example, DifferentialDriveWheelSpeeds{2.0, 2.5} / 2 = DifferentialDriveWheelSpeeds{1.0,
   * 1.25}
   *
   * @param scalar The scalar to divide by.
   * @return The scaled DifferentialDriveWheelSpeeds.
   */
  public DifferentialDriveWheelSpeeds div(double scalar) {
    return new DifferentialDriveWheelSpeeds(left / scalar, right / scalar);
  }

  /**
   * Returns the linear interpolation of this DifferentialDriveWheelSpeeds and another.
   *
   * @param endValue The end value for the interpolation.
   * @param t How far between the two values to interpolate. This is clamped to [0, 1].
   * @return The interpolated value.
   */
  @Override
  public DifferentialDriveWheelSpeeds interpolate(DifferentialDriveWheelSpeeds endValue, double t) {
    // Clamp t to [0, 1]
    t = Math.max(0.0, Math.min(1.0, t));

    return new DifferentialDriveWheelSpeeds(
        left + t * (endValue.left - left), right + t * (endValue.right - right));
  }

  @Override
  public String toString() {
    return String.format(
        "DifferentialDriveWheelSpeeds(Left: %.2f m/s, Right: %.2f m/s)", left, right);
  }
}
