package edu.wpi.first.math.trajectory;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.kinematics.ChassisAccelerations;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;
import edu.wpi.first.math.kinematics.MecanumDriveWheelSpeeds;
import edu.wpi.first.units.measure.Time;

public class MecanumSample extends TrajectorySample<MecanumSample> {
  private final MecanumDriveKinematics kinematics;

  /** The speeds of the wheels in the robot's reference frame, in meters per second. */
  public final MecanumDriveWheelSpeeds speeds;

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
    super(timestamp, pose, vel, accel);
    this.kinematics = kinematics;

    speeds = kinematics.toWheelSpeeds(vel);
  }

  /**
   * Constructs a MecanumSample from another TrajectorySample.
   *
   * @param sample The TrajectorySample to copy.
   * @param kinematics The kinematics of the drivetrain.
   */
  public MecanumSample(TrajectorySample<?> sample, MecanumDriveKinematics kinematics) {
    this(sample.timestamp, sample.pose, sample.vel, sample.accel, kinematics);
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
  public MecanumSample interpolate(MecanumSample endValue, double t) {
    return new MecanumSample(TrajectorySample.kinematicInterpolate(this, endValue, t), kinematics);
  }
}
