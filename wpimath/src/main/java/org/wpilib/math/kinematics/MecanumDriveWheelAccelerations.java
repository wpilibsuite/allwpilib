// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.wpilib.units.Units.MetersPerSecondPerSecond;

import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.kinematics.proto.MecanumDriveWheelAccelerationsProto;
import org.wpilib.math.kinematics.struct.MecanumDriveWheelAccelerationsStruct;
import org.wpilib.units.measure.LinearAcceleration;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/** Represents the wheel accelerations for a mecanum drive drivetrain. */
public class MecanumDriveWheelAccelerations
    implements Interpolatable<MecanumDriveWheelAccelerations>,
        ProtobufSerializable,
        StructSerializable {
  /** Acceleration of the front left wheel in meters per second squared. */
  public double frontLeft;

  /** Acceleration of the front right wheel in meters per second squared. */
  public double frontRight;

  /** Acceleration of the rear left wheel in meters per second squared. */
  public double rearLeft;

  /** Acceleration of the rear right wheel in meters per second squared. */
  public double rearRight;

  /** MecanumDriveWheelAccelerations protobuf for serialization. */
  public static final MecanumDriveWheelAccelerationsProto proto =
      new MecanumDriveWheelAccelerationsProto();

  /** MecanumDriveWheelAccelerations struct for serialization. */
  public static final MecanumDriveWheelAccelerationsStruct struct =
      new MecanumDriveWheelAccelerationsStruct();

  /** Constructs a MecanumDriveWheelAccelerations with zeros for all member fields. */
  public MecanumDriveWheelAccelerations() {}

  /**
   * Constructs a MecanumDriveWheelAccelerations.
   *
   * @param frontLeft Acceleration of the front left wheel in meters per second squared.
   * @param frontRight Acceleration of the front right wheel in meters per second squared.
   * @param rearLeft Acceleration of the rear left wheel in meters per second squared.
   * @param rearRight Acceleration of the rear right wheel in meters per second squared.
   */
  public MecanumDriveWheelAccelerations(
      double frontLeft, double frontRight, double rearLeft, double rearRight) {
    this.frontLeft = frontLeft;
    this.frontRight = frontRight;
    this.rearLeft = rearLeft;
    this.rearRight = rearRight;
  }

  /**
   * Constructs a MecanumDriveWheelAccelerations.
   *
   * @param frontLeft Acceleration of the front left wheel in meters per second squared.
   * @param frontRight Acceleration of the front right wheel in meters per second squared.
   * @param rearLeft Acceleration of the rear left wheel in meters per second squared.
   * @param rearRight Acceleration of the rear right wheel in meters per second squared.
   */
  public MecanumDriveWheelAccelerations(
      LinearAcceleration frontLeft,
      LinearAcceleration frontRight,
      LinearAcceleration rearLeft,
      LinearAcceleration rearRight) {
    this(
        frontLeft.in(MetersPerSecondPerSecond),
        frontRight.in(MetersPerSecondPerSecond),
        rearLeft.in(MetersPerSecondPerSecond),
        rearRight.in(MetersPerSecondPerSecond));
  }

  /**
   * Adds two MecanumDriveWheelAccelerations and returns the sum.
   *
   * <p>For example, MecanumDriveWheelAccelerations{1.0, 0.5, 2.0, 1.5} +
   * MecanumDriveWheelAccelerations{2.0, 1.5, 0.5, 1.0} = MecanumDriveWheelAccelerations{3.0, 2.0,
   * 2.5, 2.5}
   *
   * @param other The MecanumDriveWheelAccelerations to add.
   * @return The sum of the MecanumDriveWheelAccelerations.
   */
  public MecanumDriveWheelAccelerations plus(MecanumDriveWheelAccelerations other) {
    return new MecanumDriveWheelAccelerations(
        frontLeft + other.frontLeft,
        frontRight + other.frontRight,
        rearLeft + other.rearLeft,
        rearRight + other.rearRight);
  }

  /**
   * Subtracts the other MecanumDriveWheelAccelerations from the current
   * MecanumDriveWheelAccelerations and returns the difference.
   *
   * <p>For example, MecanumDriveWheelAccelerations{5.0, 4.0, 6.0, 2.5} -
   * MecanumDriveWheelAccelerations{1.0, 2.0, 3.0, 0.5} = MecanumDriveWheelAccelerations{4.0, 2.0,
   * 3.0, 2.0}
   *
   * @param other The MecanumDriveWheelAccelerations to subtract.
   * @return The difference between the two MecanumDriveWheelAccelerations.
   */
  public MecanumDriveWheelAccelerations minus(MecanumDriveWheelAccelerations other) {
    return new MecanumDriveWheelAccelerations(
        frontLeft - other.frontLeft,
        frontRight - other.frontRight,
        rearLeft - other.rearLeft,
        rearRight - other.rearRight);
  }

  /**
   * Returns the inverse of the current MecanumDriveWheelAccelerations. This is equivalent to
   * negating all components of the MecanumDriveWheelAccelerations.
   *
   * @return The inverse of the current MecanumDriveWheelAccelerations.
   */
  public MecanumDriveWheelAccelerations unaryMinus() {
    return new MecanumDriveWheelAccelerations(-frontLeft, -frontRight, -rearLeft, -rearRight);
  }

  /**
   * Multiplies the MecanumDriveWheelAccelerations by a scalar and returns the new
   * MecanumDriveWheelAccelerations.
   *
   * <p>For example, MecanumDriveWheelAccelerations{2.0, 2.5, 3.0, 3.5} * 2 =
   * MecanumDriveWheelAccelerations{4.0, 5.0, 6.0, 7.0}
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled MecanumDriveWheelAccelerations.
   */
  public MecanumDriveWheelAccelerations times(double scalar) {
    return new MecanumDriveWheelAccelerations(
        frontLeft * scalar, frontRight * scalar, rearLeft * scalar, rearRight * scalar);
  }

  /**
   * Divides the MecanumDriveWheelAccelerations by a scalar and returns the new
   * MecanumDriveWheelAccelerations.
   *
   * <p>For example, MecanumDriveWheelAccelerations{2.0, 2.5, 1.5, 1.0} / 2 =
   * MecanumDriveWheelAccelerations{1.0, 1.25, 0.75, 0.5}
   *
   * @param scalar The scalar to divide by.
   * @return The scaled MecanumDriveWheelAccelerations.
   */
  public MecanumDriveWheelAccelerations div(double scalar) {
    return new MecanumDriveWheelAccelerations(
        frontLeft / scalar, frontRight / scalar, rearLeft / scalar, rearRight / scalar);
  }

  /**
   * Returns the linear interpolation of this MecanumDriveWheelAccelerations and another.
   *
   * @param endValue The end value for the interpolation.
   * @param t How far between the two values to interpolate. This is clamped to [0, 1].
   * @return The interpolated value.
   */
  @Override
  public MecanumDriveWheelAccelerations interpolate(
      MecanumDriveWheelAccelerations endValue, double t) {
    // Clamp t to [0, 1]
    t = Math.max(0.0, Math.min(1.0, t));

    return new MecanumDriveWheelAccelerations(
        frontLeft + t * (endValue.frontLeft - frontLeft),
        frontRight + t * (endValue.frontRight - frontRight),
        rearLeft + t * (endValue.rearLeft - rearLeft),
        rearRight + t * (endValue.rearRight - rearRight));
  }

  @Override
  public String toString() {
    return String.format(
        "MecanumDriveWheelAccelerations(Front Left: %.2f m/s², Front Right: %.2f m/s², "
            + "Rear Left: %.2f m/s², Rear Right: %.2f m/s²)",
        frontLeft, frontRight, rearLeft, rearRight);
  }
}
