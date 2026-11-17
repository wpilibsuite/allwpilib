// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.estimator;

import java.util.NavigableMap;
import java.util.Optional;
import java.util.TreeMap;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.geometry.Transform3d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.geometry.Translation3d;
import org.wpilib.math.interpolation.TimeInterpolatableBuffer;
import org.wpilib.math.kinematics.TwoDeadWheelOdometry3d;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N4;
import org.wpilib.math.util.MathSharedStore;

/**
 * This class wraps {@link TwoDeadWheelOdometry3d Two Dead Wheel Odometry} to fuse
 * latency-compensated vision measurements with two dead wheel encoder distance measurements. It is
 * intended to be a drop-in replacement for {@link TwoDeadWheelOdometry3d}. It is also intended to
 * be an easy replacement for {@link TwoDeadWheelPoseEstimator}, only requiring the addition of a
 * standard deviation for Z and appropriate conversions between 2D and 3D versions of geometry
 * classes. (See {@link Pose3d#Pose3d(Pose2d)}, {@link Rotation3d#Rotation3d(Rotation2d)}, {@link
 * Translation3d#Translation3d(Translation2d)}, and {@link Pose3d#toPose2d()}.)
 *
 * <p>{@link TwoDeadWheelPoseEstimator3d#update} should be called every robot loop.
 *
 * <p>{@link TwoDeadWheelPoseEstimator3d#addVisionMeasurement} can be called as infrequently as you
 * want; if you never call it, then this class will behave as regular encoder odometry.
 */
public class TwoDeadWheelPoseEstimator3d {
  /*
   * This class uses logic cloned from PoseEstimator3d.java. It does not extend PoseEstimator3d
   * because the structure of the kinematics and odometry classes assume that the gyro angle is
   * independent from the kinematics, while in a two-dead-wheel configuration, the gyro is
   * required to do the kinematics, as otherwise the forward kinematics is rank-deficient.
   */

  private final TwoDeadWheelOdometry3d m_odometry;

  // Diagonal of process noise covariance matrix Q
  private final double[] m_q = new double[] {0.0, 0.0, 0.0, 0.0};

