// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.math.geometry.Twist3d;
import edu.wpi.first.math.interpolation.TimeInterpolatableBuffer;
import edu.wpi.first.math.kinematics.Kinematics;
import edu.wpi.first.math.kinematics.Odometry3d;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N4;
import edu.wpi.first.math.numbers.N6;
import java.util.NavigableMap;
import java.util.Optional;
import java.util.TreeMap;

/**
 * This class wraps {@link Odometry3d} to fuse latency-compensated vision measurements with encoder
 * measurements. Robot code should not use this directly- Instead, use the particular type for your
 * drivetrain (e.g., {@link DifferentialDrivePoseEstimator3d}). It is intended to be a drop-in
 * replacement for {@link Odometry3d}; in fact, if you never call {@link
 * PoseEstimator3d#addVisionMeasurement} and only call {@link PoseEstimator3d#update} then this will
 * behave exactly the same as Odometry3d. It is also intended to be an easy replacement for {@link
 * PoseEstimator}, only requiring the addition of a standard deviation for Z and appropriate
 * conversions between 2D and 3D versions of geometry classes. (See {@link Pose3d#Pose3d(Pose2d)},
 * {@link Rotation3d#Rotation3d(Rotation2d)}, {@link Translation3d#Translation3d(Translation2d)},
 * and {@link Pose3d#toPose2d()}.)
 *
 * <p>{@link PoseEstimator3d#update} should be called every robot loop.
 *
 * <p>{@link PoseEstimator3d#addVisionMeasurement} can be called as infrequently as you want; if you
 * never call it then this class will behave exactly like regular encoder odometry.
 *
 * @param <T> Wheel positions type.
 */
public class PoseEstimator3d<T> {
  private final Odometry3d<T> m_odometry;
  private final Matrix<N4, N1> m_q = new Matrix<>(Nat.N4(), Nat.N1());
  private final Matrix<N6, N6> m_visionK = new Matrix<>(Nat.N6(), Nat.N6());

  private static final double kBufferDuration = 1.5;
  // Maps timestamps to odometry-only pose estimates
  private final TimeInterpolatableBuffer<Pose3d> m_odometryPoseBuffer =
      TimeInterpolatableBuffer.createBuffer(kBufferDuration);
  // Maps timestamps to vision updates
  // Always contains one entry before the oldest entry in m_odometryPoseBuffer, unless there have
  // been no vision measurements after the last reset
  private final NavigableMap<Double, VisionUpdate> m_visionUpdates = new TreeMap<>();

  private Pose3d m_poseEstimate;

  /**
   * Constructs a PoseEstimator3d.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param odometry A correctly-configured odometry object for your drivetrain.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in meters, y position
   *     in meters, z position in meters, and angle in radians). Increase these numbers to trust
   *     your state estimate less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x position
   *     in meters, y position in meters, z position in meters, and angle in radians). Increase
   *     these numbers to trust the vision pose measurement less.
   */
  @SuppressWarnings("PMD.UnusedFormalParameter")
  public PoseEstimator3d(
      Kinematics<?, T> kinematics,
      Odometry3d<T> odometry,
      Matrix<N4, N1> stateStdDevs,
      Matrix<N4, N1> visionMeasurementStdDevs) {
    m_odometry = odometry;

    m_poseEstimate = m_odometry.getPoseMeters();

    for (int i = 0; i < 4; ++i) {
      m_q.set(i, 0, stateStdDevs.get(i, 0) * stateStdDevs.get(i, 0));
    }
    setVisionMeasurementStdDevs(visionMeasurementStdDevs);
  }

