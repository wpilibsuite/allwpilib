// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.wpilib.units.Units.Seconds;

import io.avaje.jsonb.Json;
import java.util.Objects;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Transform2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.trajectory.proto.TrajectorySampleProto;
import org.wpilib.math.trajectory.struct.TrajectorySampleStruct;
import org.wpilib.math.util.MathUtil;
import org.wpilib.units.measure.Time;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/** Represents a single sample in a trajectory. */
@Json
public class TrajectorySample implements StructSerializable, ProtobufSerializable {
  /** The timestamp of the sample relative to the trajectory start, in seconds. */
  @Json.Property("timestamp")
  public final double timestamp;

  /** The robot pose at this sample (in the field reference frame). */
  @Json.Property("pose")
  public final Pose2d pose;

  /** The robot velocity at this sample (in the field reference frame). */
  @Json.Property("velocity")
  public final ChassisVelocities velocity;

  /** The robot acceleration at this sample (in the field reference frame). */
  @Json.Property("acceleration")
  public final ChassisAccelerations acceleration;

  /** Base struct for serialization. */
  public static final TrajectorySampleStruct struct = new TrajectorySampleStruct();

  /** Base proto for serialization. */
  public static final TrajectorySampleProto proto = new TrajectorySampleProto();

  /**
   * Constructs a TrajectorySample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory start, in seconds.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the field reference frame).
   * @param acceleration The robot acceleration at this sample (in the field reference frame).
   */
  @Json.Creator
  public TrajectorySample(
      double timestamp,
      Pose2d pose,
      ChassisVelocities velocity,
      ChassisAccelerations acceleration) {
    this.timestamp = timestamp;
    this.pose = pose;
    this.velocity = velocity;
    this.acceleration = acceleration;
  }

  public TrajectorySample(
      Time timestamp, Pose2d pose, ChassisVelocities velocity, ChassisAccelerations acceleration) {
    this(timestamp.in(Seconds), pose, velocity, acceleration);
  }

  @Override
  public int hashCode() {
    return Objects.hash(timestamp, pose, velocity, acceleration);
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    TrajectorySample that = (TrajectorySample) o;
    return Double.compare(timestamp, that.timestamp) == 0
        && pose.equals(that.pose)
        && velocity.equals(that.velocity)
        && acceleration.equals(that.acceleration);
  }

  /**
   * Interpolates between two samples using constant-acceleration kinematic equations.
   *
   * @param start The start sample.
   * @param end The end sample.
   * @param t The time between the start and end samples. Should be in the range [0, 1].
   * @return new sample.
   */
  public static TrajectorySample kinematicInterpolate(
      TrajectorySample start, TrajectorySample end, double t) {
    if (t <= 0) {
      return start;
    } else if (t >= 1) {
      return end;
    }

    var interpTime = MathUtil.lerp(start.timestamp, end.timestamp, t);
    var deltaT = interpTime - start.timestamp;

    var newAccel =
        new ChassisAccelerations(
            MathUtil.lerp(start.acceleration.ax, end.acceleration.ax, t),
            MathUtil.lerp(start.acceleration.ay, end.acceleration.ay, t),
            MathUtil.lerp(start.acceleration.alpha, end.acceleration.alpha, t));

    // vₖ₊₁ = vₖ + aₖΔt
    var newVel =
        new ChassisVelocities(
            start.velocity.vx + start.acceleration.ax * deltaT,
            start.velocity.vy + start.acceleration.ay * deltaT,
            start.velocity.omega + start.acceleration.alpha * deltaT);

    // xₖ₊₁ = xₖ + vₖΔt + ½aₖ(Δt)²
    var newPose =
        new Pose2d(
            start.pose.getX()
                + start.velocity.vx * deltaT
                + 0.5 * start.acceleration.ax * deltaT * deltaT,
            start.pose.getY()
                + start.velocity.vy * deltaT
                + 0.5 * start.acceleration.ay * deltaT * deltaT,
            new Rotation2d(
                start.pose.getRotation().getRadians()
                    + start.velocity.omega * deltaT
                    + 0.5 * start.acceleration.alpha * deltaT * deltaT));

    return new TrajectorySample(interpTime, newPose, newVel, newAccel);
  }

  /**
   * Transforms the pose of this sample by the given transform.
   *
   * @param transform The transform to apply to the pose.
   * @return A new sample with the transformed pose.
   */
  public TrajectorySample transform(Transform2d transform) {
    return new TrajectorySample(
        timestamp,
        pose.transformBy(transform),
        velocity.toFieldRelative(transform.getRotation()),
        acceleration.toFieldRelative(transform.getRotation()));
  }

  /**
   * Transforms this sample to be relative to the given pose.
   *
   * @param other The pose to make this sample relative to.
   * @return A new sample with the relative pose.
   */
  public TrajectorySample relativeTo(Pose2d other) {
    return new TrajectorySample(
        timestamp,
        pose.relativeTo(other),
        velocity.toRobotRelative(other.getRotation()),
        acceleration.toRobotRelative(other.getRotation()));
  }

  /**
   * Creates a new sample with the given timestamp.
   *
   * @param timestamp The new timestamp, in seconds.
   * @return A new sample with the given timestamp.
   */
  public TrajectorySample withNewTimestamp(double timestamp) {
    return new TrajectorySample(timestamp, pose, velocity, acceleration);
  }
}
