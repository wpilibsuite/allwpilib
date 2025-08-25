// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static edu.wpi.first.units.Units.MetersPerSecondPerSecond;
import static edu.wpi.first.units.Units.RadiansPerSecondPerSecond;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.interpolation.Interpolatable;
import edu.wpi.first.math.kinematics.struct.ChassisAccelerationsStruct;
import edu.wpi.first.units.measure.AngularAcceleration;
import edu.wpi.first.units.measure.LinearAcceleration;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Objects;

/**
 * Represents the acceleration of a robot chassis. Although this class contains similar members
 * compared to a Twist2d, they do NOT represent the same thing. Whereas a Twist2d represents a
 * change in pose w.r.t to the robot frame of reference, a ChassisAccelerations object represents a
 * robot's velocity.
 *
 * <p>A strictly non-holonomic drivetrain, such as a differential drive, should never have an ay
 * component because it can never move sideways. Holonomic drivetrains such as swerve and mecanum
 * will often have all three components.
 */
public class ChassisAccelerations
    implements StructSerializable, Interpolatable<ChassisAccelerations> {
  /** Velocity along the x-axis in meters per second². (Fwd is +) */
  public double ax;

  /** Velocity along the y-axis in meters per second². (Left is +) */
  public double ay;

  /** Angular velocity of the robot frame in radians per second². (CCW is +) */
  public double alpha;

  /** ChassisAccelerations struct for serialization. */
  public static final ChassisAccelerationsStruct struct = new ChassisAccelerationsStruct();

  /** Constructs a ChassisAccelerations with zeros for ax, ay, and omega. */
  public ChassisAccelerations() {}

  /**
   * Constructs a ChassisAccelerations object.
   *
   * @param ax Forward acceleration in meters per second².
   * @param ay Sideways acceleration in meters per second².
   * @param alpha Angular acceleration in radians per second.
   */
  public ChassisAccelerations(double ax, double ay, double alpha) {
    this.ax = ax;
    this.ay = ay;
    this.alpha = alpha;
  }

  /**
   * Constructs a ChassisAccelerations object.
   *
   * @param ax Forward velocity.
   * @param ay Sideways velocity.
   * @param alpha Angular velocity.
   */
  public ChassisAccelerations(
      LinearAcceleration ax, LinearAcceleration ay, AngularAcceleration alpha) {
    this(
        ax.in(MetersPerSecondPerSecond),
        ay.in(MetersPerSecondPerSecond),
        alpha.in(RadiansPerSecondPerSecond));
  }

  /**
   * Creates a Twist2d from ChassisAccelerations.
   *
   * @param dt The duration of the timestep in seconds.
   * @return Twist2d.
   */
  public Twist2d toTwist2d(double dt) {
    return new Twist2d(ax * dt, ay * dt, alpha * dt);
  }

  /**
   * Discretizes a continuous-time chassis speed.
   *
   * <p>This function converts this continuous-time chassis speed into a discrete-time one such that
   * when the discrete-time chassis speed is applied for one timestep, the robot moves as if the
   * velocity components are independent (i.e., the robot moves v_x * dt along the x-axis, v_y * dt
   * along the y-axis, and alpha * dt around the z-axis).
   *
   * <p>This is useful for compensating for translational skew when translating and rotating a
   * holonomic (swerve or mecanum) drivetrain. However, scaling down the ChassisAccelerations after
   * discretizing (e.g., when desaturating swerve module speeds) rotates the direction of net motion
   * in the opposite direction of rotational velocity, introducing a different translational skew
   * which is not accounted for by discretization.
   *
   * @param dt The duration of the timestep in seconds the speeds should be applied for.
   * @return Discretized ChassisAccelerations.
   */
  public ChassisAccelerations discretize(double dt) {
    // Construct the desired pose after a timestep, relative to the current pose. The desired pose
    // has decoupled translation and rotation.
    var desiredDeltaPose = new Pose2d(ax * dt, ay * dt, new Rotation2d(alpha * dt));

    // Find the chassis translation/rotation deltas in the robot frame that move the robot from its
    // current pose to the desired pose
    var twist = Pose2d.kZero.log(desiredDeltaPose);

    // Turn the chassis translation/rotation deltas into average velocities
    return new ChassisAccelerations(twist.dx / dt, twist.dy / dt, twist.dtheta / dt);
  }

  /**
   * Converts this field-relative set of speeds into a robot-relative ChassisAccelerations object.
   *
   * @param robotAngle The angle of the robot as measured by a gyroscope. The robot's angle is
   *     considered to be zero when it is facing directly away from your alliance station wall.
   *     Remember that this should be CCW positive.
   * @return ChassisAccelerations object representing the speeds in the robot's frame of reference.
   */
  public ChassisAccelerations toRobotRelative(Rotation2d robotAngle) {
    // CW rotation into chassis frame
    var rotated = new Translation2d(ax, ay).rotateBy(robotAngle.unaryMinus());
    return new ChassisAccelerations(rotated.getX(), rotated.getY(), alpha);
  }

  /**
   * Converts this robot-relative set of speeds into a field-relative ChassisAccelerations object.
   *
   * @param robotAngle The angle of the robot as measured by a gyroscope. The robot's angle is
   *     considered to be zero when it is facing directly away from your alliance station wall.
   *     Remember that this should be CCW positive.
   * @return ChassisAccelerations object representing the speeds in the field's frame of reference.
   */
  public ChassisAccelerations toFieldRelative(Rotation2d robotAngle) {
    // CCW rotation out of chassis frame
    var rotated = new Translation2d(ax, ay).rotateBy(robotAngle);
    return new ChassisAccelerations(rotated.getX(), rotated.getY(), alpha);
  }

  /**
   * Adds two ChassisAccelerations and returns the sum.
   *
   * <p>For example, ChassisAccelerations{1.0, 0.5, 0.75} + ChassisAccelerations{2.0, 1.5, 0.25} =
   * ChassisAccelerations{3.0, 2.0, 1.0}
   *
   * @param other The ChassisAccelerations to add.
   * @return The sum of the ChassisAccelerations.
   */
  public ChassisAccelerations plus(ChassisAccelerations other) {
    return new ChassisAccelerations(ax + other.ax, ay + other.ay, alpha + other.alpha);
  }

  /**
   * Subtracts the other ChassisAccelerations from the current ChassisAccelerations and returns the
   * difference.
   *
   * <p>For example, ChassisAccelerations{5.0, 4.0, 2.0} - ChassisAccelerations{1.0, 2.0, 1.0} =
   * ChassisAccelerations{4.0, 2.0, 1.0}
   *
   * @param other The ChassisAccelerations to subtract.
   * @return The difference between the two ChassisAccelerations.
   */
  public ChassisAccelerations minus(ChassisAccelerations other) {
    return new ChassisAccelerations(ax - other.ax, ay - other.ay, alpha - other.alpha);
  }

  /**
   * Returns the inverse of the current ChassisAccelerations. This is equivalent to negating all
   * components of the ChassisAccelerations.
   *
   * @return The inverse of the current ChassisAccelerations.
   */
  public ChassisAccelerations unaryMinus() {
    return new ChassisAccelerations(-ax, -ay, -alpha);
  }

  /**
   * Multiplies the ChassisAccelerations by a scalar and returns the new ChassisAccelerations.
   *
   * <p>For example, ChassisAccelerations{2.0, 2.5, 1.0} * 2 = ChassisAccelerations{4.0, 5.0, 1.0}
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled ChassisAccelerations.
   */
  public ChassisAccelerations times(double scalar) {
    return new ChassisAccelerations(ax * scalar, ay * scalar, alpha * scalar);
  }

  /**
   * Divides the ChassisAccelerations by a scalar and returns the new ChassisAccelerations.
   *
   * <p>For example, ChassisAccelerations{2.0, 2.5, 1.0} / 2 = ChassisAccelerations{1.0, 1.25, 0.5}
   *
   * @param scalar The scalar to divide by.
   * @return The scaled ChassisAccelerations.
   */
  public ChassisAccelerations div(double scalar) {
    return new ChassisAccelerations(ax / scalar, ay / scalar, alpha / scalar);
  }

  @Override
  public ChassisAccelerations interpolate(ChassisAccelerations endValue, double t) {
    if (t <= 0) {
      return this;
    } else if (t >= 1) {
      return endValue;
    } else {
      return new ChassisAccelerations(
          MathUtil.interpolate(this.ax, endValue.ax, t),
          MathUtil.interpolate(this.ay, endValue.ay, t),
          MathUtil.interpolate(this.alpha, endValue.alpha, t));
    }
  }

  @Override
  public final int hashCode() {
    return Objects.hash(ax, ay, alpha);
  }

  @Override
  public boolean equals(Object o) {
    return o == this
        || o instanceof ChassisAccelerations c && ax == c.ax && ay == c.ay && alpha == c.alpha;
  }

  @Override
  public String toString() {
    return String.format(
        "ChassisAccelerations(Vx: %.2f m/s, Vy: %.2f m/s, Omega: %.2f rad/s)", ax, ay, alpha);
  }
}
