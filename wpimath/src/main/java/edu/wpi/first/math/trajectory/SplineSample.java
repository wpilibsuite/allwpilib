package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.kinematics.ChassisAccelerations;
import edu.wpi.first.math.kinematics.ChassisSpeeds;

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

  public SplineSample(TrajectorySample<?> sample) {
    super(sample.timestamp, sample.pose, sample.vel, sample.accel);
    this.curvature = sample.vel.omega / (sample.vel.vx == 0.0 ? 1E-9 : sample.vel.vx);
  }

  public SplineSample() {
    this(0.0, Pose2d.kZero, 0.0, 0.0, 0.0);
  }

  @Override
  public SplineSample interpolate(SplineSample endValue, double t) {
    // Find the new t value.
    final double newT =
        MathUtil.interpolate(timestamp.in(Seconds), endValue.timestamp.in(Seconds), t);

    // Find the delta time between the current state and the interpolated state.
    final double deltaT = newT - timestamp.in(Seconds);

    // If delta time is negative, flip the order of interpolation.
    if (deltaT < 0) {
      return endValue.interpolate(this, 1 - t);
    }

    // Check whether the robot is reversing at this stage.
    final boolean reversing = vel.vx < 0 || Math.abs(vel.vx) < 1E-9 && accel.ax < 0;

    // Calculate the new velocity
    // v_f = v_0 + at
    final double newV = vel.vx + (accel.ax * deltaT);

    // Calculate the change in position.
    // delta_s = v_0 t + 0.5at²
    final double newS =
        (vel.vx * deltaT + 0.5 * accel.ax * Math.pow(deltaT, 2)) * (reversing ? -1.0 : 1.0);

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
        accel.ax + (endValue.accel.ax - accel.ax) * t,
        MathUtil.interpolate(curvature, endValue.curvature, t));
  }

  @Override
  public SplineSample fromSample(TrajectorySample<?> sample) {
    return new SplineSample(sample);
  }
}
