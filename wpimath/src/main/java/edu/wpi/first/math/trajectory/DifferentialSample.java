package edu.wpi.first.math.trajectory;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.interpolation.Interpolatable;
import edu.wpi.first.math.kinematics.ChassisAccelerations;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.units.measure.Time;

/**
 * Represents a single sample in a differential drive trajectory.
 */
public class DifferentialSample extends TrajectorySample
    implements Interpolatable<TrajectorySample> {
  private final DifferentialDriveKinematics kinematics;
  public final double leftSpeed;
  public final double rightSpeed;

  /**
   * Constructs a DifferentialSample.
   *
   * @param timestamp The timestamp of the sample relative to the trajectory start.
   * @param pose The robot pose at this sample (in the field reference frame).
   * @param vel The robot velocity at this sample (in the robot's reference frame).
   * @param accel The robot acceleration at this sample (in the robot's reference frame).
   * @param kinematics The kinematics of the drivetrain.
   */
  public DifferentialSample(
      Time timestamp,
      Pose2d pose,
      ChassisSpeeds vel,
      ChassisAccelerations accel,
      DifferentialDriveKinematics kinematics) {
    super(timestamp, pose, vel, accel);

    this.kinematics = kinematics;

    var wheelSpeeds = this.kinematics.toWheelSpeeds(vel);
    this.leftSpeed = wheelSpeeds.left;
    this.rightSpeed = wheelSpeeds.right;
  }

  public DifferentialSample(TrajectorySample sample, DifferentialDriveKinematics kinematics) {
    this(sample.timestamp, sample.pose, sample.vel, sample.accel, kinematics);
  }

  @Override
  public DifferentialSample interpolate(TrajectorySample end, double t) {
    return new DifferentialSample(super.interpolate(this, t), kinematics);
  }
}
