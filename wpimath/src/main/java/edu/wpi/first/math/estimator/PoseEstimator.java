// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.interpolation.Interpolatable;
import edu.wpi.first.math.interpolation.TimeInterpolatableBuffer;
import edu.wpi.first.math.kinematics.Kinematics;
import edu.wpi.first.math.kinematics.Odometry;
import edu.wpi.first.math.kinematics.WheelPositions;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N3;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.Objects;

/**
 * This class wraps {@link Odometry} to fuse latency-compensated vision measurements with encoder
 * measurements. Robot code should not use this directly- Instead, use the particular type for your
 * drivetrain (e.g., {@link DifferentialDrivePoseEstimator}). It is intended to be a drop-in
 * replacement for {@link Odometry}; in fact, if you never call {@link
 * PoseEstimator#addVisionMeasurement} and only call {@link PoseEstimator#update} then this will
 * behave exactly the same as Odometry.
 *
 * <p>{@link PoseEstimator#update} should be called every robot loop.
 *
 * <p>{@link PoseEstimator#addVisionMeasurement} can be called as infrequently as you want; if you
 * never call it then this class will behave exactly like regular encoder odometry.
 *
 * @param <T> Wheel positions type.
 */
public class PoseEstimator<T extends WheelPositions<T>> {
  private final Kinematics<?, T> m_kinematics;
  private final Odometry<T> m_odometry;
  private final Matrix<N3, N1> m_q = new Matrix<>(Nat.N3(), Nat.N1());
  private final Matrix<N3, N3> m_visionK = new Matrix<>(Nat.N3(), Nat.N3());

  private static final double kBufferDuration = 1.5;
  private final TimeInterpolatableBuffer<InterpolationRecord> m_poseBuffer =
      TimeInterpolatableBuffer.createBuffer(kBufferDuration);