  // Diagonal of Kalman gain matrix K
  private final double[] m_vision_k = new double[] {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  private static final double kBufferDuration = 1.5;
  // Maps timestamps to odometry-only pose estimates
  private final TimeInterpolatableBuffer<Pose3d> m_odometryPoseBuffer =
      TimeInterpolatableBuffer.createBuffer(kBufferDuration);
  // Maps timestamps to vision updates
  // Always contains one entry before the oldest entry in m_odometryPoseBuffer, unless there have
  // been no vision measurements after the last reset. May contain one entry while
  // m_odometryPoseBuffer is empty to correct for translation/rotation after a call to
  // ResetRotation/ResetTranslation.
  private final NavigableMap<Double, VisionUpdate> m_visionUpdates = new TreeMap<>();

  private Pose3d m_poseEstimate;

  /**
   * Constructs a TwoDeadWheelPoseEstimator3d.
   *
   * <p>The default standard deviations of the model states are 0.1 meters for x, 0.1 meters for y,
   * 0.1 meters for z, and 0.1 radians for angle. The default standard deviations of the vision
   * measurements are 0.9 meters for x, 0.9 meters for y, 0.9 meters for z, and 0.9 radians for
   * angle.
   *
   * @param xWheelYPos The y-position of the forward-facing wheel relative to the center of the
   *     robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the center of the robot
   *     in meters.
   * @param xWheelPos The distance traveled by the forward-facing wheel, in meters.
   * @param yWheelPos The distance traveled by the left-facing wheel, in meters.
   * @param gyroAngle The current gyro angle.
   * @param initialPose The starting pose estimate.
   */
  public TwoDeadWheelPoseEstimator3d(
      double xWheelYPos,
      double yWheelXPos,
      double xWheelPos,
      double yWheelPos,
      Rotation3d gyroAngle,
      Pose3d initialPose) {
    this(
        xWheelYPos,
        yWheelXPos,
        xWheelPos,
        yWheelPos,
        gyroAngle,
        initialPose,
        VecBuilder.fill(0.1, 0.1, 0.1, 0.1),
        VecBuilder.fill(0.9, 0.9, 0.9, 0.9));
  }

  /**
   * Constructs a TwoDeadWheelPoseEstimator3d.
   *
   * @param xWheelYPos The y-position of the forward-facing wheel relative to the center of the
   *     robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the center of the robot
   *     in meters.
   * @param xWheelPos The distance traveled by the forward-facing wheel, in meters.
   * @param yWheelPos The distance traveled by the left-facing wheel, in meters.
   * @param gyroAngle The current gyro angle.
   * @param initialPose The starting pose estimate.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in meters, y position
   *     in meters, and heading in radians). Increase these numbers to trust your state estimate
   *     less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x position
   *     in meters, y position in meters, and heading in radians). Increase these numbers to trust
   *     the vision pose measurement less.
   */
  public TwoDeadWheelPoseEstimator3d(
      double xWheelYPos,
      double yWheelXPos,
      double xWheelPos,
      double yWheelPos,
      Rotation3d gyroAngle,
      Pose3d initialPose,
      Matrix<N4, N1> stateStdDevs,
      Matrix<N4, N1> visionMeasurementStdDevs) {
    m_odometry =
        new TwoDeadWheelOdometry3d(
            xWheelYPos, yWheelXPos, xWheelPos, yWheelPos, gyroAngle, initialPose);

    m_poseEstimate = m_odometry.getPose();

    for (int i = 0; i < 4; ++i) {
      m_q[i] = stateStdDevs.get(i, 0) * stateStdDevs.get(i, 0);
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
    // Diagonal of measurement covariance matrix R
    var r = new double[4];
    for (int i = 0; i < 4; ++i) {
      r[i] = visionMeasurementStdDevs.get(i, 0) * visionMeasurementStdDevs.get(i, 0);
    }

    // Solve for closed form Kalman gain for continuous Kalman filter with A = 0
    // and C = I. See wpimath/algorithms.md.
    for (int row = 0; row < 4; ++row) {
      if (m_q[row] == 0.0) {
        m_vision_k[row] = 0.0;
      } else {
        m_vision_k[row] = m_q[row] / (m_q[row] + Math.sqrt(m_q[row] * r[row]));
      }
    }
    // Fill in the gains for the other components of the rotation vector
    double angle_gain = m_vision_k[3];
    m_vision_k[4] = angle_gain;
    m_vision_k[5] = angle_gain;
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>The gyroscope angle does not need to be reset here on the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * @param xWheelPos The distance traveled by the forward-facing wheel, in meters.
   * @param yWheelPos The distance traveled by the left-facing wheel, in meters.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param pose The position on the field that your robot is at.
   */
  public void resetPosition(double xWheelPos, double yWheelPos, Rotation3d gyroAngle, Pose3d pose) {
    // Reset state estimate and error covariance
    m_odometry.resetPosition(xWheelPos, yWheelPos, gyroAngle, pose);
    m_odometryPoseBuffer.clear();
    m_visionUpdates.clear();
    m_poseEstimate = m_odometry.getPose();
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
    m_poseEstimate = m_odometry.getPose();
  }

  /**
   * Resets the robot's translation.
   *
   * @param translation The pose to translation to.
   */
  public void resetTranslation(Translation3d translation) {
    m_odometry.resetTranslation(translation);

    final var latestVisionUpdate = m_visionUpdates.lastEntry();
    m_odometryPoseBuffer.clear();
    m_visionUpdates.clear();

    if (latestVisionUpdate != null) {
      // apply vision compensation to the pose rotation
      final var visionUpdate =
          new VisionUpdate(
              new Pose3d(translation, latestVisionUpdate.getValue().visionPose.getRotation()),
              new Pose3d(translation, latestVisionUpdate.getValue().odometryPose.getRotation()));
      m_visionUpdates.put(latestVisionUpdate.getKey(), visionUpdate);
      m_poseEstimate = visionUpdate.compensate(m_odometry.getPose());
    } else {
      m_poseEstimate = m_odometry.getPose();
    }
  }

  /**
   * Resets the robot's rotation.
   *
   * @param rotation The rotation to reset to.
   */
  public void resetRotation(Rotation3d rotation) {
    m_odometry.resetRotation(rotation);

    final var latestVisionUpdate = m_visionUpdates.lastEntry();
    m_odometryPoseBuffer.clear();
    m_visionUpdates.clear();

    if (latestVisionUpdate != null) {
      // apply vision compensation to the pose translation
      final var visionUpdate =
          new VisionUpdate(
              new Pose3d(latestVisionUpdate.getValue().visionPose.getTranslation(), rotation),
              new Pose3d(latestVisionUpdate.getValue().odometryPose.getTranslation(), rotation));
      m_visionUpdates.put(latestVisionUpdate.getKey(), visionUpdate);
      m_poseEstimate = visionUpdate.compensate(m_odometry.getPose());
    } else {
      m_poseEstimate = m_odometry.getPose();
    }
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
   * @param timestamp The pose's timestamp in seconds.
   * @return The pose at the given timestamp (or Optional.empty() if the buffer is empty).
   */
  public Optional<Pose3d> sampleAt(double timestamp) {
    // Step 0: If there are no odometry updates to sample, skip.
    if (m_odometryPoseBuffer.getInternalBuffer().isEmpty()) {
      return Optional.empty();
    }

    // Step 1: Make sure timestamp matches the sample from the odometry pose buffer. (When sampling,
    // the buffer will always use a timestamp between the first and last timestamps)
    double oldestOdometryTimestamp = m_odometryPoseBuffer.getInternalBuffer().firstKey();
    double newestOdometryTimestamp = m_odometryPoseBuffer.getInternalBuffer().lastKey();
    timestamp = Math.clamp(timestamp, oldestOdometryTimestamp, newestOdometryTimestamp);

    // Step 2: If there are no applicable vision updates, use the odometry-only information.
    if (m_visionUpdates.isEmpty() || timestamp < m_visionUpdates.firstKey()) {
      return m_odometryPoseBuffer.getSample(timestamp);
    }

    // Step 3: Get the latest vision update from before or at the timestamp to sample at.
    double floorTimestamp = m_visionUpdates.floorKey(timestamp);
    var visionUpdate = m_visionUpdates.get(floorTimestamp);

    // Step 4: Get the pose measured by odometry at the time of the sample.
    var odometryEstimate = m_odometryPoseBuffer.getSample(timestamp);

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
   * @param visionRobotPose The pose of the robot as measured by the vision camera.
   * @param timestamp The timestamp of the vision measurement in seconds. Note that if you don't use
   *     your own time source by calling {@link
   *     PoseEstimator3d#updateWithTime(double,Rotation3d,Object)} then you must use a timestamp
   *     with the same epoch as {@link org.wpilib.system.Timer#getMonotonicTimestamp()}.) This means
   *     that you should use {@link org.wpilib.system.Timer#getMonotonicTimestamp()} as your time
   *     source or sync the epochs.
   */
  public void addVisionMeasurement(Pose3d visionRobotPose, double timestamp) {
    // Step 0: If this measurement is old enough to be outside the pose buffer's timespan, skip.
    if (m_odometryPoseBuffer.getInternalBuffer().isEmpty()
        || m_odometryPoseBuffer.getInternalBuffer().lastKey() - kBufferDuration > timestamp) {
      return;
    }

    // Step 1: Clean up any old entries
    cleanUpVisionUpdates();

    // Step 2: Get the pose measured by odometry at the moment the vision measurement was made.
    var odometrySample = m_odometryPoseBuffer.getSample(timestamp);

    if (odometrySample.isEmpty()) {
      return;
    }

    // Step 3: Get the vision-compensated pose estimate at the moment the vision measurement was
    // made.
    var visionSample = sampleAt(timestamp);

    if (visionSample.isEmpty()) {
      return;
    }

    // Step 4: Measure the transform between the old pose estimate and the vision pose.
    var transform = visionRobotPose.minus(visionSample.get());

    // Step 5: We should not trust the transform entirely, so instead we scale this transform by a
    // Kalman gain matrix representing how much we trust vision measurements compared to our current
    // pose. Then we convert the result back to a Transform3d.
    var scaledTransform =
        new Transform3d(
            m_vision_k[0] * transform.getX(),
            m_vision_k[1] * transform.getY(),
            m_vision_k[2] * transform.getZ(),
            new Rotation3d(
                m_vision_k[3] * transform.getRotation().getX(),
                m_vision_k[4] * transform.getRotation().getY(),
                m_vision_k[5] * transform.getRotation().getZ()));

    // Step 6: Calculate and record the vision update.
    var visionUpdate =
        new VisionUpdate(visionSample.get().plus(scaledTransform), odometrySample.get());
    m_visionUpdates.put(timestamp, visionUpdate);

    // Step 7: Remove later vision measurements. (Matches previous behavior)
    m_visionUpdates.tailMap(timestamp, false).entrySet().clear();

    // Step 8: Update latest pose estimate. Since we cleared all updates after this vision update,
    // it's guaranteed to be the latest vision update.
    m_poseEstimate = visionUpdate.compensate(m_odometry.getPose());
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
   * @param visionRobotPose The pose of the robot as measured by the vision camera.
   * @param timestamp The timestamp of the vision measurement in seconds. Note that if you don't use
   *     your own time source by calling {@link #updateWithTime}, then you must use a timestamp with
   *     the same epoch as {@link org.wpilib.system.Timer#getMonotonicTimestamp()}). This means that
   *     you should use {@link org.wpilib.system.Timer#getMonotonicTimestamp()} as your time source
   *     in this case.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x position
   *     in meters, y position in meters, z position in meters, and angle in radians). Increase
   *     these numbers to trust the vision pose measurement less.
   */
  public void addVisionMeasurement(
      Pose3d visionRobotPose, double timestamp, Matrix<N4, N1> visionMeasurementStdDevs) {
    setVisionMeasurementStdDevs(visionMeasurementStdDevs);
    addVisionMeasurement(visionRobotPose, timestamp);
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This should be called every
   * loop.
   *
   * @param xWheelPos The distance traveled by the forward-facing wheel, in meters.
   * @param yWheelPos The distance traveled by the left-facing wheel, in meters.
   * @param gyroAngle The current gyro angle.
   * @return The estimated pose of the robot in meters.
   */
  public Pose3d update(double xWheelPos, double yWheelPos, Rotation3d gyroAngle) {
    return updateWithTime(MathSharedStore.getTimestamp(), xWheelPos, yWheelPos, gyroAngle);
  }

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This should be called every
   * loop.
   *
   * @param currentTime Time at which this method was called, in seconds.
   * @param xWheelPos The distance traveled by the forward-facing wheel, in meters.
   * @param yWheelPos The distance traveled by the left-facing wheel, in meters.
   * @param gyroAngle The current gyro angle.
   * @return The estimated pose of the robot in meters.
   */
  public Pose3d updateWithTime(
      double currentTime, double xWheelPos, double yWheelPos, Rotation3d gyroAngle) {
    var odometryEstimate = m_odometry.update(xWheelPos, yWheelPos, gyroAngle);

    m_odometryPoseBuffer.addSample(currentTime, odometryEstimate);

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
