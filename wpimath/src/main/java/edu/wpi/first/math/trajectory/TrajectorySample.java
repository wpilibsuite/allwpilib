package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.interpolation.Interpolatable;
import edu.wpi.first.math.kinematics.ChassisAccelerations;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.units.measure.Time;
import java.util.Objects;

/** Represents a single sample in a trajectory. */
public abstract class TrajectorySample<SampleType extends TrajectorySample<SampleType>>
    implements Interpolatable<SampleType> {
  /** The timestamp of the sample relative to the trajectory start. */
  public final Time timestamp;

  /** The robot pose at this sample (in the field reference frame). */
  public final Pose2d pose;

  /** The robot velocity at this sample (in the robot's reference frame). */
  public final ChassisSpeeds vel;

  /** The robot acceleration at this sample (in the robot's reference frame). */
  public final ChassisAccelerations accel;

  /**
   * Constructs a TrajectorySample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param vel The robot velocity at this sample (in the robot's reference frame).
   * @param accel The robot acceleration at this sample (in the robot's reference frame).
   */
  public TrajectorySample(
      Time timestamp, Pose2d pose, ChassisSpeeds vel, ChassisAccelerations accel) {
    this.timestamp = timestamp;
    this.pose = pose;
    this.vel = vel;
    this.accel = accel;
  }

  @Override
  public int hashCode() {
    return Objects.hash(timestamp, pose, vel, accel);
  }

  @Override
  public final boolean equals(Object o) {
    if (!(o instanceof TrajectorySample<?> that)) {
      return false;
    }

    return timestamp.equals(that.timestamp)
        && pose.equals(that.pose)
        && vel.equals(that.vel)
        && accel.equals(that.accel);
  }

  /**
   * Integrates assuming constant acceleration.
   *
   * @param dt timestamp
   * @return new sample
   */
  public Base integrate(Time dt) {
    var dts = dt.in(Seconds);

    var newVel =
        new ChassisSpeeds(
            vel.vx + accel.ax * dts, vel.vy + accel.ay * dts, vel.omega + accel.alpha * dts);

    var newPose = pose.exp(new Twist2d(newVel.vx * dts, newVel.vy * dts, newVel.omega * dts));

    return new Base(timestamp.plus(dt), newPose, newVel, accel);
  }

  /** Creates a new sample from the given sample. */
  public abstract SampleType fromSample(TrajectorySample<?> sample);

  /**
   * Transforms the pose of this sample by the given transform.
   *
   * @param transform2d The transform to apply to the pose.
   * @return A new sample with the transformed pose.
   */
  public SampleType transform(Transform2d transform2d) {
    var newPose = pose.transformBy(transform2d);

    return fromSample(new Base(timestamp, newPose, vel, accel));
  }

  /** A base class for trajectory samples. */
  public static class Base extends TrajectorySample<Base> {
    public Base(Time timestamp, Pose2d pose, ChassisSpeeds vel, ChassisAccelerations accel) {
      super(timestamp, pose, vel, accel);
    }

    public Base(TrajectorySample<?> sample) {
      super(sample.timestamp, sample.pose, sample.vel, sample.accel);
    }

    /**
     * Interpolates between this sample and the end sample using constant-acceleratopm kinematic
     * equations.
     *
     * @param endValue The end sample.
     * @param t The time between this sample and the end sample. Should be in the range [0, 1].
     * @return new sample
     */
    @Override
    public Base interpolate(Base endValue, double t) {
      if (t <= 0) {
        return new Base(this);
      } else if (t >= 1) {
        return new Base(this);
      }

      var interpDt =
          MathUtil.interpolate(this.timestamp.in(Seconds), endValue.timestamp.in(Seconds), t);

      var newAccel =
          new ChassisAccelerations(
              MathUtil.interpolate(this.accel.ax, endValue.accel.ax, t),
              MathUtil.interpolate(this.accel.ay, endValue.accel.ay, t),
              MathUtil.interpolate(this.accel.alpha, endValue.accel.alpha, t));

      // vₖ₊₁ = vₖ + aₖΔt
      var newVel =
          new ChassisSpeeds(
              vel.vx + accel.ax * interpDt,
              vel.vy + accel.ay * interpDt,
              vel.omega + accel.alpha * interpDt);

      // xₖ₊₁ = xₖ + vₖΔt + ½a(Δt)²
      var newPose =
          new Pose2d(
              pose.getX() + vel.vx * interpDt + 0.5 * accel.ax * interpDt * interpDt,
              pose.getY() + vel.vy * interpDt + 0.5 * accel.ay * interpDt * interpDt,
              new Rotation2d(
                  pose.getRotation().getRadians()
                      + vel.omega * interpDt
                      + 0.5 * accel.alpha * interpDt * interpDt));

      return new Base(Seconds.of(interpDt), newPose, newVel, newAccel);
    }

    @Override
    public Base fromSample(TrajectorySample<?> sample) {
      return new Base(sample);
    }
  }
}
