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
import org.wpilib.math.trajectory.proto.HolonomicSampleProto;
import org.wpilib.math.trajectory.struct.HolonomicSampleStruct;
import org.wpilib.math.util.MathUtil;
import org.wpilib.units.measure.Time;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.StructSerializable;

/** Represents a single sample in a trajectory. */
@Json
public class HolonomicSample extends TrajectorySample
    implements StructSerializable, ProtobufSerializable {
  /** The robot pose at this sample (in the field reference frame). */
  @Json.Property("pose")
  public Pose2d pose;

  /** The robot velocity at this sample (in the field reference frame). */
  @Json.Property("velocity")
  public ChassisVelocities velocity;

  /** The robot acceleration at this sample (in the field reference frame). */
  @Json.Property("acceleration")
  public ChassisAccelerations acceleration;

  /** Base struct for serialization. */
  public static final HolonomicSampleStruct struct = new HolonomicSampleStruct();

  /** Base proto for serialization. */
  public static final HolonomicSampleProto proto = new HolonomicSampleProto();

  /**
   * Constructs a HolonomicSample.
   *
   * @param time The time of the sample relative to the trajectory start, in seconds.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the field reference frame).
   * @param acceleration The robot acceleration at this sample (in the field reference frame).
   */
  @Json.Creator
  public HolonomicSample(
      double time, Pose2d pose, ChassisVelocities velocity, ChassisAccelerations acceleration) {
    super(time);
    this.pose = pose;
    this.velocity = velocity;
    this.acceleration = acceleration;
  }

  /**
   * Constructs a HolonomicSample.
   *
   * @param time The time of the sample relative to the trajectory start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the field reference frame).
   * @param acceleration The robot acceleration at this sample (in the field reference frame).
   */
  public HolonomicSample(
      Time time, Pose2d pose, ChassisVelocities velocity, ChassisAccelerations acceleration) {
    this(time.in(Seconds), pose, velocity, acceleration);
  }

  @Override
  public int hashCode() {
    return Objects.hash(time, pose, velocity, acceleration);
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    HolonomicSample that = (HolonomicSample) o;
    return Double.compare(time, that.time) == 0
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
  public static HolonomicSample kinematicInterpolate(
      HolonomicSample start, HolonomicSample end, double t) {
    if (t <= 0) {
      return start;
    } else if (t >= 1) {
      return end;
    }

    // Absolute time of the interpolated sample
    var interpTime = MathUtil.lerp(start.time, end.time, t);

    // Elapsed time from the start sample, used for integration
    var deltaT = interpTime - start.time;

    // Constant acceleration
    var accel =
        new ChassisAccelerations(
            start.acceleration.ax, start.acceleration.ay, start.acceleration.alpha);

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

    return new HolonomicSample(interpTime, newPose, newVel, accel);
  }

  /**
   * Transforms the pose of this sample by the given transform.
   *
   * @param transform The transform to apply to the pose.
   * @return A new sample with the transformed pose.
   */
  public HolonomicSample transform(Transform2d transform) {
    return new HolonomicSample(
        time,
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
  public HolonomicSample relativeTo(Pose2d other) {
    return new HolonomicSample(
        time,
        pose.relativeTo(other),
        velocity.toRobotRelative(other.getRotation()),
        acceleration.toRobotRelative(other.getRotation()));
  }
}
