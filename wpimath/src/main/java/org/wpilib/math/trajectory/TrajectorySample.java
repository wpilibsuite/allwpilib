// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.wpilib.units.Units.Seconds;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import java.util.Objects;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Transform2d;
import org.wpilib.math.geometry.Twist2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisSpeeds;
import org.wpilib.math.trajectory.proto.TrajectorySampleProto;
import org.wpilib.math.trajectory.struct.TrajectorySampleStruct;
import org.wpilib.math.util.MathUtil;
import org.wpilib.units.measure.Time;
import org.wpilib.util.struct.StructSerializable;

/** Represents a single sample in a trajectory. */
@JsonPropertyOrder({"timestamp", "pose", "velocity", "acceleration"})
public class TrajectorySample implements StructSerializable {
  /** The timestamp of the sample relative to the trajectory start. */
  @JsonIgnore public final Time timestamp;

  /** The timestamp of the sample relative to the trajectory start in seconds. */
  @JsonProperty("timestamp")
  private final double timeSeconds;

  /** The robot pose at this sample (in the field reference frame). */
  public final Pose2d pose;

  /** The robot velocity at this sample (in the robot's reference frame). */
  public final ChassisSpeeds velocity;

  /** The robot acceleration at this sample (in the robot's reference frame). */
  public final ChassisAccelerations acceleration;

  /** Base struct for serialization. */
  public static final TrajectorySampleStruct struct = new TrajectorySampleStruct();

  /** Base proto for serialization. */
  public static final TrajectorySampleProto proto = new TrajectorySampleProto();

  /**
   * Constructs a TrajectorySample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the robot's reference frame).
   * @param acceleration The robot acceleration at this sample (in the robot's reference frame).
   */
  public TrajectorySample(
      Time timestamp, Pose2d pose, ChassisSpeeds velocity, ChassisAccelerations acceleration) {
    this.timestamp = timestamp;
    this.pose = pose;
    this.velocity = velocity;
    this.acceleration = acceleration;

    this.timeSeconds = timestamp.in(Seconds);
  }

  /**
   * Constructs a TrajectorySample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory start, in seconds.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the robot's reference frame).
   * @param acceleration The robot acceleration at this sample (in the robot's reference frame).
   */
  @JsonCreator
  public TrajectorySample(
      double timestamp, Pose2d pose, ChassisSpeeds velocity, ChassisAccelerations acceleration) {
    this(Seconds.of(timestamp), pose, velocity, acceleration);
  }

  @Override
  public int hashCode() {
    return Objects.hash(timestamp, pose, velocity, acceleration);
  }

  @Override
  public final boolean equals(Object o) {
    if (!(o instanceof TrajectorySample that)) {
      return false;
    }

    return timestamp.equals(that.timestamp)
        && pose.equals(that.pose)
        && velocity.equals(that.velocity)
        && acceleration.equals(that.acceleration);
  }

  /**
   * Integrates assuming constant acceleration.
   *
   * @param dt timestamp
   * @return new sample
   */
  public TrajectorySample integrate(Time dt) {
    var dts = dt.in(Seconds);

    var newVel =
        new ChassisSpeeds(
            velocity.vx + acceleration.ax * dts,
            velocity.vy + acceleration.ay * dts,
            velocity.omega + acceleration.alpha * dts);

    var newPose =
        pose.plus(new Twist2d(newVel.vx * dts, newVel.vy * dts, newVel.omega * dts).exp());

    return new TrajectorySample(timestamp.plus(dt), newPose, newVel, acceleration);
  }

  /**
   * Interpolates between two samples using constant-acceleratopm kinematic equations.
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

    var interpDt = MathUtil.lerp(start.timestamp.in(Seconds), end.timestamp.in(Seconds), t);

    var newAccel =
        new ChassisAccelerations(
            MathUtil.lerp(start.acceleration.ax, end.acceleration.ax, t),
            MathUtil.lerp(start.acceleration.ay, end.acceleration.ay, t),
            MathUtil.lerp(start.acceleration.alpha, end.acceleration.alpha, t));

    // vₖ₊₁ = vₖ + aₖΔt
    var newVel =
        new ChassisSpeeds(
            start.velocity.vx + start.acceleration.ax * interpDt,
            start.velocity.vy + start.acceleration.ay * interpDt,
            start.velocity.omega + start.acceleration.alpha * interpDt);

    // xₖ₊₁ = xₖ + vₖΔt + ½a(Δt)²
    var newPose =
        new Pose2d(
            start.pose.getX()
                + start.velocity.vx * interpDt
                + 0.5 * start.acceleration.ax * interpDt * interpDt,
            start.pose.getY()
                + start.velocity.vy * interpDt
                + 0.5 * start.acceleration.ay * interpDt * interpDt,
            new Rotation2d(
                start.pose.getRotation().getRadians()
                    + start.velocity.omega * interpDt
                    + 0.5 * start.acceleration.alpha * interpDt * interpDt));

    return new TrajectorySample(Seconds.of(interpDt), newPose, newVel, newAccel);
  }

  /**
   * Transforms the pose of this sample by the given transform.
   *
   * @param transform The transform to apply to the pose.
   * @return A new sample with the transformed pose.
   */
  public TrajectorySample transform(Transform2d transform) {
    return new TrajectorySample(timestamp, pose.transformBy(transform), velocity, acceleration);
  }

  /**
   * Transforms this sample to be relative to the given pose.
   *
   * @param other The pose to make this sample relative to.
   * @return A new sample with the relative pose.
   */
  public TrajectorySample relativeTo(Pose2d other) {
    return new TrajectorySample(timestamp, pose.relativeTo(other), velocity, acceleration);
  }

  /**
   * Creates a new sample with the given timestamp.
   *
   * @param timestamp The new timestamp.
   * @return A new sample with the given timestamp.
   */
  public TrajectorySample withNewTimestamp(Time timestamp) {
    return new TrajectorySample(timestamp, pose, velocity, acceleration);
  }
}
