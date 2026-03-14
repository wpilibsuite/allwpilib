// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.wpilib.units.Units.MetersPerSecond;

import java.util.Objects;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.kinematics.proto.SwerveModuleVelocityProto;
import org.wpilib.math.kinematics.struct.SwerveModuleVelocityStruct;
import org.wpilib.units.measure.LinearVelocity;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/** Represents the velocity of one swerve module. */
public class SwerveModuleVelocity
    implements Interpolatable<SwerveModuleVelocity>,
        Comparable<SwerveModuleVelocity>,
        ProtobufSerializable,
        StructSerializable {
  /** Velocity of the wheel of the module in meters per second. */
  public double velocity;

  /** Angle of the module. */
  public Rotation2d angle = Rotation2d.kZero;

  /** SwerveModuleVelocity protobuf for serialization. */
  public static final SwerveModuleVelocityProto proto = new SwerveModuleVelocityProto();

  /** SwerveModuleVelocity struct for serialization. */
  public static final SwerveModuleVelocityStruct struct = new SwerveModuleVelocityStruct();

  /** Constructs a SwerveModuleVelocity with zeros for velocity and angle. */
  public SwerveModuleVelocity() {}

  /**
   * Constructs a SwerveModuleVelocity.
   *
   * @param velocity The velocity of the wheel of the module in meters per second.
   * @param angle The angle of the module.
   */
  public SwerveModuleVelocity(double velocity, Rotation2d angle) {
    this.velocity = velocity;
    this.angle = angle;
  }

  /**
   * Constructs a SwerveModuleVelocity.
   *
   * @param velocity The velocity of the wheel of the module.
   * @param angle The angle of the module.
   */
  public SwerveModuleVelocity(LinearVelocity velocity, Rotation2d angle) {
    this(velocity.in(MetersPerSecond), angle);
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof SwerveModuleVelocity other
        && Math.abs(other.velocity - velocity) < 1E-9
        && angle.equals(other.angle);
  }

  @Override
  public int hashCode() {
    return Objects.hash(velocity, angle);
  }

  /**
   * Compares two swerve module velocities. One swerve module is "greater" than the other if its
   * velocity is higher than the other.
   *
   * @param other The other swerve module.
   * @return 1 if this is greater, 0 if both are equal, -1 if other is greater.
   */
  @Override
  public int compareTo(SwerveModuleVelocity other) {
    return Double.compare(this.velocity, other.velocity);
  }

  @Override
  public String toString() {
    return String.format("SwerveModuleVelocity(Velocity: %.2f m/s, Angle: %s)", velocity, angle);
  }

  /**
   * Minimize the change in heading this swerve module velocity would require by potentially
   * reversing the direction the wheel spins. If this is used with the PIDController class's
   * continuous input functionality, the furthest a wheel will ever rotate is 90 degrees.
   *
   * @param currentAngle The current module angle.
   */
  public void optimize(Rotation2d currentAngle) {
    var delta = angle.minus(currentAngle);
    if (Math.abs(delta.getDegrees()) > 90.0) {
      velocity *= -1;
      angle = angle.rotateBy(Rotation2d.kPi);
    }
  }

  /**
   * Minimize the change in heading the desired swerve module velocity would require by potentially
   * reversing the direction the wheel spins. If this is used with the PIDController class's
   * continuous input functionality, the furthest a wheel will ever rotate is 90 degrees.
   *
   * @param desiredVelocity The desired velocity.
   * @param currentAngle The current module angle.
   * @return Optimized swerve module velocity.
   * @deprecated Use the instance method instead.
   */
  @Deprecated
  public static SwerveModuleVelocity optimize(
      SwerveModuleVelocity desiredVelocity, Rotation2d currentAngle) {
    var delta = desiredVelocity.angle.minus(currentAngle);
    if (Math.abs(delta.getDegrees()) > 90.0) {
      return new SwerveModuleVelocity(
          -desiredVelocity.velocity, desiredVelocity.angle.rotateBy(Rotation2d.kPi));
    } else {
      return new SwerveModuleVelocity(desiredVelocity.velocity, desiredVelocity.angle);
    }
  }

  /**
   * Returns the linear interpolation of this SwerveModuleVelocity and another. The angle is
   * interpolated using the shortest path between the two angles.
   *
   * @param endValue The end value for the interpolation.
   * @param t How far between the two values to interpolate. This is clamped to [0, 1].
   * @return The interpolated value.
   */
  @Override
  public SwerveModuleVelocity interpolate(SwerveModuleVelocity endValue, double t) {
    // Clamp t to [0, 1]
    t = Math.max(0.0, Math.min(1.0, t));

    // Interpolate velocity linearly
    double interpolatedVelocity = velocity + t * (endValue.velocity - velocity);

    // Interpolate angle using the shortest path
    Rotation2d interpolatedAngle = angle.interpolate(endValue.angle, t);

    return new SwerveModuleVelocity(interpolatedVelocity, interpolatedAngle);
  }

  /**
   * Scales velocity by cosine of angle error. This scales down movement perpendicular to the
   * desired direction of travel that can occur when modules change directions. This results in
   * smoother driving.
   *
   * @param currentAngle The current module angle.
   */
  public void cosineScale(Rotation2d currentAngle) {
    velocity *= angle.minus(currentAngle).getCos();
  }
}
