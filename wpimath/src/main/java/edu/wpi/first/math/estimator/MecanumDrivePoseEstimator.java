// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.estimator.MecanumDrivePoseEstimator.InterpolationRecord;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;
import edu.wpi.first.math.kinematics.MecanumDriveOdometry;
import edu.wpi.first.math.kinematics.MecanumDriveWheelPositions;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N3;
import java.util.Objects;

/**
 * This class wraps {@link MecanumDriveOdometry Mecanum Drive Odometry} to fuse latency-compensated
 * vision measurements with mecanum drive encoder distance measurements. It will correct for noisy
 * measurements and encoder drift. It is intended to be a drop-in replacement for {@link
 * edu.wpi.first.math.kinematics.MecanumDriveOdometry}.
 *
 * <p>{@link MecanumDrivePoseEstimator#update} should be called every robot loop.
 *
 * <p>{@link MecanumDrivePoseEstimator#addVisionMeasurement} can be called as infrequently as you
 * want; if you never call it, then this class will behave mostly like regular encoder odometry.
 */
public class MecanumDrivePoseEstimator extends PoseEstimator<InterpolationRecord> {
  private final MecanumDriveKinematics m_kinematics;
  private final MecanumDriveOdometry m_odometry;

  /**
   * Constructs a MecanumDrivePoseEstimator with default standard deviations for the model and
   * vision measurements.
   *
   * <p>The default standard deviations of the model states are 0.1 meters for x, 0.1 meters for y,
   * and 0.1 radians for heading. The default standard deviations of the vision measurements are
   * 0.45 meters for x, 0.45 meters for y, and 0.45 radians for heading.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The distances driven by each wheel.
   * @param initialPoseMeters The starting pose estimate.
   */
  public MecanumDrivePoseEstimator(
      MecanumDriveKinematics kinematics,
      Rotation2d gyroAngle,
      MecanumDriveWheelPositions wheelPositions,
      Pose2d initialPoseMeters) {
    this(
        kinematics,
        gyroAngle,
        wheelPositions,
        initialPoseMeters,
        VecBuilder.fill(0.1, 0.1, 0.1),
        VecBuilder.fill(0.45, 0.45, 0.45));
  }

  /**
   * Constructs a MecanumDrivePoseEstimator.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The distance measured by each wheel.
   * @param initialPoseMeters The starting pose estimate.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in meters, y position
   *     in meters, and heading in radians). Increase these numbers to trust your state estimate
   *     less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x position
   *     in meters, y position in meters, and heading in radians). Increase these numbers to trust
   *     the vision pose measurement less.
   */
  public MecanumDrivePoseEstimator(
      MecanumDriveKinematics kinematics,
      Rotation2d gyroAngle,
      MecanumDriveWheelPositions wheelPositions,
      Pose2d initialPoseMeters,
      Matrix<N3, N1> stateStdDevs,
      Matrix<N3, N1> visionMeasurementStdDevs) {
    m_kinematics = kinematics;
    m_odometry = new MecanumDriveOdometry(kinematics, gyroAngle, wheelPositions, initialPoseMeters);

    for (int i = 0; i < 3; ++i) {
      m_q.set(i, 0, stateStdDevs.get(i, 0) * stateStdDevs.get(i, 0));
    }

    // Initialize vision R
    setVisionMeasurementStdDevs(visionMeasurementStdDevs);
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset in the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The distances driven by each wheel.
   * @param poseMeters The position on the field that your robot is at.
   */
  public void resetPosition(
      Rotation2d gyroAngle, MecanumDriveWheelPositions wheelPositions, Pose2d poseMeters) {
    // Reset state estimate and error covariance
    m_odometry.resetPosition(gyroAngle, wheelPositions, poseMeters);
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
        sample.gyroAngle, sample.wheelPositions, sample.poseMeters.exp(scaledTwist));
  }

  @Override
  protected void recordCurrentPose(InterpolationRecord sample, double timestampSeconds) {
    m_poseBuffer.addSample(
        timestampSeconds,
        new InterpolationRecord(getEstimatedPosition(), sample.gyroAngle, sample.wheelPositions));
  }

