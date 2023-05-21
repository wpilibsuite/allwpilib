// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.kinematics.SwerveDriveKinematics;
import edu.wpi.first.math.kinematics.SwerveDriveOdometry;
import edu.wpi.first.math.kinematics.SwerveModulePosition;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N3;
import java.util.Arrays;
import java.util.Map.Entry;
import java.util.Objects;

/**
 * This class wraps {@link SwerveDriveOdometry Swerve Drive Odometry} to fuse latency-compensated
 * vision measurements with swerve drive encoder distance measurements. It is intended to be a
 * drop-in replacement for {@link edu.wpi.first.math.kinematics.SwerveDriveOdometry}.
 *
 * <p>{@link SwerveDrivePoseEstimator#update} should be called every robot loop.
 *
 * <p>{@link SwerveDrivePoseEstimator#addVisionMeasurement} can be called as infrequently as you
 * want; if you never call it, then this class will behave as regular encoder odometry.
 */
public class SwerveDrivePoseEstimator extends PoseEstimator {
  private final SwerveDriveKinematics m_kinematics;
  private final SwerveDriveOdometry m_odometry;
  private final int m_numModules;

  private static final double kBufferDuration = 1.5;

  /**
   * Constructs a SwerveDrivePoseEstimator with default standard deviations for the model and vision
   * measurements.
   *
   * <p>The default standard deviations of the model states are 0.1 meters for x, 0.1 meters for y,
   * and 0.1 radians for heading. The default standard deviations of the vision measurements are 0.9
   * meters for x, 0.9 meters for y, and 0.9 radians for heading.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param modulePositions The current distance measurements and rotations of the swerve modules.
   * @param initialPoseMeters The starting pose estimate.
   */
  public SwerveDrivePoseEstimator(
      SwerveDriveKinematics kinematics,
      Rotation2d gyroAngle,
      SwerveModulePosition[] modulePositions,
      Pose2d initialPoseMeters) {
    this(
        kinematics,
        gyroAngle,
        modulePositions,
        initialPoseMeters,
        VecBuilder.fill(0.1, 0.1, 0.1),
        VecBuilder.fill(0.9, 0.9, 0.9));
  }

