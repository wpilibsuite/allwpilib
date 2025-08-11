package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.interpolation.Interpolatable;
import edu.wpi.first.math.kinematics.ChassisAccelerations;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.trajectory.struct.TrajectorySampleStruct;
import edu.wpi.first.units.measure.Time;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Objects;

/** Represents a single sample in a trajectory. */
public class TrajectorySample implements Interpolatable<TrajectorySample>, StructSerializable {
  /** The timestamp of the sample relative to the trajectory start. */
  public final Time timestamp;

  /** The robot pose at this sample (in the field reference frame). */
  public final Pose2d pose;

  /** The robot velocity at this sample (in the robot's reference frame). */
  public final ChassisSpeeds vel;

  /** The robot acceleration at this sample (in the robot's reference frame). */
  public final ChassisAccelerations accel;

  /** TrajectorySample struct for serialization. */
  public static final TrajectorySampleStruct struct = new TrajectorySampleStruct();

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
    if (!(o instanceof TrajectorySample that)) {
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
  public TrajectorySample integrate(Time dt) {
    var dts = dt.in(Seconds);

    var newVel =
        new ChassisSpeeds(
            vel.vx + accel.ax * dts, vel.vy + accel.ay * dts, vel.omega + accel.alpha * dts);

    var newPose = pose.exp(new Twist2d(newVel.vx * dts, newVel.vy * dts, newVel.omega * dts));

    return new TrajectorySample(timestamp.plus(dt), newPose, newVel, accel);
  }

  /**
   * Interpolates between this sample and the end sample using constant-jerk kinematic equations.
   *
   * @param end The end sample.
   * @param t The time between this sample and the end sample. Should be in the range [0, 1].
   * @return new sample
   */
  @Override
  public TrajectorySample interpolate(TrajectorySample end, double t) {
    if (t <= 0) {
      return this;
    } else if (t >= 1) {
      return end;
    }

    var totalDt = this.timestamp.minus(end.timestamp).in(Seconds);
    var interpDt = MathUtil.interpolate(this.timestamp.in(Seconds), end.timestamp.in(Seconds), t);

    // note that the units of jerk are m/s³ and rad/s³
    // do not add this to an acceleration object unless you know units are accurate
    var jerk = this.accel.minus(end.accel).div(totalDt);

    // aₖ₊₁ = aₖ + jΔt
    var newAccel =
        new ChassisAccelerations(
            accel.ax + jerk.ax * interpDt,
            accel.ay + jerk.ay * interpDt,
            accel.alpha + jerk.alpha * interpDt);

    // vₖ₊₁ = vₖ + aₖΔt + ½jₖ(Δt)²
    var newVel =
        new ChassisSpeeds(
            vel.vx + accel.ax * interpDt + 0.5 * jerk.ax * interpDt * interpDt,
            vel.vy + accel.ay * interpDt + 0.5 * jerk.ay * interpDt * interpDt,
            vel.omega + accel.alpha * interpDt + 0.5 * jerk.alpha * interpDt * interpDt);

    // xₖ₊₁ = xₖ + vₖΔt + ½a(Δt)² + ⅙j(Δt)³
    var newPose =
        new Pose2d(
            pose.getX()
                + vel.vx * interpDt
                + 0.5 * accel.ax * interpDt * interpDt
                + (1.0 / 6) * jerk.ax * Math.pow(interpDt, 3),
            pose.getY()
                + vel.vy * interpDt
                + 0.5 * accel.ay * interpDt * interpDt
                + (1.0 / 6) * jerk.ay * Math.pow(interpDt, 3),
            new Rotation2d(
                pose.getRotation().getRadians()
                    + vel.omega * interpDt
                    + 0.5 * accel.alpha * interpDt * interpDt
                    + (1.0 / 6) * jerk.alpha * Math.pow(interpDt, 3)));

    return new TrajectorySample(Seconds.of(interpDt), newPose, newVel, newAccel);
  }
}
