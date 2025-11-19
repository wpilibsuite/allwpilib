// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.wpilib.units.Units.Seconds;

import java.util.Arrays;
import java.util.List;
import java.util.stream.Stream;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Transform2d;
import org.wpilib.math.util.MathUtil;

/** A trajectory for spline-based path following. */
public class SplineTrajectory extends Trajectory<SplineSample> {
  /**
   * Constructs a SplineTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  public SplineTrajectory(SplineSample[] samples) {
    super(samples);
  }

  /**
   * Constructs a SplineTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  public SplineTrajectory(List<SplineSample> samples) {
    super(samples.toArray(SplineSample[]::new));
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
  public SplineSample interpolate(SplineSample start, SplineSample end, double t) {
    // Find the new t value.
    final double newT = MathUtil.lerp(start.timestamp.in(Seconds), end.timestamp.in(Seconds), t);

    // Find the delta time between the current state and the interpolated state.
    final double deltaT = newT - start.timestamp.in(Seconds);

    // If delta time is negative, flip the order of interpolation.
    if (deltaT < 0) {
      return interpolate(end, start, 1 - t);
    }

    // Check whether the robot is reversing at this stage.
    final boolean reversing =
        start.velocity.vx < 0 || Math.abs(start.velocity.vx) < 1E-9 && start.acceleration.ax < 0;

    // Calculate the new velocity
    // v_f = v_0 + at
    final double newV = start.velocity.vx + (start.acceleration.ax * deltaT);

    // Calculate the change in position.
    // delta_s = v_0 t + 0.5at²
    final double newS =
        (start.velocity.vx * deltaT + 0.5 * start.acceleration.ax * Math.pow(deltaT, 2))
            * (reversing ? -1.0 : 1.0);

    // Return the new state. To find the new position for the new state, we need
    // to interpolate between the two endpoint poses. The fraction for
    // interpolation is the change in position (delta s) divided by the total
    // distance between the two endpoints.
    final double interpolationFrac =
        newS / end.pose.getTranslation().getDistance(start.pose.getTranslation());

    return new SplineSample(
        newT,
        start.pose.plus(end.pose.minus(start.pose).times(interpolationFrac)),
        newV,
        start.acceleration.ax + (end.acceleration.ax - start.acceleration.ax) * t,
        MathUtil.lerp(start.curvature, end.curvature, t));
  }

  @Override
  public SplineTrajectory transformBy(Transform2d transform) {
    Pose2d firstPose = start().pose;
    Pose2d transformedFirstPose = firstPose.transformBy(transform);

    SplineSample transformedFirstSample =
        new SplineSample(
            start().timestamp,
            transformedFirstPose,
            start().velocity,
            start().acceleration,
            start().curvature);

    Stream<SplineSample> transformedSamples =
        Arrays.stream(samples)
            .skip(1)
            .map(
                sample ->
                    new SplineSample(
                        sample.timestamp,
                        transformedFirstPose.plus(sample.pose.minus(firstPose)),
                        sample.velocity,
                        sample.acceleration,
                        sample.curvature));

    return new SplineTrajectory(
        Stream.concat(Stream.of(transformedFirstSample), transformedSamples)
            .toArray(SplineSample[]::new));
  }

  @Override
  public SplineTrajectory concatenate(Trajectory<SplineSample> other) {
    if (other.samples.length < 1) {
      return this;
    }

    var withNewTimestamp =
        Arrays.stream(other.samples)
            .map(s -> s.withNewTimestamp(s.timestamp.plus(this.duration)))
            .toArray(SplineSample[]::new);

    var combinedSamples =
        Stream.concat(Arrays.stream(samples), Arrays.stream(withNewTimestamp))
            .toArray(SplineSample[]::new);

    return new SplineTrajectory(combinedSamples);
  }

  @Override
  public SplineTrajectory relativeTo(Pose2d other) {
    return new SplineTrajectory(
        Arrays.stream(samples).map(s -> s.relativeTo(other)).toArray(SplineSample[]::new));
  }
}
