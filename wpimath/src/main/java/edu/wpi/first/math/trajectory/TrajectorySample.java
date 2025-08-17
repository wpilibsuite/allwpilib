package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.Seconds;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.interpolation.Interpolatable;
import edu.wpi.first.math.kinematics.ChassisAccelerations;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.trajectory.struct.TrajectorySampleStruct;
import edu.wpi.first.units.measure.Time;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.Objects;

/** Represents a single sample in a trajectory. */
@JsonPropertyOrder({"timestamp", "pose", "vel", "accel"})
public abstract class TrajectorySample<SampleType extends TrajectorySample<SampleType>>
    implements Interpolatable<SampleType>, StructSerializable {
  /** The timestamp of the sample relative to the trajectory start. */
  @JsonIgnore public final Time timestamp;

  /** The timestamp of the sample relative to the trajectory start in seconds. */
  @JsonProperty("timestamp")
  private final double timeSeconds;

  /** The robot pose at this sample (in the field reference frame). */
  public final Pose2d pose;

  /** The robot velocity at this sample (in the robot's reference frame). */
  public final ChassisSpeeds vel;

  /** The robot acceleration at this sample (in the robot's reference frame). */
  public final ChassisAccelerations accel;

  /** Base struct for serialization. */
  public static TrajectorySampleStruct struct = new TrajectorySampleStruct();

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

    this.timeSeconds = timestamp.in(Seconds);
  }

  /**
   * Constructs a TrajectorySample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory start, in seconds.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param vel The robot velocity at this sample (in the robot's reference frame).
   * @param accel The robot acceleration at this sample (in the robot's reference frame).
   */
  @JsonCreator
  public TrajectorySample(
      double timestamp, Pose2d pose, ChassisSpeeds vel, ChassisAccelerations accel) {
    this(Seconds.of(timestamp), pose, vel, accel);
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

  public static Base kinematicInterpolate(
      TrajectorySample<?> start, TrajectorySample<?> end, double t) {
    if (t <= 0) {
      return new Base(start);
    } else if (t >= 1) {
      return new Base(end);
    }

    var interpDt = MathUtil.interpolate(start.timestamp.in(Seconds), end.timestamp.in(Seconds), t);

    var newAccel =
        new ChassisAccelerations(
            MathUtil.interpolate(start.accel.ax, end.accel.ax, t),
            MathUtil.interpolate(start.accel.ay, end.accel.ay, t),
            MathUtil.interpolate(start.accel.alpha, end.accel.alpha, t));

    // vₖ₊₁ = vₖ + aₖΔt
    var newVel =
        new ChassisSpeeds(
            start.vel.vx + start.accel.ax * interpDt,
            start.vel.vy + start.accel.ay * interpDt,
            start.vel.omega + start.accel.alpha * interpDt);

    // xₖ₊₁ = xₖ + vₖΔt + ½a(Δt)²
    var newPose =
        new Pose2d(
            start.pose.getX()
                + start.vel.vx * interpDt
                + 0.5 * start.accel.ax * interpDt * interpDt,
            start.pose.getY()
                + start.vel.vy * interpDt
                + 0.5 * start.accel.ay * interpDt * interpDt,
            new Rotation2d(
                start.pose.getRotation().getRadians()
                    + start.vel.omega * interpDt
                    + 0.5 * start.accel.alpha * interpDt * interpDt));

    return new Base(Seconds.of(interpDt), newPose, newVel, newAccel);
  }

  /**
   * Transforms the pose of this sample by the given transform.
   *
   * @param transform The transform to apply to the pose.
   * @return A new sample with the transformed pose.
   */
  public abstract SampleType transform(Transform2d transform);

  /** A base class for trajectory samples. */
  public static class Base extends TrajectorySample<Base> {
    @JsonCreator
    public Base(
        @JsonProperty("timestamp") double timestamp,
        @JsonProperty("pose") Pose2d pose,
        @JsonProperty("vel") ChassisSpeeds vel,
        @JsonProperty("accel") ChassisAccelerations accel) {
      super(timestamp, pose, vel, accel);
    }

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
     * @param t        The time between this sample and the end sample. Should be in the range [0, 1].
     * @return new sample
     */
    @Override
    public Base interpolate(Base endValue, double t) {
      return TrajectorySample.kinematicInterpolate(this, endValue, t);
    }

    @Override
    public Base transform(Transform2d transform) {
      return new Base(
          timestamp,
          pose.transformBy(transform),
          vel, accel);
    }
  }
}
