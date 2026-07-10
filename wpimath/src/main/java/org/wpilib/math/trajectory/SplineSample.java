// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import java.util.Objects;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Transform2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisVelocities;

/** Represents a single sample in a spline-based trajectory. */
public class SplineSample extends TrajectorySample {
  /** The curvature of the path at this sample, in rad/m. */
  public final double curvature;

  /**
   * Constructs a SplineSample from path-relative scalars.
   *
   * <p>The robot follows the path along its heading, so the scalar forward velocity and
   * acceleration are rotated into the field frame using the sample's pose. The resulting {@link
   * #velocity} and {@link #acceleration} are field-relative.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory start, in seconds.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The signed forward (path-tangential) velocity in meters per second.
   * @param acceleration The signed forward (path-tangential) acceleration in meters per second
   *     squared.
   * @param curvature The curvature of the path at this sample, in rad/m.
   */
  public SplineSample(
      double timestamp, Pose2d pose, double velocity, double acceleration, double curvature) {
    super(
        timestamp,
        pose,
        new ChassisVelocities(velocity, 0.0, velocity * curvature)
            .toFieldRelative(pose.getRotation()),
        new ChassisAccelerations(acceleration, 0.0, acceleration * curvature)
            .toFieldRelative(pose.getRotation()));
    this.curvature = curvature;
  }

  /**
   * Constructs a SplineSample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory start, in seconds.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the field reference frame).
   * @param acceleration The robot acceleration at this sample (in the field reference frame).
   * @param curvature The curvature of the path at this sample, in rad/m.
   */
  public SplineSample(
      double timestamp,
      Pose2d pose,
      ChassisVelocities velocity,
      ChassisAccelerations acceleration,
      double curvature) {
    super(timestamp, pose, velocity, acceleration);
    this.curvature = curvature;
  }

  /**
   * Constructs a SplineSample from another TrajectorySample, assuming the other sample's linear
   * velocity is not zero. Curvature is computed as omega / forward velocity.
   *
   * @param sample The TrajectorySample to convert.
   */
  public SplineSample(TrajectorySample sample) {
    super(sample.timestamp, sample.pose, sample.velocity, sample.acceleration);
    double vx = sample.velocity.toRobotRelative(sample.pose.getRotation()).vx;
    this.curvature = sample.velocity.omega / (vx == 0.0 ? 1e-9 : vx);
  }

  /** Constructs a SplineSample with all values set to zero. */
  public SplineSample() {
    this(0.0, Pose2d.kZero, 0.0, 0.0, 0.0);
  }

  /**
   * Returns the signed forward (robot-relative tangential) velocity at this sample, in meters per
   * second. This is the field-relative {@link #velocity} projected onto the sample's heading.
   *
   * @return The forward velocity in meters per second.
   */
  public double forwardVelocity() {
    return velocity.toRobotRelative(pose.getRotation()).vx;
  }

  /**
   * Returns the signed forward (robot-relative tangential) acceleration at this sample, in meters
   * per second squared. This is the field-relative {@link #acceleration} projected onto the
   * sample's heading.
   *
   * @return The forward acceleration in meters per second squared.
   */
  public double forwardAcceleration() {
    return acceleration.toRobotRelative(pose.getRotation()).ax;
  }

  /**
   * Transforms the pose of this sample by the given transform.
   *
   * @param transform The transform to apply to the pose.
   * @return A new sample with the transformed pose.
   */
  @Override
  public SplineSample transform(Transform2d transform) {
    return new SplineSample(
        timestamp,
        pose.transformBy(transform),
        velocity.toFieldRelative(transform.getRotation()),
        acceleration.toFieldRelative(transform.getRotation()),
        curvature);
  }

  /**
   * Transforms this sample to be relative to the given pose.
   *
   * @param other The pose to make this sample relative to.
   * @return A new sample with the relative pose.
   */
  @Override
  public SplineSample relativeTo(Pose2d other) {
    return new SplineSample(
        timestamp,
        pose.relativeTo(other),
        velocity.toRobotRelative(other.getRotation()),
        acceleration.toRobotRelative(other.getRotation()),
        curvature);
  }

  /**
   * Creates a new sample with the given timestamp.
   *
   * @param timestamp The new timestamp, in seconds.
   * @return A new sample with the given timestamp.
   */
  @Override
  public SplineSample withNewTimestamp(double timestamp) {
    return new SplineSample(timestamp, pose, velocity, acceleration, curvature);
  }

  @Override
  public boolean equals(Object o) {
    if (!super.equals(o)) {
      return false;
    }

    SplineSample that = (SplineSample) o;
    return Double.compare(curvature, that.curvature) == 0;
  }

  @Override
  public int hashCode() {
    return Objects.hash(timestamp, pose, velocity, acceleration, curvature);
  }
}
