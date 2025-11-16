// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.wpilib.units.Units.MetersPerSecondPerSecond;
import static org.wpilib.units.Units.RadiansPerSecondPerSecond;

import java.util.Objects;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.interpolation.Interpolatable;
import org.wpilib.math.kinematics.proto.ChassisAccelerationsProto;
import org.wpilib.math.kinematics.struct.ChassisAccelerationsStruct;
import org.wpilib.math.util.MathUtil;
import org.wpilib.units.measure.AngularAcceleration;
import org.wpilib.units.measure.LinearAcceleration;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/**
 * Represents the acceleration of a robot chassis.
 *
 * <p>A strictly non-holonomic drivetrain, such as a differential drive, should never have an ay
 * component because it can never move sideways. Holonomic drivetrains such as swerve and mecanum
 * will often have all three components.
 */
public class ChassisAccelerations
    implements ProtobufSerializable, StructSerializable, Interpolatable<ChassisAccelerations> {
  /** Acceleration along the x-axis in meters per second squared. (Fwd is +) */
  public double ax;

  /** Acceleration along the y-axis in meters per second squared. (Left is +) */
  public double ay;

  /** Angular acceleration of the robot frame in radians per second squared. (CCW is +) */
  public double alpha;

  /** ChassisAccelerations struct for serialization. */
  public static final ChassisAccelerationsStruct struct = new ChassisAccelerationsStruct();

  /** ChassisAccelerations proto for serialization. */
  public static final ChassisAccelerationsProto proto = new ChassisAccelerationsProto();

  /** Constructs a ChassisAccelerations with zeros for ax, ay, and omega. */
  public ChassisAccelerations() {}

  /**
   * Constructs a ChassisAccelerations object.
   *
   * @param ax Forward acceleration in meters per second squared.
   * @param ay Sideways acceleration in meters per second squared.
   * @param alpha Angular acceleration in radians per second squared.
   */
  public ChassisAccelerations(double ax, double ay, double alpha) {
    this.ax = ax;
    this.ay = ay;
    this.alpha = alpha;
  }

  /**
   * Constructs a ChassisAccelerations object.
   *
   * @param ax Forward acceleration.
   * @param ay Sideways acceleration.
   * @param alpha Angular acceleration.
   */
  public ChassisAccelerations(
      LinearAcceleration ax, LinearAcceleration ay, AngularAcceleration alpha) {
    this(
        ax.in(MetersPerSecondPerSecond),
        ay.in(MetersPerSecondPerSecond),
        alpha.in(RadiansPerSecondPerSecond));
  }

  /**
   * Converts this field-relative set of accelerations into a robot-relative ChassisAccelerations
   * object.
   *
   * @param robotAngle The angle of the robot as measured by a gyroscope. The robot's angle is
   *     considered to be zero when it is facing directly away from your alliance station wall.
   *     Remember that this should be CCW positive.
   * @return ChassisAccelerations object representing the accelerations in the robot's frame of
   *     reference.
   */
  public ChassisAccelerations toRobotRelative(Rotation2d robotAngle) {
    // CW rotation into chassis frame
    var rotated = new Translation2d(ax, ay).rotateBy(robotAngle.unaryMinus());
    return new ChassisAccelerations(rotated.getX(), rotated.getY(), alpha);
  }

  /**
   * Converts this robot-relative set of accelerations into a field-relative ChassisAccelerations
   * object.
   *
   * @param robotAngle The angle of the robot as measured by a gyroscope. The robot's angle is
   *     considered to be zero when it is facing directly away from your alliance station wall.
   *     Remember that this should be CCW positive.
   * @return ChassisAccelerations object representing the accelerations in the field's frame of
   *     reference.
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
          MathUtil.lerp(this.ax, endValue.ax, t),
          MathUtil.lerp(this.ay, endValue.ay, t),
          MathUtil.lerp(this.alpha, endValue.alpha, t));
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
        "ChassisAccelerations(Ax: %.2f m/s², Ay: %.2f m/s², Alpha: %.2f rad/s²)", ax, ay, alpha);
  }
}