  /**
   * Constructs a SwerveDrivePoseEstimator.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param modulePositions The current distance and rotation measurements of the swerve modules.
   * @param initialPoseMeters The starting pose estimate.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in meters, y position
   *     in meters, and heading in radians). Increase these numbers to trust your state estimate
   *     less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x position
   *     in meters, y position in meters, and heading in radians). Increase these numbers to trust
   *     the vision pose measurement less.
   */
  public SwerveDrivePoseEstimator(
      SwerveDriveKinematics kinematics,
      Rotation2d gyroAngle,
      SwerveModulePosition[] modulePositions,
      Pose2d initialPoseMeters,
      Matrix<N3, N1> stateStdDevs,
      Matrix<N3, N1> visionMeasurementStdDevs) {
    m_kinematics = kinematics;
    m_odometry = new SwerveDriveOdometry(kinematics, gyroAngle, modulePositions, initialPoseMeters);

    for (int i = 0; i < 3; ++i) {
      m_q.set(i, 0, stateStdDevs.get(i, 0) * stateStdDevs.get(i, 0));
    }

    m_numModules = modulePositions.length;

    setVisionMeasurementStdDevs(visionMeasurementStdDevs);
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset in the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param modulePositions The current distance measurements and rotations of the swerve modules.
   * @param poseMeters The position on the field that your robot is at.
   */
  public void resetPosition(
      Rotation2d gyroAngle, SwerveModulePosition[] modulePositions, Pose2d poseMeters) {
    // Reset state estimate and error covariance
    m_odometry.resetPosition(gyroAngle, modulePositions, poseMeters);
    m_poseBuffer.clear();
  }

  /**
   * Gets the estimated robot pose.
   *
   * @return The estimated robot pose in meters.
   */
  public Pose2d getEstimatedPosition() {
    return m_odometry.getPoseMeters();
  }

  @Override
  protected void resetOdometry(BaseInterpolationRecord sample, Twist2d scaledTwist) {
    InterpolationRecord pose = (InterpolationRecord) sample;
    m_odometry.resetPosition(
        pose.gyroAngle, pose.modulePositions, pose.poseMeters.exp(scaledTwist));
  }

  @Override
  protected void recordCurrentPose(BaseInterpolationRecord sample, double timestampSeconds) {
    InterpolationRecord pose = (InterpolationRecord) sample;
    m_poseBuffer.addSample(
        timestampSeconds,
        new InterpolationRecord(getEstimatedPosition(), pose.gyroAngle, pose.modulePositions));
  }

  @Override
  protected void replayOdometryInputs(Entry<Double, BaseInterpolationRecord> entry) {
    updateWithTime(
        entry.getKey(),
        entry.getValue().gyroAngle,
        ((InterpolationRecord) entry.getValue()).modulePositions);
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This should be called every
   * loop.
   *
   * @param gyroAngle The current gyro angle.
   * @param modulePositions The current distance measurements and rotations of the swerve modules.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d update(Rotation2d gyroAngle, SwerveModulePosition[] modulePositions) {
    return updateWithTime(MathSharedStore.getTimestamp(), gyroAngle, modulePositions);
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This should be called every
   * loop.
   *
   * @param currentTimeSeconds Time at which this method was called, in seconds.
   * @param gyroAngle The current gyroscope angle.
   * @param modulePositions The current distance measurements and rotations of the swerve modules.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d updateWithTime(
      double currentTimeSeconds, Rotation2d gyroAngle, SwerveModulePosition[] modulePositions) {
    if (modulePositions.length != m_numModules) {
      throw new IllegalArgumentException(
          "Number of modules is not consistent with number of wheel locations provided in "
              + "constructor");
    }

    var internalModulePositions = new SwerveModulePosition[m_numModules];

    for (int i = 0; i < m_numModules; i++) {
      internalModulePositions[i] =
          new SwerveModulePosition(modulePositions[i].distanceMeters, modulePositions[i].angle);
    }

    m_odometry.update(gyroAngle, internalModulePositions);

    m_poseBuffer.addSample(
        currentTimeSeconds,
        new InterpolationRecord(getEstimatedPosition(), gyroAngle, internalModulePositions));

    return getEstimatedPosition();
  }

  /**
   * Represents an odometry record. The record contains the inputs provided as well as the pose that
   * was observed based on these inputs, as well as the previous record and its inputs.
   */
  private class InterpolationRecord extends BaseInterpolationRecord {
    // The pose observed given the current sensor inputs and the previous pose.
    private final Pose2d poseMeters;

    // The current gyro angle.
    private final Rotation2d gyroAngle;

    // The distances and rotations measured at each module.
    private final SwerveModulePosition[] modulePositions;

    /**
     * Constructs an Interpolation Record with the specified parameters.
     *
     * @param poseMeters The pose observed given the current sensor inputs and the previous pose.
     * @param gyro The current gyro angle.
     * @param modulePositions The distances and rotations measured at each wheel.
     */
    private InterpolationRecord(
        Pose2d poseMeters, Rotation2d gyro, SwerveModulePosition[] modulePositions) {
      super(poseMeters, gyro);
      this.poseMeters = poseMeters;
      this.gyroAngle = gyro;
      this.modulePositions = modulePositions;
    }

    /**
     * Return the interpolated record. This object is assumed to be the starting position, or lower
     * bound.
     *
     * @param endVal The upper bound, or end.
     * @param t How far between the lower and upper bound we are. This should be bounded in [0, 1].
     * @return The interpolated value.
     */
    @Override
    public InterpolationRecord interpolate(BaseInterpolationRecord endValue, double t) {
      InterpolationRecord endVal = (InterpolationRecord) endValue;
      if (t < 0) {
        return this;
      } else if (t >= 1) {
        return endVal;
      } else {
        // Find the new wheel distances.
        var modulePositions = new SwerveModulePosition[m_numModules];

        // Find the distance travelled between this measurement and the interpolated measurement.
        var moduleDeltas = new SwerveModulePosition[m_numModules];

        for (int i = 0; i < m_numModules; i++) {
          double ds =
              MathUtil.interpolate(
                  this.modulePositions[i].distanceMeters,
                  endVal.modulePositions[i].distanceMeters,
                  t);
          Rotation2d theta =
              this.modulePositions[i].angle.interpolate(endVal.modulePositions[i].angle, t);
          modulePositions[i] = new SwerveModulePosition(ds, theta);
          moduleDeltas[i] =
              new SwerveModulePosition(ds - this.modulePositions[i].distanceMeters, theta);
        }

        // Find the new gyro angle.
        var gyro_lerp = gyroAngle.interpolate(endVal.gyroAngle, t);

        // Create a twist to represent this change based on the interpolated sensor inputs.
        Twist2d twist = m_kinematics.toTwist2d(moduleDeltas);
        twist.dtheta = gyro_lerp.minus(gyroAngle).getRadians();

        return new InterpolationRecord(poseMeters.exp(twist), gyro_lerp, modulePositions);
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
          && Arrays.equals(modulePositions, record.modulePositions)
          && Objects.equals(poseMeters, record.poseMeters);
    }

    @Override
    public int hashCode() {
      return Objects.hash(gyroAngle, Arrays.hashCode(modulePositions), poseMeters);
    }
  }
}
