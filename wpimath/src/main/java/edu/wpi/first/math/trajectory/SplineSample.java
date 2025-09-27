package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.kinematics.ChassisAccelerations;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.units.measure.Time;

public class SplineSample extends TrajectorySample<SplineSample> {
  public final double curvature;

  /** Constructs a SplineSample. */
  public SplineSample(double timestamp, Pose2d pose, double vel, double accel, double curvature) {
    super(
        Seconds.of(timestamp),
        pose,
        new ChassisSpeeds(vel, 0.0, vel * curvature),
        new ChassisAccelerations(accel, 0.0, accel * curvature));
    this.curvature = curvature;
  }

  /** Constructs a SplineSample. */
  public SplineSample(
      Time timestamp,
      Pose2d pose,
      ChassisSpeeds vel,
      ChassisAccelerations accel,
      double curvature) {
    super(timestamp, pose, vel, accel);
    this.curvature = curvature;
  }

  /**
   * Constructs a SplineSample from another TrajectorySample, assuming the other sample's linear
   * velocity is not zero.
   */
  public SplineSample(TrajectorySample<?> sample) {
    super(sample.timestamp, sample.pose, sample.velocity, sample.acceleration);
    this.curvature =
        sample.velocity.omega / (sample.velocity.vx == 0.0 ? 1E-9 : sample.velocity.vx);
  }

  public SplineSample() {
    this(0.0, Pose2d.kZero, 0.0, 0.0, 0.0);
  }

  @Override
  public SplineSample interpolate(SplineSample endValue, double t) {
    // Find the new t value.
    final double newT =
        MathUtil.lerp(timestamp.in(Seconds), endValue.timestamp.in(Seconds), t);

    // Find the delta time between the current state and the interpolated state.
    final double deltaT = newT - timestamp.in(Seconds);

    // If delta time is negative, flip the order of interpolation.
    if (deltaT < 0) {
      return endValue.interpolate(this, 1 - t);
    }

    // Check whether the robot is reversing at this stage.
    final boolean reversing =
        velocity.vx < 0 || Math.abs(velocity.vx) < 1E-9 && acceleration.ax < 0;

    // Calculate the new velocity
    // v_f = v_0 + at
    final double newV = velocity.vx + (acceleration.ax * deltaT);

    // Calculate the change in position.
    // delta_s = v_0 t + 0.5at²
    final double newS =
        (velocity.vx * deltaT + 0.5 * acceleration.ax * Math.pow(deltaT, 2))
            * (reversing ? -1.0 : 1.0);

    // Return the new state. To find the new position for the new state, we need
    // to interpolate between the two endpoint poses. The fraction for
    // interpolation is the change in position (delta s) divided by the total
    // distance between the two endpoints.
    final double interpolationFrac =
        newS / endValue.pose.getTranslation().getDistance(pose.getTranslation());

    return new SplineSample(
        newT,
        pose.plus(endValue.pose.minus(this.pose).times(interpolationFrac)),
        newV,
        acceleration.ax + (endValue.acceleration.ax - acceleration.ax) * t,
        MathUtil.lerp(curvature, endValue.curvature, t));
  }

  @Override
  public SplineSample transform(Transform2d transform) {
    return new SplineSample(
        timestamp, pose.transformBy(transform), velocity, acceleration, curvature);
  }

  @Override
  public SplineSample relativeTo(Pose2d other) {
    return new SplineSample(timestamp, pose.relativeTo(other), velocity, acceleration, curvature);
  }

  @Override
  public SplineSample withNewTimestamp(Time timestamp) {
    return new SplineSample(timestamp, pose, velocity, acceleration, curvature);
  }
}