  /**
   * Sets the pose estimator's trust of global measurements. This might be used to change trust in
   * vision measurements after the autonomous period, or to change trust as distance to a vision
   * target increases.
   *
   * @param visionMeasurementStdDevs Standard deviations of the vision measurements. Increase these
   *     numbers to trust global measurements from vision less. This matrix is in the form [x, y, z,
   *     theta]ᵀ, with units in meters and radians.
   */
  public final void setVisionMeasurementStdDevs(Matrix<N4, N1> visionMeasurementStdDevs) {
    var r = new double[4];
    for (int i = 0; i < 4; ++i) {
      r[i] = visionMeasurementStdDevs.get(i, 0) * visionMeasurementStdDevs.get(i, 0);
    }

    // Solve for closed form Kalman gain for continuous Kalman filter with A = 0
    // and C = I. See wpimath/algorithms.md.
    for (int row = 0; row < 4; ++row) {
      if (m_q.get(row, 0) == 0.0) {
        m_visionK.set(row, row, 0.0);
      } else {
        m_visionK.set(
            row, row, m_q.get(row, 0) / (m_q.get(row, 0) + Math.sqrt(m_q.get(row, 0) * r[row])));
      }
    }
    // Fill in the gains for the other components of the rotation vector
    double angle_gain = m_visionK.get(3, 3);
    m_visionK.set(4, 4, angle_gain);
    m_visionK.set(5, 5, angle_gain);
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset here on the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The current encoder readings.
   * @param poseMeters The position on the field that your robot is at.
   */
  public void resetPosition(Rotation3d gyroAngle, T wheelPositions, Pose3d poseMeters) {
    // Reset state estimate and error covariance
    m_odometry.resetPosition(gyroAngle, wheelPositions, poseMeters);
    m_odometryPoseBuffer.clear();
    m_visionUpdates.clear();
    m_poseEstimate = m_odometry.getPoseMeters();
  }

  /**
   * Resets the robot's pose.
   *
   * @param pose The pose to reset to.
   */
  public void resetPose(Pose3d pose) {
    m_odometry.resetPose(pose);
    m_odometryPoseBuffer.clear();
    m_visionUpdates.clear();
    m_poseEstimate = m_odometry.getPoseMeters();
  }

  /**
   * Resets the robot's translation.
   *
   * @param translation The pose to translation to.
   */
  public void resetTranslation(Translation3d translation) {
    m_odometry.resetTranslation(translation);
    m_odometryPoseBuffer.clear();
    m_visionUpdates.clear();
    m_poseEstimate = m_odometry.getPoseMeters();
  }

  /**
   * Resets the robot's rotation.
   *
   * @param rotation The rotation to reset to.
   */
  public void resetRotation(Rotation3d rotation) {
    m_odometry.resetRotation(rotation);
    m_odometryPoseBuffer.clear();
    m_visionUpdates.clear();
    m_poseEstimate = m_odometry.getPoseMeters();
  }

  /**
   * Gets the estimated robot pose.
   *
   * @return The estimated robot pose in meters.
   */
  public Pose3d getEstimatedPosition() {
    return m_poseEstimate;
  }

  /**
   * Return the pose at a given timestamp, if the buffer is not empty.
   *
   * @param timestampSeconds The pose's timestamp in seconds.
   * @return The pose at the given timestamp (or Optional.empty() if the buffer is empty).
   */
  public Optional<Pose3d> sampleAt(double timestampSeconds) {
    // Step 0: If there are no odometry updates to sample, skip.
    if (m_odometryPoseBuffer.getInternalBuffer().isEmpty()) {
      return Optional.empty();
    }

    // Step 1: Make sure timestamp matches the sample from the odometry pose buffer. (When sampling,
    // the buffer will always use a timestamp between the first and last timestamps)
    double oldestOdometryTimestamp = m_odometryPoseBuffer.getInternalBuffer().firstKey();
    double newestOdometryTimestamp = m_odometryPoseBuffer.getInternalBuffer().lastKey();
    timestampSeconds =
        MathUtil.clamp(timestampSeconds, oldestOdometryTimestamp, newestOdometryTimestamp);

    // Step 2: If there are no applicable vision updates, use the odometry-only information.
    if (m_visionUpdates.isEmpty() || timestampSeconds < m_visionUpdates.firstKey()) {
      return m_odometryPoseBuffer.getSample(timestampSeconds);
    }

    // Step 3: Get the latest vision update from before or at the timestamp to sample at.
    double floorTimestamp = m_visionUpdates.floorKey(timestampSeconds);
    var visionUpdate = m_visionUpdates.get(floorTimestamp);

    // Step 4: Get the pose measured by odometry at the time of the sample.
    var odometryEstimate = m_odometryPoseBuffer.getSample(timestampSeconds);

    // Step 5: Apply the vision compensation to the odometry pose.
    return odometryEstimate.map(odometryPose -> visionUpdate.compensate(odometryPose));
  }

  /** Removes stale vision updates that won't affect sampling. */
  private void cleanUpVisionUpdates() {
    // Step 0: If there are no odometry samples, skip.
    if (m_odometryPoseBuffer.getInternalBuffer().isEmpty()) {
      return;
    }

    // Step 1: Find the oldest timestamp that needs a vision update.
    double oldestOdometryTimestamp = m_odometryPoseBuffer.getInternalBuffer().firstKey();

    // Step 2: If there are no vision updates before that timestamp, skip.
    if (m_visionUpdates.isEmpty() || oldestOdometryTimestamp < m_visionUpdates.firstKey()) {
      return;
    }

    // Step 3: Find the newest vision update timestamp before or at the oldest timestamp.
    double newestNeededVisionUpdateTimestamp = m_visionUpdates.floorKey(oldestOdometryTimestamp);

    // Step 4: Remove all entries strictly before the newest timestamp we need.
    m_visionUpdates.headMap(newestNeededVisionUpdateTimestamp, false).clear();
  }

  /**
   * Adds a vision measurement to the Kalman Filter. This will correct the odometry pose estimate
   * while still accounting for measurement noise.
   *
   * <p>This method can be called as infrequently as you want, as long as you are calling {@link
   * PoseEstimator3d#update} every loop.
   *
   * <p>To promote stability of the pose estimate and make it robust to bad vision data, we
   * recommend only adding vision measurements that are already within one meter or so of the
   * current pose estimate.
   *
   * @param visionRobotPoseMeters The pose of the robot as measured by the vision camera.
   * @param timestampSeconds The timestamp of the vision measurement in seconds. Note that if you
   *     don't use your own time source by calling {@link
   *     PoseEstimator3d#updateWithTime(double,Rotation3d,Object)} then you must use a timestamp
   *     with an epoch since FPGA startup (i.e., the epoch of this timestamp is the same epoch as
   *     {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()}.) This means that you should use
   *     {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()} as your time source or sync the
   *     epochs.
   */
  public void addVisionMeasurement(Pose3d visionRobotPoseMeters, double timestampSeconds) {
    // Step 0: If this measurement is old enough to be outside the pose buffer's timespan, skip.
    if (m_odometryPoseBuffer.getInternalBuffer().isEmpty()
        || m_odometryPoseBuffer.getInternalBuffer().lastKey() - kBufferDuration
            > timestampSeconds) {
      return;
    }

    // Step 1: Clean up any old entries
    cleanUpVisionUpdates();

    // Step 2: Get the pose measured by odometry at the moment the vision measurement was made.
    var odometrySample = m_odometryPoseBuffer.getSample(timestampSeconds);

    if (odometrySample.isEmpty()) {
      return;
    }

    // Step 3: Get the vision-compensated pose estimate at the moment the vision measurement was
    // made.
    var visionSample = sampleAt(timestampSeconds);

    if (visionSample.isEmpty()) {
      return;
    }

    // Step 4: Measure the twist between the old pose estimate and the vision pose.
    var twist = visionSample.get().log(visionRobotPoseMeters);

    // Step 5: We should not trust the twist entirely, so instead we scale this twist by a Kalman
    // gain matrix representing how much we trust vision measurements compared to our current pose.
    var k_times_twist =
        m_visionK.times(
            VecBuilder.fill(twist.dx, twist.dy, twist.dz, twist.rx, twist.ry, twist.rz));

    // Step 6: Convert back to Twist3d.
    var scaledTwist =
        new Twist3d(
            k_times_twist.get(0, 0),
            k_times_twist.get(1, 0),
            k_times_twist.get(2, 0),
            k_times_twist.get(3, 0),
            k_times_twist.get(4, 0),
            k_times_twist.get(5, 0));

    // Step 7: Calculate and record the vision update.
    var visionUpdate = new VisionUpdate(visionSample.get().exp(scaledTwist), odometrySample.get());
    m_visionUpdates.put(timestampSeconds, visionUpdate);

    // Step 8: Remove later vision measurements. (Matches previous behavior)
    m_visionUpdates.tailMap(timestampSeconds, false).entrySet().clear();

    // Step 9: Update latest pose estimate. Since we cleared all updates after this vision update,
    // it's guaranteed to be the latest vision update.
    m_poseEstimate = visionUpdate.compensate(m_odometry.getPoseMeters());
  }

  /**
   * Adds a vision measurement to the Kalman Filter. This will correct the odometry pose estimate
   * while still accounting for measurement noise.
   *
   * <p>This method can be called as infrequently as you want, as long as you are calling {@link
   * PoseEstimator3d#update} every loop.
   *
   * <p>To promote stability of the pose estimate and make it robust to bad vision data, we
   * recommend only adding vision measurements that are already within one meter or so of the
   * current pose estimate.
   *
   * <p>Note that the vision measurement standard deviations passed into this method will continue
   * to apply to future measurements until a subsequent call to {@link
   * PoseEstimator3d#setVisionMeasurementStdDevs(Matrix)} or this method.
   *
   * @param visionRobotPoseMeters The pose of the robot as measured by the vision camera.
   * @param timestampSeconds The timestamp of the vision measurement in seconds. Note that if you
   *     don't use your own time source by calling {@link #updateWithTime}, then you must use a
   *     timestamp with an epoch since FPGA startup (i.e., the epoch of this timestamp is the same
   *     epoch as {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()}). This means that you
   *     should use {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()} as your time source in
   *     this case.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x position
   *     in meters, y position in meters, z position in meters, and angle in radians). Increase
   *     these numbers to trust the vision pose measurement less.
   */
  public void addVisionMeasurement(
      Pose3d visionRobotPoseMeters,
      double timestampSeconds,
      Matrix<N4, N1> visionMeasurementStdDevs) {
    setVisionMeasurementStdDevs(visionMeasurementStdDevs);
    addVisionMeasurement(visionRobotPoseMeters, timestampSeconds);
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This should be called every
   * loop.
   *
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The current encoder readings.
   * @return The estimated pose of the robot in meters.
   */
  public Pose3d update(Rotation3d gyroAngle, T wheelPositions) {
    return updateWithTime(MathSharedStore.getTimestamp(), gyroAngle, wheelPositions);
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This should be called every
   * loop.
   *
   * @param currentTimeSeconds Time at which this method was called, in seconds.
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The current encoder readings.
   * @return The estimated pose of the robot in meters.
   */
  public Pose3d updateWithTime(double currentTimeSeconds, Rotation3d gyroAngle, T wheelPositions) {
    var odometryEstimate = m_odometry.update(gyroAngle, wheelPositions);

    m_odometryPoseBuffer.addSample(currentTimeSeconds, odometryEstimate);

    if (m_visionUpdates.isEmpty()) {
      m_poseEstimate = odometryEstimate;
    } else {
      var visionUpdate = m_visionUpdates.get(m_visionUpdates.lastKey());
      m_poseEstimate = visionUpdate.compensate(odometryEstimate);
    }

    return getEstimatedPosition();
  }

  /**
   * Represents a vision update record. The record contains the vision-compensated pose estimate as
   * well as the corresponding odometry pose estimate.
   */
  private static final class VisionUpdate {
    // The vision-compensated pose estimate.
    private final Pose3d visionPose;

    // The pose estimated based solely on odometry.
    private final Pose3d odometryPose;

    /**
     * Constructs a vision update record with the specified parameters.
     *
     * @param visionPose The vision-compensated pose estimate.
     * @param odometryPose The pose estimate based solely on odometry.
     */
    private VisionUpdate(Pose3d visionPose, Pose3d odometryPose) {
      this.visionPose = visionPose;
      this.odometryPose = odometryPose;
    }

    /**
     * Returns the vision-compensated version of the pose. Specifically, changes the pose from being
     * relative to this record's odometry pose to being relative to this record's vision pose.
     *
     * @param pose The pose to compensate.
     * @return The compensated pose.
     */
    public Pose3d compensate(Pose3d pose) {
      var delta = pose.minus(this.odometryPose);
      return this.visionPose.plus(delta);
    }
  }
}
