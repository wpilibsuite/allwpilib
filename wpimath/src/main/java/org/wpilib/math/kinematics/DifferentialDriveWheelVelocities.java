// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.wpilib.units.Units.MetersPerSecond;

import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.kinematics.proto.DifferentialDriveWheelVelocitiesProto;
import org.wpilib.math.kinematics.struct.DifferentialDriveWheelVelocitiesStruct;
import org.wpilib.units.measure.LinearVelocity;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/** Represents the wheel velocities for a differential drive drivetrain. */
public class DifferentialDriveWheelVelocities
    implements Interpolatable<DifferentialDriveWheelVelocities>,
        ProtobufSerializable,
        StructSerializable {
  /** Velocity of the left side of the robot in meters per second. */
  public double left;

  /** Velocity of the right side of the robot in meters per second. */
  public double right;

  /** DifferentialDriveWheelVelocities protobuf for serialization. */
  public static final DifferentialDriveWheelVelocitiesProto proto =
      new DifferentialDriveWheelVelocitiesProto();

  /** DifferentialDriveWheelVelocities struct for serialization. */
  public static final DifferentialDriveWheelVelocitiesStruct struct =
      new DifferentialDriveWheelVelocitiesStruct();

  /** Constructs a DifferentialDriveWheelVelocities with zeros for left and right velocities. */
  public DifferentialDriveWheelVelocities() {}

  /**
   * Constructs a DifferentialDriveWheelVelocities.
   *
   * @param left The left velocity in meters per second.
   * @param right The right velocity in meters per second.
   */
  public DifferentialDriveWheelVelocities(double left, double right) {
    this.left = left;
    this.right = right;
  }

  /**
   * Constructs a DifferentialDriveWheelVelocities.
   *
   * @param left The left velocity in meters per second.
   * @param right The right velocity in meters per second.
   */
  public DifferentialDriveWheelVelocities(LinearVelocity left, LinearVelocity right) {
    this(left.in(MetersPerSecond), right.in(MetersPerSecond));
  }

  /**
   * Renormalizes the wheel velocities if any either side is above the specified maximum.
   *
   * <p>Sometimes, after inverse kinematics, the requested velocity from one or more wheels may be
   * above the max attainable velocity for the driving motor on that wheel. To fix this issue, one
   * can reduce all the wheel velocities to make sure that all requested module velocities are
   * at-or-below the absolute threshold, while maintaining the ratio of velocities between wheels.
   *
   * @param attainableMaxVelocity The absolute max velocity in meters per second that a wheel can
   *     reach.
   */
  public void desaturate(double attainableMaxVelocity) {
    double realMaxVelocity = Math.max(Math.abs(left), Math.abs(right));

    if (realMaxVelocity > attainableMaxVelocity) {
      left = left / realMaxVelocity * attainableMaxVelocity;
      right = right / realMaxVelocity * attainableMaxVelocity;
    }
  }

  /**
   * Renormalizes the wheel velocities if any either side is above the specified maximum.
   *
   * <p>Sometimes, after inverse kinematics, the requested velocity from one or more wheels may be
   * above the max attainable velocity for the driving motor on that wheel. To fix this issue, one
   * can reduce all the wheel velocities to make sure that all requested module velocities are
   * at-or-below the absolute threshold, while maintaining the ratio of velocities between wheels.
   *
   * @param attainableMaxVelocity The absolute max velocity in meters per second that a wheel can
   *     reach.
   */
  public void desaturate(LinearVelocity attainableMaxVelocity) {
    desaturate(attainableMaxVelocity.in(MetersPerSecond));
  }

  /**
   * Adds two DifferentialDriveWheelVelocities and returns the sum.
   *
   * <p>For example, DifferentialDriveWheelVelocities{1.0, 0.5} +
   * DifferentialDriveWheelVelocities{2.0, 1.5} = DifferentialDriveWheelVelocities{3.0, 2.0}
   *
   * @param other The DifferentialDriveWheelVelocities to add.
   * @return The sum of the DifferentialDriveWheelVelocities.
   */
  public DifferentialDriveWheelVelocities plus(DifferentialDriveWheelVelocities other) {
    return new DifferentialDriveWheelVelocities(left + other.left, right + other.right);
  }

  /**
   * Subtracts the other DifferentialDriveWheelVelocities from the current
   * DifferentialDriveWheelVelocities and returns the difference.
   *
   * <p>For example, DifferentialDriveWheelVelocities{5.0, 4.0} -
   * DifferentialDriveWheelVelocities{1.0, 2.0} = DifferentialDriveWheelVelocities{4.0, 2.0}
   *
   * @param other The DifferentialDriveWheelVelocities to subtract.
   * @return The difference between the two DifferentialDriveWheelVelocities.
   */
  public DifferentialDriveWheelVelocities minus(DifferentialDriveWheelVelocities other) {
    return new DifferentialDriveWheelVelocities(left - other.left, right - other.right);
  }

  /**
   * Returns the inverse of the current DifferentialDriveWheelVelocities. This is equivalent to
   * negating all components of the DifferentialDriveWheelVelocities.
   *
   * @return The inverse of the current DifferentialDriveWheelVelocities.
   */
  public DifferentialDriveWheelVelocities unaryMinus() {
    return new DifferentialDriveWheelVelocities(-left, -right);
  }

  /**
   * Multiplies the DifferentialDriveWheelVelocities by a scalar and returns the new
   * DifferentialDriveWheelVelocities.
   *
   * <p>For example, DifferentialDriveWheelVelocities{2.0, 2.5} * 2 =
   * DifferentialDriveWheelVelocities{4.0, 5.0}
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled DifferentialDriveWheelVelocities.
   */
  public DifferentialDriveWheelVelocities times(double scalar) {
    return new DifferentialDriveWheelVelocities(left * scalar, right * scalar);
  }

  /**
   * Divides the DifferentialDriveWheelVelocities by a scalar and returns the new
   * DifferentialDriveWheelVelocities.
   *
   * <p>For example, DifferentialDriveWheelVelocities{2.0, 2.5} / 2 =
   * DifferentialDriveWheelVelocities{1.0, 1.25}
   *
   * @param scalar The scalar to divide by.
   * @return The scaled DifferentialDriveWheelVelocities.
   */
  public DifferentialDriveWheelVelocities div(double scalar) {
    return new DifferentialDriveWheelVelocities(left / scalar, right / scalar);
  }

  /**
   * Returns the linear interpolation of this DifferentialDriveWheelVelocities and another.
   *
   * @param endValue The end value for the interpolation.
   * @param t How far between the two values to interpolate. This is clamped to [0, 1].
   * @return The interpolated value.
   */
  @Override
  public DifferentialDriveWheelVelocities interpolate(
      DifferentialDriveWheelVelocities endValue, double t) {
    // Clamp t to [0, 1]
    t = Math.max(0.0, Math.min(1.0, t));

    return new DifferentialDriveWheelVelocities(
        left + t * (endValue.left - left), right + t * (endValue.right - right));
  }

  @Override
  public String toString() {
    return String.format(
        "DifferentialDriveWheelVelocities(Left: %.2f m/s, Right: %.2f m/s)", left, right);
  }
}
