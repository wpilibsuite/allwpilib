// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import java.util.List;
import java.util.stream.Stream;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Transform2d;
import org.wpilib.math.util.MathUtil;

/** A trajectory for spline-based path following. */
public class DrivetrainSplineTrajectory extends Trajectory<DrivetrainSplineSample> {
  /**
   * Constructs a DrivetrainSplineTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  public DrivetrainSplineTrajectory(DrivetrainSplineSample[] samples) {
    super(samples);
  }

  /**
   * Constructs a DrivetrainSplineTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  public DrivetrainSplineTrajectory(List<DrivetrainSplineSample> samples) {
    super(samples.toArray(DrivetrainSplineSample[]::new));
  }

  /**
   * Interpolates between two samples using constant-acceleration kinematic equations.
   *
   * @param start The starting sample.
   * @param end The ending sample.
   * @param t The interpolation parameter between 0 and 1.
   * @return The interpolated sample.
   */
  @Override
  public DrivetrainSplineSample interpolate(
      DrivetrainSplineSample start, DrivetrainSplineSample end, double t) {
    // Find the new t value.
    final double newT = MathUtil.lerp(start.time, end.time, t);

    // Find the delta time between the current state and the interpolated state.
    final double deltaT = newT - start.time;

    // If delta time is negative, flip the order of interpolation.
    if (deltaT < 0) {
      return interpolate(end, start, 1 - t);
    }

    // Velocities and accelerations are stored field-relative; project them onto
    // the sample's heading to recover the signed forward (path-relative) scalars.
    final double startForwardVelocity = start.forwardVelocity();
    final double startForwardAccel = start.forwardAcceleration();

    // Check whether the robot is reversing at this stage.
    final boolean reversing =
        startForwardVelocity < 0 || Math.abs(startForwardVelocity) < 1E-9 && startForwardAccel < 0;

    // Calculate the new velocity
    // v_f = v_0 + at
    final double newV = startForwardVelocity + (startForwardAccel * deltaT);

    // Calculate the change in position.
    // delta_s = v_0 t + 0.5at²
    final double newS =
        (startForwardVelocity * deltaT + 0.5 * startForwardAccel * Math.pow(deltaT, 2))
            * (reversing ? -1.0 : 1.0);

    // Return the new state. To find the new position for the new state, we need
    // to interpolate between the two endpoint poses. The fraction for
    // interpolation is the change in position (delta s) divided by the total
    // distance between the two endpoints.
    final double interpolationFrac =
        newS / end.pose.getTranslation().getDistance(start.pose.getTranslation());

    return new DrivetrainSplineSample(
        newT,
        start.pose.plus(end.pose.minus(start.pose).times(interpolationFrac)),
        newV,
        MathUtil.lerp(startForwardAccel, end.forwardAcceleration(), t),
        MathUtil.lerp(start.curvature, end.curvature, t));
  }

  /**
   * Transforms all poses in the trajectory by the given transform.
   *
   * @param transform The transform to apply to the trajectory.
   * @return The transformed trajectory.
   */
  @Override
  public DrivetrainSplineTrajectory transformBy(Transform2d transform) {
    Pose2d firstPose = start().pose;
    Pose2d transformedFirstPose = firstPose.transformBy(transform);

    // The whole trajectory is rigidly rotated by the transform's rotation, so
    // the field-relative velocities and accelerations rotate by the same amount.
    Rotation2d rotation = transform.getRotation();

    DrivetrainSplineSample transformedFirstSample =
        new DrivetrainSplineSample(
            start().time,
            transformedFirstPose,
            start().velocity.toFieldRelative(rotation),
            start().acceleration.toFieldRelative(rotation),
            start().curvature);

    Stream<DrivetrainSplineSample> transformedSamples =
        samples.stream()
            .skip(1)
            .map(
                sample ->
                    new DrivetrainSplineSample(
                        sample.time,
                        transformedFirstPose.plus(sample.pose.minus(firstPose)),
                        sample.velocity.toFieldRelative(rotation),
                        sample.acceleration.toFieldRelative(rotation),
                        sample.curvature));

    return new DrivetrainSplineTrajectory(
        Stream.concat(Stream.of(transformedFirstSample), transformedSamples).toList());
  }

  /**
   * Concatenates this trajectory with another trajectory.
   *
   * @param other the other trajectory to concatenate with this one.
   * @return a new trajectory that is the concatenation of this trajectory and the other trajectory.
   */
  @Override
  public DrivetrainSplineTrajectory concatenate(Trajectory<DrivetrainSplineSample> other) {
    if (other.samples.isEmpty()) {
      return this;
    }

    var timeShifted =
        other.samples.stream()
            .map(
                s ->
                    new DrivetrainSplineSample(
                        s.time + this.duration, s.pose, s.velocity, s.acceleration, s.curvature));

    return new DrivetrainSplineTrajectory(Stream.concat(samples.stream(), timeShifted).toList());
  }

  /**
   * Returns a new trajectory that is relative to the given pose.
   *
   * @param other the pose to which the trajectory should be relative.
   * @return a new trajectory relative to the given pose.
   */
  @Override
  public DrivetrainSplineTrajectory relativeTo(Pose2d other) {
    return new DrivetrainSplineTrajectory(samples.stream().map(s -> s.relativeTo(other)).toList());
  }
}
