// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static edu.wpi.first.units.Units.MetersPerSecond;

import edu.wpi.first.math.kinematics.proto.DifferentialDriveWheelSpeedsProto;
import edu.wpi.first.math.kinematics.struct.DifferentialDriveWheelSpeedsStruct;
import edu.wpi.first.units.measure.LinearVelocity;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;

/** Represents the wheel speeds for a differential drive drivetrain. */
public class DifferentialDriveWheelSpeeds implements ProtobufSerializable, StructSerializable {
  /** Speed of the left side of the robot. */
  public double leftMetersPerSecond;

  /** Speed of the right side of the robot. */
  public double rightMetersPerSecond;

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
   * @param leftMetersPerSecond The left speed.
   * @param rightMetersPerSecond The right speed.
   */
  public DifferentialDriveWheelSpeeds(double leftMetersPerSecond, double rightMetersPerSecond) {
    this.leftMetersPerSecond = leftMetersPerSecond;
    this.rightMetersPerSecond = rightMetersPerSecond;
  }

  /**
   * Constructs a DifferentialDriveWheelSpeeds.
   *
   * @param left The left speed.
   * @param right The right speed.
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
   * @param attainableMaxSpeedMetersPerSecond The absolute max speed that a wheel can reach.
   */
  public void desaturate(double attainableMaxSpeedMetersPerSecond) {
    double realMaxSpeed = Math.max(Math.abs(leftMetersPerSecond), Math.abs(rightMetersPerSecond));

    if (realMaxSpeed > attainableMaxSpeedMetersPerSecond) {
      leftMetersPerSecond = leftMetersPerSecond / realMaxSpeed * attainableMaxSpeedMetersPerSecond;
      rightMetersPerSecond =
          rightMetersPerSecond / realMaxSpeed * attainableMaxSpeedMetersPerSecond;
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
   * @param attainableMaxSpeed The absolute max speed that a wheel can reach.
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
    return new DifferentialDriveWheelSpeeds(
        leftMetersPerSecond + other.leftMetersPerSecond,
        rightMetersPerSecond + other.rightMetersPerSecond);
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
    return new DifferentialDriveWheelSpeeds(
        leftMetersPerSecond - other.leftMetersPerSecond,
        rightMetersPerSecond - other.rightMetersPerSecond);
  }

  /**
   * Returns the inverse of the current DifferentialDriveWheelSpeeds. This is equivalent to negating
   * all components of the DifferentialDriveWheelSpeeds.
   *
   * @return The inverse of the current DifferentialDriveWheelSpeeds.
   */
  public DifferentialDriveWheelSpeeds unaryMinus() {
    return new DifferentialDriveWheelSpeeds(-leftMetersPerSecond, -rightMetersPerSecond);
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
    return new DifferentialDriveWheelSpeeds(
        leftMetersPerSecond * scalar, rightMetersPerSecond * scalar);
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
    return new DifferentialDriveWheelSpeeds(
        leftMetersPerSecond / scalar, rightMetersPerSecond / scalar);
  }

  @Override
  public String toString() {
    return String.format(
        "DifferentialDriveWheelSpeeds(Left: %.2f m/s, Right: %.2f m/s)",
        leftMetersPerSecond, rightMetersPerSecond);
  }
}
