package edu.wpi.first.math.trajectory;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.kinematics.ChassisAccelerations;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;
import edu.wpi.first.math.kinematics.MecanumDriveWheelSpeeds;
import edu.wpi.first.units.measure.Time;

import static edu.wpi.first.units.Units.Seconds;

public class MecanumSample extends TrajectorySample<MecanumSample> {
  /** The speeds of the wheels in the robot's reference frame, in meters per second. */
  public final MecanumDriveWheelSpeeds speeds;

  /**
   * Constructs a MecanumSample.
   *
   * @param timestamp The timestamp of the sample.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param vel The robot velocity at this sample (in the robot's reference frame).
   * @param accel The robot acceleration at this sample (in the robot's reference frame).
   * @param speeds The mecanum wheel speeds.
   */
  public MecanumSample(
      Time timestamp,
      Pose2d pose,
      ChassisSpeeds vel,
      ChassisAccelerations accel,
      MecanumDriveWheelSpeeds speeds) {
    super(timestamp, pose, vel, accel);

    this.speeds = speeds;
  }

  /**
   * Constructs a MecanumSample.
   *
   * @param timeSeconds The timestamp of the sample in seconds.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param vel The robot velocity at this sample (in the robot's reference frame).
   * @param accel The robot acceleration at this sample (in the robot's reference frame).
   * @param speeds The mecanum wheel speeds.
   */
  @JsonCreator
  public MecanumSample(
      @JsonProperty("timestamp") double timeSeconds,
      @JsonProperty("pose") Pose2d pose,
      @JsonProperty("vel") ChassisSpeeds vel,
      @JsonProperty("accel") ChassisAccelerations accel,
      @JsonProperty("wheelSpeeds") MecanumDriveWheelSpeeds speeds) {
    super(timeSeconds, pose, vel, accel);

    this.speeds = speeds;
  }

  /**
   * Constructs a MecanumSample.
   *
   * @param timestamp The timestamp of the sample.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param vel The robot velocity at this sample (in the robot's reference frame).
   * @param accel The robot acceleration at this sample (in the robot's reference frame).
   * @param kinematics The kinematics of the drivetrain.
   */
  public MecanumSample(
      Time timestamp,
      Pose2d pose,
      ChassisSpeeds vel,
      ChassisAccelerations accel,
      MecanumDriveKinematics kinematics) {
    this(timestamp, pose, vel, accel, kinematics.toWheelSpeeds(vel));
  }

  /**
   * Constructs a MecanumSample from another TrajectorySample.
   *
   * @param sample The TrajectorySample to copy.
   * @param speeds The mecanum wheel speeds.
   */
  public MecanumSample(TrajectorySample<?> sample, MecanumDriveWheelSpeeds speeds) {
    this(sample.timestamp, sample.pose, sample.vel, sample.accel, speeds);
  }

  /**
   * Constructs a MecanumSample from another TrajectorySample.
   *
   * @param sample The TrajectorySample to copy.
   * @param kinematics The mecanum drivetrain kinematics.
   */
  public MecanumSample(TrajectorySample<?> sample, MecanumDriveKinematics kinematics) {
    this(sample.timestamp, sample.pose, sample.vel, sample.accel, kinematics);
  }

  /**
   * Linearly interpolates between this sample and the given sample.
   *
   * @param endValue The end sample.
   * @param t The time between this sample and the end sample. Should be in the range [0, 1].
   * @return new sample
   */
  @Override
  public MecanumSample interpolate(MecanumSample endValue, double t) {
    return new MecanumSample(
        Seconds.of(MathUtil.interpolate(this.timestamp.in(Seconds), endValue.timestamp.in(Seconds), t)),
        this.pose.interpolate(endValue.pose, t),
        this.vel.interpolate(endValue.vel, t),
        this.accel.interpolate(endValue.accel, t),
        new MecanumDriveWheelSpeeds(
            MathUtil.interpolate(this.speeds.frontLeft, endValue.speeds.frontLeft, t),
            MathUtil.interpolate(this.speeds.frontRight, endValue.speeds.frontRight, t),
            MathUtil.interpolate(this.speeds.rearLeft, endValue.speeds.rearLeft, t),
            MathUtil.interpolate(this.speeds.rearRight, endValue.speeds.rearRight, t)
        )
    );
  }

  @Override
  public MecanumSample transform(Transform2d transform) {
    return new MecanumSample(
        this.timestamp,
        this.pose.transformBy(transform),
        this.vel,
        this.accel,
        this.speeds
    );
  }
}