  @Override
  protected void replayOdometryInput(double timestampSeconds, InterpolationRecord sample) {
    updateWithTime(timestampSeconds, sample.gyroAngle, sample.wheelPositions);
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This should be called every
   * loop.
   *
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The distances driven by each wheel.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d update(Rotation2d gyroAngle, MecanumDriveWheelPositions wheelPositions) {
    return updateWithTime(MathSharedStore.getTimestamp(), gyroAngle, wheelPositions);
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This should be called every
   * loop.
   *
   * @param currentTimeSeconds Time at which this method was called, in seconds.
   * @param gyroAngle The current gyroscope angle.
   * @param wheelPositions The distances driven by each wheel.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d updateWithTime(
      double currentTimeSeconds, Rotation2d gyroAngle, MecanumDriveWheelPositions wheelPositions) {
    m_odometry.update(gyroAngle, wheelPositions);

    m_poseBuffer.addSample(
        currentTimeSeconds,
        new InterpolationRecord(
            getEstimatedPosition(),
            gyroAngle,
            new MecanumDriveWheelPositions(
                wheelPositions.frontLeftMeters,
                wheelPositions.frontRightMeters,
                wheelPositions.rearLeftMeters,
                wheelPositions.rearRightMeters)));

    return getEstimatedPosition();
  }

  /**
   * Represents an odometry record. The record contains the inputs provided as well as the pose that
   * was observed based on these inputs, as well as the previous record and its inputs.
   */
  class InterpolationRecord extends BaseInterpolationRecord<InterpolationRecord> {
    // The distances traveled by each wheel encoder.
    private final MecanumDriveWheelPositions wheelPositions;

    /**
     * Constructs an Interpolation Record with the specified parameters.
     *
     * @param poseMeters The pose observed given the current sensor inputs and the previous pose.
     * @param gyro The current gyro angle.
     * @param wheelPositions The distances traveled by each wheel encoder.
     */
    private InterpolationRecord(
        Pose2d poseMeters, Rotation2d gyro, MecanumDriveWheelPositions wheelPositions) {
      super(poseMeters, gyro);
      this.wheelPositions = wheelPositions;
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
        // Find the new wheel distances.
        var wheels_lerp =
            new MecanumDriveWheelPositions(
                MathUtil.interpolate(
                    this.wheelPositions.frontLeftMeters,
                    endValue.wheelPositions.frontLeftMeters,
                    t),
                MathUtil.interpolate(
                    this.wheelPositions.frontRightMeters,
                    endValue.wheelPositions.frontRightMeters,
                    t),
                MathUtil.interpolate(
                    this.wheelPositions.rearLeftMeters, endValue.wheelPositions.rearLeftMeters, t),
                MathUtil.interpolate(
                    this.wheelPositions.rearRightMeters,
                    endValue.wheelPositions.rearRightMeters,
                    t));

        // Find the distance travelled between this measurement and the interpolated measurement.
        var wheels_delta =
            new MecanumDriveWheelPositions(
                wheels_lerp.frontLeftMeters - this.wheelPositions.frontLeftMeters,
                wheels_lerp.frontRightMeters - this.wheelPositions.frontRightMeters,
                wheels_lerp.rearLeftMeters - this.wheelPositions.rearLeftMeters,
                wheels_lerp.rearRightMeters - this.wheelPositions.rearRightMeters);

        // Find the new gyro angle.
        var gyro_lerp = gyroAngle.interpolate(endValue.gyroAngle, t);

        // Create a twist to represent this change based on the interpolated sensor inputs.
        Twist2d twist = m_kinematics.toTwist2d(wheels_delta);
        twist.dtheta = gyro_lerp.minus(gyroAngle).getRadians();

        return new InterpolationRecord(poseMeters.exp(twist), gyro_lerp, wheels_lerp);
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
          && Objects.equals(wheelPositions, record.wheelPositions)
          && Objects.equals(poseMeters, record.poseMeters);
    }

    @Override
    public int hashCode() {
      return Objects.hash(gyroAngle, wheelPositions, poseMeters);
    }
  }
}
