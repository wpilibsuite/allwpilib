// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.estimator.DifferentialDrivePoseEstimator.InterpolationRecord;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.math.kinematics.DifferentialDriveOdometry;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N3;
import java.util.Objects;

/**
 * This class wraps {@link DifferentialDriveOdometry Differential Drive Odometry} to fuse
 * latency-compensated vision measurements with differential drive encoder measurements. It is
 * intended to be a drop-in replacement for {@link DifferentialDriveOdometry}; in fact, if you never
 * call {@link DifferentialDrivePoseEstimator#addVisionMeasurement} and only call {@link
 * DifferentialDrivePoseEstimator#update} then this will behave exactly the same as
 * DifferentialDriveOdometry.
 *
 * <p>{@link DifferentialDrivePoseEstimator#update} should be called every robot loop.
 *
 * <p>{@link DifferentialDrivePoseEstimator#addVisionMeasurement} can be called as infrequently as
 * you want; if you never call it then this class will behave exactly like regular encoder odometry.
 */
public class DifferentialDrivePoseEstimator extends PoseEstimator<InterpolationRecord> {
  private final DifferentialDriveKinematics m_kinematics;
  private final DifferentialDriveOdometry m_odometry;

  /**
   * Constructs a DifferentialDrivePoseEstimator with default standard deviations for the model and
   * vision measurements.
   *
   * <p>The default standard deviations of the model states are 0.02 meters for x, 0.02 meters for
   * y, and 0.01 radians for heading. The default standard deviations of the vision measurements are
   * 0.1 meters for x, 0.1 meters for y, and 0.1 radians for heading.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param leftDistanceMeters The distance traveled by the left encoder.
   * @param rightDistanceMeters The distance traveled by the right encoder.
   * @param initialPoseMeters The starting pose estimate.
   */
  public DifferentialDrivePoseEstimator(
      DifferentialDriveKinematics kinematics,
      Rotation2d gyroAngle,
      double leftDistanceMeters,
      double rightDistanceMeters,
      Pose2d initialPoseMeters) {
    this(
        kinematics,
        gyroAngle,
        leftDistanceMeters,
        rightDistanceMeters,
        initialPoseMeters,
        VecBuilder.fill(0.02, 0.02, 0.01),
        VecBuilder.fill(0.1, 0.1, 0.1));
  }

