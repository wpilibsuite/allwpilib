// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.wpilib.units.Units.MetersPerSecond;
import static org.wpilib.units.Units.RadiansPerSecond;

import java.util.Objects;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Transform2d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.geometry.Twist2d;
import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.kinematics.proto.ChassisVelocitiesProto;
import org.wpilib.math.kinematics.struct.ChassisVelocitiesStruct;
import org.wpilib.math.util.MathUtil;
import org.wpilib.units.measure.AngularVelocity;
import org.wpilib.units.measure.LinearVelocity;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/**
 * Represents robot chassis velocities.
 *
 * <p>Although this class contains similar members compared to a Twist2d, they do NOT represent the
 * same thing. Whereas a Twist2d represents a change in pose w.r.t to the robot frame of reference,
 * a ChassisVelocities object represents a robot's velocities.
 *
 * <p>A strictly non-holonomic drivetrain, such as a differential drive, should never have a dy
 * component because it can never move sideways. Holonomic drivetrains such as swerve and mecanum
 * will often have all three components.
 */
public class ChassisVelocities
    implements ProtobufSerializable, StructSerializable, Interpolatable<ChassisVelocities> {
  /** Velocity along the x-axis in meters per second. (Fwd is +) */
  public double vx;

  /** Velocity along the y-axis in meters per second. (Left is +) */
  public double vy;

  /** Angular velocity of the robot frame in radians per second. (CCW is +) */
  public double omega;

  /** ChassisVelocities protobuf for serialization. */
  public static final ChassisVelocitiesProto proto = new ChassisVelocitiesProto();

  /** ChassisVelocities struct for serialization. */
  public static final ChassisVelocitiesStruct struct = new ChassisVelocitiesStruct();

  /** Constructs a ChassisVelocities with zeros for dx, dy, and theta. */
  public ChassisVelocities() {}

  /**
   * Constructs a ChassisVelocities object.
   *
   * @param vx Forward velocity in meters per second.
   * @param vy Sideways velocity in meters per second.
   * @param omega Angular velocity in radians per second.
   */
  public ChassisVelocities(double vx, double vy, double omega) {
    this.vx = vx;
    this.vy = vy;
    this.omega = omega;
  }

  /**
   * Constructs a ChassisVelocities object.
   *
   * @param vx Forward velocity.
   * @param vy Sideways velocity.
   * @param omega Angular velocity.
   */
  public ChassisVelocities(LinearVelocity vx, LinearVelocity vy, AngularVelocity omega) {
    this(vx.in(MetersPerSecond), vy.in(MetersPerSecond), omega.in(RadiansPerSecond));
  }

  /**
   * Creates a Twist2d from ChassisVelocities.
   *
   * @param dt The duration of the timestep in seconds.
   * @return Twist2d.
   */
  public Twist2d toTwist2d(double dt) {
    return new Twist2d(vx * dt, vy * dt, omega * dt);
  }

  /**
   * Discretizes continuous-time chassis velocities.
   *
   * <p>This function converts these continuous-time chassis velocities into discrete-time ones such
   * that when the discrete-time chassis velocities are applied for one timestep, the robot moves as
   * if the velocity components are independent (i.e., the robot moves v_x * dt along the x-axis,
   * v_y * dt along the y-axis, and omega * dt around the z-axis).
   *
   * <p>This is useful for compensating for translational skew when translating and rotating a
   * holonomic (swerve or mecanum) drivetrain. However, scaling down the ChassisVelocities after
   * discretizing (e.g., when desaturating swerve module velocities) rotates the direction of net
   * motion in the opposite direction of rotational velocity, introducing a different translational
   * skew which is not accounted for by discretization.
   *
   * @param dt The duration of the timestep in seconds the velocities should be applied for.
   * @return Discretized ChassisVelocities.
   */
  public ChassisVelocities discretize(double dt) {
    // Construct the desired pose after a timestep, relative to the current pose. The desired pose
    // has decoupled translation and rotation.
    var desiredTransform = new Transform2d(vx * dt, vy * dt, new Rotation2d(omega * dt));

    // Find the chassis translation/rotation deltas in the robot frame that move the robot from its
    // current pose to the desired pose
    var twist = desiredTransform.log();

    // Turn the chassis translation/rotation deltas into average velocities
    return new ChassisVelocities(twist.dx / dt, twist.dy / dt, twist.dtheta / dt);
  }

  /**
   * Converts this field-relative set of velocities into a robot-relative ChassisVelocities object.
   *
   * @param robotAngle The angle of the robot as measured by a gyroscope. The robot's angle is
   *     considered to be zero when it is facing directly away from your alliance station wall.
   *     Remember that this should be CCW positive.
   * @return ChassisVelocities object representing the velocities in the robot's frame of reference.
   */
  public ChassisVelocities toRobotRelative(Rotation2d robotAngle) {
    // CW rotation into chassis frame
    var rotated = new Translation2d(vx, vy).rotateBy(robotAngle.unaryMinus());
    return new ChassisVelocities(rotated.getX(), rotated.getY(), omega);
  }

  /**
   * Converts this robot-relative set of velocities into a field-relative ChassisVelocities object.
   *
   * @param robotAngle The angle of the robot as measured by a gyroscope. The robot's angle is
   *     considered to be zero when it is facing directly away from your alliance station wall.
   *     Remember that this should be CCW positive.
   * @return ChassisVelocities object representing the velocities in the field's frame of reference.
   */
  public ChassisVelocities toFieldRelative(Rotation2d robotAngle) {
    // CCW rotation out of chassis frame
    var rotated = new Translation2d(vx, vy).rotateBy(robotAngle);
    return new ChassisVelocities(rotated.getX(), rotated.getY(), omega);
  }

  /**
   * Adds two ChassisVelocities and returns the sum.
   *
   * <p>For example, ChassisVelocities{1.0, 0.5, 0.75} + ChassisVelocities{2.0, 1.5, 0.25} =
   * ChassisVelocities{3.0, 2.0, 1.0}
   *
   * @param other The ChassisVelocities to add.
   * @return The sum of the ChassisVelocities.
   */
  public ChassisVelocities plus(ChassisVelocities other) {
    return new ChassisVelocities(vx + other.vx, vy + other.vy, omega + other.omega);
  }

  /**
   * Subtracts the other ChassisVelocities from the current ChassisVelocities and returns the
   * difference.
   *
   * <p>For example, ChassisVelocities{5.0, 4.0, 2.0} - ChassisVelocities{1.0, 2.0, 1.0} =
   * ChassisVelocities{4.0, 2.0, 1.0}
   *
   * @param other The ChassisVelocities to subtract.
   * @return The difference between the two ChassisVelocities.
   */
  public ChassisVelocities minus(ChassisVelocities other) {
    return new ChassisVelocities(vx - other.vx, vy - other.vy, omega - other.omega);
  }

  /**
   * Returns the inverse of the current ChassisVelocities. This is equivalent to negating all
   * components of the ChassisVelocities.
   *
   * @return The inverse of the current ChassisVelocities.
   */
  public ChassisVelocities unaryMinus() {
    return new ChassisVelocities(-vx, -vy, -omega);
  }

  /**
   * Multiplies the ChassisVelocities by a scalar and returns the new ChassisVelocities.
   *
   * <p>For example, ChassisVelocities{2.0, 2.5, 1.0} * 2 = ChassisVelocities{4.0, 5.0, 1.0}
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled ChassisVelocities.
   */
  public ChassisVelocities times(double scalar) {
    return new ChassisVelocities(vx * scalar, vy * scalar, omega * scalar);
  }

  /**
   * Divides the ChassisVelocities by a scalar and returns the new ChassisVelocities.
   *
   * <p>For example, ChassisVelocities{2.0, 2.5, 1.0} / 2 = ChassisVelocities{1.0, 1.25, 0.5}
   *
   * @param scalar The scalar to divide by.
   * @return The scaled ChassisVelocities.
   */
  public ChassisVelocities div(double scalar) {
    return new ChassisVelocities(vx / scalar, vy / scalar, omega / scalar);
  }

  @Override
  public ChassisVelocities interpolate(ChassisVelocities endValue, double t) {
    if (t <= 0) {
      return this;
    } else if (t >= 1) {
      return endValue;
    } else {
      return new ChassisVelocities(
          MathUtil.lerp(this.vx, endValue.vx, t),
          MathUtil.lerp(this.vy, endValue.vy, t),
          MathUtil.lerp(this.omega, endValue.omega, t));
    }
  }

  @Override
  public final int hashCode() {
    return Objects.hash(vx, vy, omega);
  }

  @Override
  public boolean equals(Object o) {
    return o == this
        || o instanceof ChassisVelocities c && vx == c.vx && vy == c.vy && omega == c.omega;
  }

  @Override
  public String toString() {
    return String.format(
        "ChassisVelocities(Vx: %.2f m/s, Vy: %.2f m/s, Omega: %.2f rad/s)", vx, vy, omega);
  }
}
