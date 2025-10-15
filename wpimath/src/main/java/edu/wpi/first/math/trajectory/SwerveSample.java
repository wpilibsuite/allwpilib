package edu.wpi.first.math.trajectory;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.kinematics.ChassisAccelerations;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.kinematics.SwerveDriveKinematics;
import edu.wpi.first.math.kinematics.SwerveModuleState;
import edu.wpi.first.units.measure.Time;
import java.util.Arrays;

public class SwerveSample extends TrajectorySample<SwerveSample> {
  /**
   * The states of the wheels in the robot's reference frame, in the order that each wheel is
   * specified in the {@link SwerveDriveKinematics} object.
   */
  public final SwerveModuleState[] states;

  /**
   * Constructs a SwerveSample.
   *
   * @param timestamp The timestamp of the sample.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the robot's reference frame).
   * @param acceleration The robot acceleration at this sample (in the robot's reference frame).
   * @param states The swerve module states at this sample.
   */
  public SwerveSample(
      Time timestamp,
      Pose2d pose,
      ChassisSpeeds velocity,
      ChassisAccelerations acceleration,
      SwerveModuleState... states) {
    super(timestamp, pose, velocity, acceleration);

    this.states = Arrays.copyOf(states, states.length);
  }

  /**
   * Constructs a SwerveSample.
   *
   * @param timestamp The timestamp of the sample in seconds.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the robot's reference frame).
   * @param acceleration The robot acceleration at this sample (in the robot's reference frame).
   * @param states The swerve module states at this sample.
   */
  @JsonCreator
  public SwerveSample(
      @JsonProperty("timestamp") double timestamp,
      @JsonProperty("pose") Pose2d pose,
      @JsonProperty("velocity") ChassisSpeeds velocity,
      @JsonProperty("acceleration") ChassisAccelerations acceleration,
      @JsonProperty("states") SwerveModuleState... states) {
    super(timestamp, pose, velocity, acceleration);

    this.states = Arrays.copyOf(states, states.length);
  }

  /**
   * Constructs a SwerveSample.
   *
   * @param timestamp The timestamp of the sample.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param velocity The robot velocity at this sample (in the robot's reference frame).
   * @param acceleration The robot acceleration at this sample (in the robot's reference frame).
   * @param kinematics The kinematics of the drivetrain.
   */
  public SwerveSample(
      Time timestamp,
      Pose2d pose,
      ChassisSpeeds velocity,
      ChassisAccelerations acceleration,
      SwerveDriveKinematics kinematics) {
    super(timestamp, pose, velocity, acceleration);

    states = kinematics.toWheelSpeeds(velocity);
  }

  /**
   * Constructs a SwerveSample from another TrajectorySample.
   *
   * @param sample The TrajectorySample to copy.
   * @param kinematics The kinematics of the drivetrain.
   */
  public SwerveSample(TrajectorySample<?> sample, SwerveDriveKinematics kinematics) {
    this(sample.timestamp, sample.pose, sample.velocity, sample.acceleration, kinematics);
  }

  @Override
  public SwerveSample transform(Transform2d transform) {
    return new SwerveSample(timestamp, pose.transformBy(transform), velocity, acceleration, states);
  }

  @Override
  public SwerveSample relativeTo(Pose2d other) {
    return new SwerveSample(timestamp, pose.relativeTo(other), velocity, acceleration, states);
  }

  @Override
  public SwerveSample withNewTimestamp(Time timestamp) {
    return new SwerveSample(timestamp, pose, velocity, acceleration, states);
  }
}
