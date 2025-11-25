// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.wpilib.units.Units.MetersPerSecondPerSecond;

import java.util.Objects;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.kinematics.proto.SwerveModuleAccelerationProto;
import org.wpilib.math.kinematics.struct.SwerveModuleAccelerationStruct;
import org.wpilib.units.measure.LinearAcceleration;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/** Represents the accelerations of one swerve module. */
public class SwerveModuleAcceleration
    implements Interpolatable<SwerveModuleAcceleration>,
        Comparable<SwerveModuleAcceleration>,
        ProtobufSerializable,
        StructSerializable {
  /** Acceleration of the wheel of the module in meters per second squared. */
  public double acceleration;

  /** Angle of the acceleration vector. */
  public Rotation2d angle = new Rotation2d();

  /** SwerveModuleAccelerations protobuf for serialization. */
  public static final SwerveModuleAccelerationProto proto = new SwerveModuleAccelerationProto();

  /** SwerveModuleAccelerations struct for serialization. */
  public static final SwerveModuleAccelerationStruct struct =
      new SwerveModuleAccelerationStruct();

  /** Constructs a SwerveModuleAccelerations with zeros for acceleration and angle. */
  public SwerveModuleAcceleration() {}

  /**
   * Constructs a SwerveModuleAccelerations.
   *
   * @param acceleration The acceleration of the wheel of the module in meters per second squared.
   * @param angle The angle of the acceleration vector.
   */
  public SwerveModuleAcceleration(double acceleration, Rotation2d angle) {
    this.acceleration = acceleration;
    this.angle = angle;
  }

  /**
   * Constructs a SwerveModuleAccelerations.
   *
   * @param acceleration The acceleration of the wheel of the module.
   * @param angle The angle of the acceleration vector.
   */
  public SwerveModuleAcceleration(LinearAcceleration acceleration, Rotation2d angle) {
    this(acceleration.in(MetersPerSecondPerSecond), angle);
  }

  /**
   * Returns the linear interpolation of this SwerveModuleAccelerations and another.
   *
   * @param endValue The end value for the interpolation.
   * @param t How far between the two values to interpolate. This is clamped to [0, 1].
   * @return The interpolated value.
   */
  @Override
  public SwerveModuleAcceleration interpolate(SwerveModuleAcceleration endValue, double t) {
    // Clamp t to [0, 1]
    t = Math.max(0.0, Math.min(1.0, t));

    return new SwerveModuleAcceleration(
        acceleration + t * (endValue.acceleration - acceleration),
        angle.interpolate(endValue.angle, t));
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof SwerveModuleAcceleration other
        && Math.abs(other.acceleration - acceleration) < 1E-9
        && angle.equals(other.angle);
  }

  @Override
  public int hashCode() {
    return Objects.hash(acceleration, angle);
  }

  /**
   * Compares two swerve module accelerations. One swerve module is "greater" than the other if its
   * acceleration magnitude is higher than the other.
   *
   * @param other The other swerve module.
   * @return 1 if this is greater, 0 if both are equal, -1 if other is greater.
   */
  @Override
  public int compareTo(SwerveModuleAcceleration other) {
    return Double.compare(this.acceleration, other.acceleration);
  }

  @Override
  public String toString() {
    return String.format(
        "SwerveModuleAccelerations(Acceleration: %.2f m/s², Angle: %s)", acceleration, angle);
  }
}
