package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Transform2d;
import java.util.ArrayList;
import java.util.List;

/** A trajectory for spline-based path following. */
public class SplineTrajectory extends Trajectory<SplineSample> {

  /**
   * Constructs a SplineTrajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  public SplineTrajectory(List<SplineSample> samples) {
    super(samples);
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
        start.velocity.vx < 0
            || Math.abs(start.velocity.vx) < 1E-9 && start.acceleration.ax < 0;

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
    return new SplineTrajectory(samples.stream().map(s -> s.transform(transform)).toList());
  }

  @Override
  public SplineTrajectory concatenate(Trajectory<SplineSample> other) {
    if (other.samples.isEmpty()) {
      return this;
    }

    var combinedSamples = new ArrayList<>(this.samples);
    combinedSamples.addAll(
        other.samples.stream()
            .map(s -> s.withNewTimestamp(s.timestamp.plus(this.duration)))
            .toList());

    return new SplineTrajectory(combinedSamples);
  }

  @Override
  public SplineTrajectory relativeTo(Pose2d other) {
    return new SplineTrajectory(samples.stream().map(s -> s.relativeTo(other)).toList());
  }

  @Override
  public SplineTrajectory reversed() {
    var reversedSamples = new ArrayList<SplineSample>();
    var lastTimestamp = this.duration;

    for (int i = samples.size() - 1; i >= 0; i--) {
      var sample = samples.get(i);
      var newTimestamp = lastTimestamp.minus(sample.timestamp);

      // Create a transform that rotates 180 degrees to reverse direction
      var reverseTransform = new Transform2d(0, 0, new Rotation2d(Math.PI));

      // Transform the sample and update timestamp
      var reversedSample = sample.transform(reverseTransform).withNewTimestamp(newTimestamp);
      reversedSamples.add(reversedSample);
    }

    return new SplineTrajectory(reversedSamples);
  }
}

