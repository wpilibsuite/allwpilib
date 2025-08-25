package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.math.MathUtil;
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

  /**
   * Interpolates between this sample and the given sample using Euler integration on this sample
   * state.
   *
   * @param endValue The end sample.
   * @param t The time between this sample and the end sample. Should be in the range [0, 1].
   * @return new sample
   */
  @Override
  public SwerveSample interpolate(SwerveSample endValue, double t) {
    if (this.states.length != endValue.states.length) {
      throw new IllegalArgumentException(
          "Cannot interpolate SwerveSample with different number of states: "
              + this.states.length
              + " vs "
              + endValue.states.length);
    }
    SwerveModuleState[] newStates = new SwerveModuleState[this.states.length];
    for (int i = 0; i < this.states.length; i++) {
      newStates[i] = this.states[i].interpolate(endValue.states[i], t);
    }
    return new SwerveSample(
        Seconds.of(MathUtil.interpolate(timestamp.in(Seconds), endValue.timestamp.in(Seconds), t)),
        pose.interpolate(endValue.pose, t),
        velocity.interpolate(endValue.velocity, t),
        acceleration.interpolate(endValue.acceleration, t),
        newStates);
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