  /**
   * Constructs a PoseEstimator.
   *
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param odometry A correctly-configured odometry object for your drivetrain.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in meters, y position
   *     in meters, and heading in radians). Increase these numbers to trust your state estimate
   *     less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x position
   *     in meters, y position in meters, and heading in radians). Increase these numbers to trust
   *     the vision pose measurement less.
   */
  public PoseEstimator(
      Kinematics<?, T> kinematics,
      Odometry<T> odometry,
      Matrix<N3, N1> stateStdDevs,
      Matrix<N3, N1> visionMeasurementStdDevs) {
    m_kinematics = kinematics;
    m_odometry = odometry;

    for (int i = 0; i < 3; ++i) {
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
   *     numbers to trust global measurements from vision less. This matrix is in the form [x, y,
   *     theta]ᵀ, with units in meters and radians.
   */
  public final void setVisionMeasurementStdDevs(Matrix<N3, N1> visionMeasurementStdDevs) {
    var r = new double[3];
    for (int i = 0; i < 3; ++i) {
      r[i] = visionMeasurementStdDevs.get(i, 0) * visionMeasurementStdDevs.get(i, 0);
    }

    // Solve for closed form Kalman gain for continuous Kalman filter with A = 0
    // and C = I. See wpimath/algorithms.md.
    for (int row = 0; row < 3; ++row) {
      if (m_q.get(row, 0) == 0.0) {
        m_visionK.set(row, row, 0.0);
      } else {
        m_visionK.set(
            row, row, m_q.get(row, 0) / (m_q.get(row, 0) + Math.sqrt(m_q.get(row, 0) * r[row])));
      }
    }
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
  public void resetPosition(Rotation2d gyroAngle, T wheelPositions, Pose2d poseMeters) {
    // Reset state estimate and error covariance
    m_odometry.resetPosition(gyroAngle, wheelPositions, poseMeters);
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

  /**
   * Adds a vision measurement to the Kalman Filter. This will correct the odometry pose estimate
   * while still accounting for measurement noise.
   *
   * <p>This method can be called as infrequently as you want, as long as you are calling {@link
   * PoseEstimator#update} every loop.
   *
   * <p>To promote stability of the pose estimate and make it robust to bad vision data, we
   * recommend only adding vision measurements that are already within one meter or so of the
   * current pose estimate.
   *
   * @param visionRobotPoseMeters The pose of the robot as measured by the vision camera.
   * @param timestampSeconds The timestamp of the vision measurement in seconds. Note that if you
   *     don't use your own time source by calling {@link
   *     PoseEstimator#updateWithTime(double,Rotation2d,WheelPositions)} then you must use a
   *     timestamp with an epoch since FPGA startup (i.e., the epoch of this timestamp is the same
   *     epoch as {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()}.) This means that you
   *     should use {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()} as your time source or
   *     sync the epochs.
   */
  public void addVisionMeasurement(Pose2d visionRobotPoseMeters, double timestampSeconds) {
    // Step 0: If this measurement is old enough to be outside the pose buffer's timespan, skip.
    try {
      if (m_poseBuffer.getInternalBuffer().lastKey() - kBufferDuration > timestampSeconds) {
        return;
      }
    } catch (NoSuchElementException ex) {
      return;
    }

    // Step 1: Get the pose odometry measured at the moment the vision measurement was made.
    var sample = m_poseBuffer.getSample(timestampSeconds);

    if (sample.isEmpty()) {
      return;
    }

    // Step 2: Measure the twist between the odometry pose and the vision pose.
    var twist = sample.get().poseMeters.log(visionRobotPoseMeters);

    // Step 3: We should not trust the twist entirely, so instead we scale this twist by a Kalman
    // gain matrix representing how much we trust vision measurements compared to our current pose.
    var k_times_twist = m_visionK.times(VecBuilder.fill(twist.dx, twist.dy, twist.dtheta));

    // Step 4: Convert back to Twist2d.
    var scaledTwist =
        new Twist2d(k_times_twist.get(0, 0), k_times_twist.get(1, 0), k_times_twist.get(2, 0));

    // Step 5: Reset Odometry to state at sample with vision adjustment.
    m_odometry.resetPosition(
        sample.get().gyroAngle,
        sample.get().wheelPositions,
        sample.get().poseMeters.exp(scaledTwist));

    // Step 6: Record the current pose to allow multiple measurements from the same timestamp
    m_poseBuffer.addSample(
        timestampSeconds,
        new InterpolationRecord(
            getEstimatedPosition(), sample.get().gyroAngle, sample.get().wheelPositions));

    // Step 7: Replay odometry inputs between sample time and latest recorded sample to update the
    // pose buffer and correct odometry.
    for (Map.Entry<Double, InterpolationRecord> entry :
        m_poseBuffer.getInternalBuffer().tailMap(timestampSeconds).entrySet()) {
      updateWithTime(entry.getKey(), entry.getValue().gyroAngle, entry.getValue().wheelPositions);
    }
  }

  /**
   * Adds a vision measurement to the Kalman Filter. This will correct the odometry pose estimate
   * while still accounting for measurement noise.
   *
   * <p>This method can be called as infrequently as you want, as long as you are calling {@link
   * PoseEstimator#update} every loop.
   *
   * <p>To promote stability of the pose estimate and make it robust to bad vision data, we
   * recommend only adding vision measurements that are already within one meter or so of the
   * current pose estimate.
   *
   * <p>Note that the vision measurement standard deviations passed into this method will continue
   * to apply to future measurements until a subsequent call to {@link
   * PoseEstimator#setVisionMeasurementStdDevs(Matrix)} or this method.
   *
   * @param visionRobotPoseMeters The pose of the robot as measured by the vision camera.
   * @param timestampSeconds The timestamp of the vision measurement in seconds. Note that if you
   *     don't use your own time source by calling {@link #updateWithTime}, then you must use a
   *     timestamp with an epoch since FPGA startup (i.e., the epoch of this timestamp is the same
   *     epoch as {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()}). This means that you
   *     should use {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()} as your time source in
   *     this case.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose measurement (x position
   *     in meters, y position in meters, and heading in radians). Increase these numbers to trust
   *     the vision pose measurement less.
   */
  public void addVisionMeasurement(
      Pose2d visionRobotPoseMeters,
      double timestampSeconds,
      Matrix<N3, N1> visionMeasurementStdDevs) {
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
  public Pose2d update(Rotation2d gyroAngle, T wheelPositions) {
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
  public Pose2d updateWithTime(double currentTimeSeconds, Rotation2d gyroAngle, T wheelPositions) {
    m_odometry.update(gyroAngle, wheelPositions);
    m_poseBuffer.addSample(
        currentTimeSeconds,
        new InterpolationRecord(getEstimatedPosition(), gyroAngle, wheelPositions.copy()));

    return getEstimatedPosition();
  }

  /**
   * Represents an odometry record. The record contains the inputs provided as well as the pose that
   * was observed based on these inputs, as well as the previous record and its inputs.
   */
  private class InterpolationRecord implements Interpolatable<InterpolationRecord> {
    // The pose observed given the current sensor inputs and the previous pose.
    private final Pose2d poseMeters;

    // The current gyro angle.
    private final Rotation2d gyroAngle;

    // The current encoder readings.
    private final T wheelPositions;

    /**
     * Constructs an Interpolation Record with the specified parameters.
     *
     * @param poseMeters The pose observed given the current sensor inputs and the previous pose.
     * @param gyro The current gyro angle.
     * @param wheelPositions The current encoder readings.
     */
    private InterpolationRecord(Pose2d poseMeters, Rotation2d gyro, T wheelPositions) {
      this.poseMeters = poseMeters;
      this.gyroAngle = gyro;
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
        var wheelLerp = wheelPositions.interpolate(endValue.wheelPositions, t);

        // Find the new gyro angle.
        var gyroLerp = gyroAngle.interpolate(endValue.gyroAngle, t);

        // Create a twist to represent the change based on the interpolated sensor inputs.
        Twist2d twist = m_kinematics.toTwist2d(wheelPositions, wheelLerp);
        twist.dtheta = gyroLerp.minus(gyroAngle).getRadians();

        return new InterpolationRecord(poseMeters.exp(twist), gyroLerp, wheelLerp);
      }
    }

    @Override
    public boolean equals(Object obj) {
      if (this == obj) {
        return true;
      }
      if (!(obj instanceof PoseEstimator.InterpolationRecord)) {
        return false;
      }
      var record = (PoseEstimator<?>.InterpolationRecord) obj;
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
