// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static edu.wpi.first.units.Units.MetersPerSecondPerSecond;
import static edu.wpi.first.units.Units.RadiansPerSecondPerSecond;

import edu.wpi.first.math.interpolation.Interpolatable;
import edu.wpi.first.math.kinematics.proto.SwerveModuleAccelerationsProto;
import edu.wpi.first.math.kinematics.struct.SwerveModuleAccelerationsStruct;
import edu.wpi.first.units.measure.AngularAcceleration;
import edu.wpi.first.units.measure.LinearAcceleration;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Objects;

/** Represents the accelerations of one swerve module. */
public class SwerveModuleAccelerations
    implements Interpolatable<SwerveModuleAccelerations>,
        Comparable<SwerveModuleAccelerations>,
        ProtobufSerializable,
        StructSerializable {
  /** Acceleration of the wheel of the module in meters per second squared. */
  public double acceleration;

  /** Angular acceleration of the module in radians per second squared. */
  public double angularAcceleration;

  /** SwerveModuleAccelerations protobuf for serialization. */
  public static final SwerveModuleAccelerationsProto proto = new SwerveModuleAccelerationsProto();

  /** SwerveModuleAccelerations struct for serialization. */
  public static final SwerveModuleAccelerationsStruct struct =
      new SwerveModuleAccelerationsStruct();

  /**
   * Constructs a SwerveModuleAccelerations with zeros for acceleration and angular acceleration.
   */
  public SwerveModuleAccelerations() {}

  /**
   * Constructs a SwerveModuleAccelerations.
   *
   * @param acceleration The acceleration of the wheel of the module in meters per second squared.
   * @param angularAcceleration The angular acceleration of the module in radians per second
   *     squared.
   */
  public SwerveModuleAccelerations(double acceleration, double angularAcceleration) {
    this.acceleration = acceleration;
    this.angularAcceleration = angularAcceleration;
  }

  /**
   * Constructs a SwerveModuleAccelerations.
   *
   * @param acceleration The acceleration of the wheel of the module.
   * @param angularAcceleration The angular acceleration of the module.
   */
  public SwerveModuleAccelerations(
      LinearAcceleration acceleration, AngularAcceleration angularAcceleration) {
    this(
        acceleration.in(MetersPerSecondPerSecond),
        angularAcceleration.in(RadiansPerSecondPerSecond));
  }

  /**
   * Adds two SwerveModuleAccelerations and returns the sum.
   *
   * @param other The SwerveModuleAccelerations to add.
   * @return The sum of the SwerveModuleAccelerations.
   */
  public SwerveModuleAccelerations plus(SwerveModuleAccelerations other) {
    return new SwerveModuleAccelerations(
        acceleration + other.acceleration, angularAcceleration + other.angularAcceleration);
  }

  /**
   * Subtracts the other SwerveModuleAccelerations from the current SwerveModuleAccelerations and
   * returns the difference.
   *
   * @param other The SwerveModuleAccelerations to subtract.
   * @return The difference between the two SwerveModuleAccelerations.
   */
  public SwerveModuleAccelerations minus(SwerveModuleAccelerations other) {
    return new SwerveModuleAccelerations(
        acceleration - other.acceleration, angularAcceleration - other.angularAcceleration);
  }

  /**
   * Returns the inverse of the current SwerveModuleAccelerations. This is equivalent to negating
   * all components of the SwerveModuleAccelerations.
   *
   * @return The inverse of the current SwerveModuleAccelerations.
   */
  public SwerveModuleAccelerations unaryMinus() {
    return new SwerveModuleAccelerations(-acceleration, -angularAcceleration);
  }

  /**
   * Multiplies the SwerveModuleAccelerations by a scalar and returns the new
   * SwerveModuleAccelerations.
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled SwerveModuleAccelerations.
   */
  public SwerveModuleAccelerations times(double scalar) {
    return new SwerveModuleAccelerations(acceleration * scalar, angularAcceleration * scalar);
  }

  /**
   * Divides the SwerveModuleAccelerations by a scalar and returns the new
   * SwerveModuleAccelerations.
   *
   * @param scalar The scalar to divide by.
   * @return The scaled SwerveModuleAccelerations.
   */
  public SwerveModuleAccelerations div(double scalar) {
    return new SwerveModuleAccelerations(acceleration / scalar, angularAcceleration / scalar);
  }

  /**
   * Returns the linear interpolation of this SwerveModuleAccelerations and another.
   *
   * @param endValue The end value for the interpolation.
   * @param t How far between the two values to interpolate. This is clamped to [0, 1].
   * @return The interpolated value.
   */
  @Override
  public SwerveModuleAccelerations interpolate(SwerveModuleAccelerations endValue, double t) {
    // Clamp t to [0, 1]
    t = Math.max(0.0, Math.min(1.0, t));

    return new SwerveModuleAccelerations(
        acceleration + t * (endValue.acceleration - acceleration),
        angularAcceleration + t * (endValue.angularAcceleration - angularAcceleration));
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof SwerveModuleAccelerations other
        && Math.abs(other.acceleration - acceleration) < 1E-9
        && Math.abs(other.angularAcceleration - angularAcceleration) < 1E-9;
  }

  @Override
  public int hashCode() {
    return Objects.hash(acceleration, angularAcceleration);
  }

  /**
   * Compares two swerve module accelerations. One swerve module is "greater" than the other if its
   * acceleration is higher than the other.
   *
   * @param other The other swerve module.
   * @return 1 if this is greater, 0 if both are equal, -1 if other is greater.
   */
  @Override
  public int compareTo(SwerveModuleAccelerations other) {
    return Double.compare(this.acceleration, other.acceleration);
  }

  @Override
  public String toString() {
    return String.format(
        "SwerveModuleAccelerations(Acceleration: %.2f m/s², Angular Acceleration: %.2f rad/s²)",
        acceleration, angularAcceleration);
  }
}
