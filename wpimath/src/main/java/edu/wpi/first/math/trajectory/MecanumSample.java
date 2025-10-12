package edu.wpi.first.math.trajectory;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.kinematics.ChassisAccelerations;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;
import edu.wpi.first.math.kinematics.MecanumDriveWheelSpeeds;
import edu.wpi.first.units.measure.Time;

public class MecanumSample extends TrajectorySample<MecanumSample> {
  /** The speeds of the wheels in the robot's reference frame, in meters per second. */
  public final MecanumDriveWheelSpeeds speeds;

  /**
   * Constructs a MecanumSample.
   *
   * @param timestamp The timestamp of the sample.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the robot's reference frame).
   * @param acceleration The robot acceleration at this sample (in the robot's reference frame).
   * @param speeds The mecanum wheel speeds.
   */
  public MecanumSample(
      Time timestamp,
      Pose2d pose,
      ChassisSpeeds velocity,
      ChassisAccelerations acceleration,
      MecanumDriveWheelSpeeds speeds) {
    super(timestamp, pose, velocity, acceleration);

    this.speeds = speeds;
  }

  /**
   * Constructs a MecanumSample.
   *
   * @param timeSeconds The timestamp of the sample in seconds.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the robot's reference frame).
   * @param acceleration The robot acceleration at this sample (in the robot's reference frame).
   * @param speeds The mecanum wheel speeds.
   */
  @JsonCreator
  public MecanumSample(
      @JsonProperty("timestamp") double timeSeconds,
      @JsonProperty("pose") Pose2d pose,
      @JsonProperty("velocity") ChassisSpeeds velocity,
      @JsonProperty("acceleration") ChassisAccelerations acceleration,
      @JsonProperty("wheelSpeeds") MecanumDriveWheelSpeeds speeds) {
    super(timeSeconds, pose, velocity, acceleration);

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
    this(sample.timestamp, sample.pose, sample.velocity, sample.acceleration, speeds);
  }

  /**
   * Constructs a MecanumSample from another TrajectorySample.
   *
   * @param sample The TrajectorySample to copy.
   * @param kinematics The mecanum drivetrain kinematics.
   */
  public MecanumSample(TrajectorySample<?> sample, MecanumDriveKinematics kinematics) {
    this(sample.timestamp, sample.pose, sample.velocity, sample.acceleration, kinematics);
  }

  @Override
  public MecanumSample transform(Transform2d transform) {
    return new MecanumSample(
        this.timestamp,
        this.pose.transformBy(transform),
        this.velocity,
        this.acceleration,
        this.speeds);
  }

  @Override
  public MecanumSample relativeTo(Pose2d other) {
    return new MecanumSample(
        this.timestamp, this.pose.relativeTo(other), this.velocity, this.acceleration, this.speeds);
  }

  @Override
  public MecanumSample withNewTimestamp(Time timestamp) {
    return new MecanumSample(timestamp, pose, velocity, acceleration, speeds);
  }
}