  /**
   * Constructs a DifferentialDrivePoseEstimator.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param gyroAngle The gyro angle of the robot.
   * @param leftDistanceMeters The distance traveled by the left encoder.
   * @param rightDistanceMeters The distance traveled by the right encoder.
   * @param initialPoseMeters The estimated initial pose.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in meters, y position
   *     in meters, and heading in radians). Increase these numbers to trust your state estimate
   *     less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x position
   *     in meters, y position in meters, and heading in radians). Increase these numbers to trust
   *     the vision pose measurement less.
   */
  public DifferentialDrivePoseEstimator(
      DifferentialDriveKinematics kinematics,
      Rotation2d gyroAngle,
      double leftDistanceMeters,
      double rightDistanceMeters,
      Pose2d initialPoseMeters,
      Matrix<N3, N1> stateStdDevs,
      Matrix<N3, N1> visionMeasurementStdDevs) {
    m_kinematics = kinematics;
    m_odometry =
        new DifferentialDriveOdometry(
            gyroAngle, leftDistanceMeters, rightDistanceMeters, initialPoseMeters);

    for (int i = 0; i < 3; ++i) {
      m_q.set(i, 0, stateStdDevs.get(i, 0) * stateStdDevs.get(i, 0));
    }

    // Initialize vision R
    setVisionMeasurementStdDevs(visionMeasurementStdDevs);
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset here on the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param leftPositionMeters The distance traveled by the left encoder.
   * @param rightPositionMeters The distance traveled by the right encoder.
   * @param poseMeters The position on the field that your robot is at.
   */
  public void resetPosition(
      Rotation2d gyroAngle,
      double leftPositionMeters,
      double rightPositionMeters,
      Pose2d poseMeters) {
    // Reset state estimate and error covariance
    m_odometry.resetPosition(gyroAngle, leftPositionMeters, rightPositionMeters, poseMeters);
    m_poseBuffer.clear();
  }

  /**
   * Gets the estimated robot pose.
   *
   * @return The estimated robot pose in meters.
   */
  @Override
  public Pose2d getEstimatedPosition() {
    return m_odometry.getPoseMeters();
  }

  @Override
  protected void resetOdometry(InterpolationRecord sample, Twist2d scaledTwist) {
    m_odometry.resetPosition(
        sample.gyroAngle,
        sample.leftMeters,
        sample.rightMeters,
        sample.poseMeters.exp(scaledTwist));
  }

  @Override
  protected void recordCurrentPose(InterpolationRecord sample, double timestampSeconds) {
    m_poseBuffer.addSample(
        timestampSeconds,
        new InterpolationRecord(
            getEstimatedPosition(), sample.gyroAngle, sample.leftMeters, sample.rightMeters));
  }

  @Override
  protected void replayOdometryInput(double timestampSeconds, InterpolationRecord sample) {
    updateWithTime(timestampSeconds, sample.gyroAngle, sample.leftMeters, sample.rightMeters);
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This should be called every
   * loop.
   *
   * @param gyroAngle The current gyro angle.
   * @param distanceLeftMeters The total distance travelled by the left wheel in meters.
   * @param distanceRightMeters The total distance travelled by the right wheel in meters.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d update(
      Rotation2d gyroAngle, double distanceLeftMeters, double distanceRightMeters) {
    return updateWithTime(
        MathSharedStore.getTimestamp(), gyroAngle, distanceLeftMeters, distanceRightMeters);
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This should be called every
   * loop.
   *
   * @param currentTimeSeconds Time at which this method was called, in seconds.
   * @param gyroAngle The current gyro angle.
   * @param distanceLeftMeters The total distance travelled by the left wheel in meters.
   * @param distanceRightMeters The total distance travelled by the right wheel in meters.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d updateWithTime(
      double currentTimeSeconds,
      Rotation2d gyroAngle,
      double distanceLeftMeters,
      double distanceRightMeters) {
    m_odometry.update(gyroAngle, distanceLeftMeters, distanceRightMeters);
    m_poseBuffer.addSample(
        currentTimeSeconds,
        new InterpolationRecord(
            getEstimatedPosition(), gyroAngle, distanceLeftMeters, distanceRightMeters));

    return getEstimatedPosition();
  }

  /**
   * Represents an odometry record. The record contains the inputs provided as well as the pose that
   * was observed based on these inputs, as well as the previous record and its inputs.
   */
  class InterpolationRecord extends BaseInterpolationRecord<InterpolationRecord> {
    // The distance traveled by the left encoder.
    private final double leftMeters;

    // The distance traveled by the right encoder.
    private final double rightMeters;

    /**
     * Constructs an Interpolation Record with the specified parameters.
     *
     * @param poseMeters The pose observed given the current sensor inputs and the previous pose.
     * @param gyro The current gyro angle.
     * @param leftMeters The distance traveled by the left encoder.
     * @param rightMeters The distanced traveled by the right encoder.
     */
    private InterpolationRecord(
        Pose2d poseMeters, Rotation2d gyro, double leftMeters, double rightMeters) {
      super(poseMeters, gyro);
      this.leftMeters = leftMeters;
      this.rightMeters = rightMeters;
    }

    /**
     * Return the interpolated record. This object is assumed to be the starting position, or lower
     * bound.
     *
     * @param endValue The upper bound, or end.
     * @param t How far between the lower and upper bound we are. This should be bounded in [0, 1].
     * @return The interpolated value.
     */
    @Override
    public InterpolationRecord interpolate(InterpolationRecord endValue, double t) {
      if (t < 0) {
        return this;
      } else if (t >= 1) {
        return endValue;
      } else {
        // Find the new left distance.
        var left_lerp = MathUtil.interpolate(this.leftMeters, endValue.leftMeters, t);

        // Find the new right distance.
        var right_lerp = MathUtil.interpolate(this.rightMeters, endValue.rightMeters, t);

        // Find the new gyro angle.
        var gyro_lerp = gyroAngle.interpolate(endValue.gyroAngle, t);

        // Create a twist to represent this change based on the interpolated sensor inputs.
        Twist2d twist = m_kinematics.toTwist2d(left_lerp - leftMeters, right_lerp - rightMeters);
        twist.dtheta = gyro_lerp.minus(gyroAngle).getRadians();

        return new InterpolationRecord(poseMeters.exp(twist), gyro_lerp, left_lerp, right_lerp);
      }
    }

    @Override
    public boolean equals(Object obj) {
      if (this == obj) {
        return true;
      }
      if (!(obj instanceof InterpolationRecord)) {
        return false;
      }
      InterpolationRecord record = (InterpolationRecord) obj;
      return Objects.equals(gyroAngle, record.gyroAngle)
          && Double.compare(leftMeters, record.leftMeters) == 0
          && Double.compare(rightMeters, record.rightMeters) == 0
          && Objects.equals(poseMeters, record.poseMeters);
    }

    @Override
    public int hashCode() {
      return Objects.hash(gyroAngle, leftMeters, rightMeters, poseMeters);
    }
  }
}
