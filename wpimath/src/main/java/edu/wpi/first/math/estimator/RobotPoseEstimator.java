package edu.wpi.first.math.estimator;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.interpolation.TimeInterpolatableBuffer;
import edu.wpi.first.math.kinematics.SwerveModulePosition;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N3;
import java.util.Map;
import java.util.NoSuchElementException;

public abstract class RobotPoseEstimator {
  private final Matrix<N3, N1> m_q = new Matrix<>(Nat.N3(), Nat.N1());
  private Matrix<N3, N3> m_visionK = new Matrix<>(Nat.N3(), Nat.N3());
  private static final double kBufferDuration = 1.5;

  protected final TimeInterpolatableBuffer<BaseInterpolationRecord> m_poseBuffer =
      TimeInterpolatableBuffer.createBuffer(kBufferDuration);

  public void setVisionMeasurementStdDevs(Matrix<N3, N1> visionMeasurementStdDevs) {
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

  public abstract void resetOdometry(BaseInterpolationRecord sample, Twist2d scaledTwist);

  public abstract void recordCurrentPose(BaseInterpolationRecord sample, double timestampSeconds);

  public abstract void replayOdometryInputs(Map.Entry<Double, BaseInterpolationRecord> entry);

  /**
   * Adds a vision measurement to the Kalman Filter. This will correct the odometry pose estimate
   * while still accounting for measurement noise.
   *
   * <p>This method can be called as infrequently as you want, as long as you are calling {@link
   * SwerveDrivePoseEstimator#update} every loop.
   *
   * <p>To promote stability of the pose estimate and make it robust to bad vision data, we
   * recommend only adding vision measurements that are already within one meter or so of the
   * current pose estimate.
   *
   * @param visionRobotPoseMeters The pose of the robot as measured by the vision camera.
   * @param timestampSeconds The timestamp of the vision measurement in seconds. Note that if you
   *     don't use your own time source by calling {@link
   *     SwerveDrivePoseEstimator#updateWithTime(double,Rotation2d,SwerveModulePosition[])} then you
   *     must use a timestamp with an epoch since FPGA startup (i.e., the epoch of this timestamp is
   *     the same epoch as {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()}.) This means that
   *     you should use {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()} as your time source
   *     or sync the epochs.
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
    resetOdometry(sample.get(), scaledTwist);

    // Step 6: Record the current pose to allow multiple measurements from the same timestamp
    recordCurrentPose(sample.get(), timestampSeconds);

    // Step 7: Replay odometry inputs between sample time and latest recorded sample to update the
    // pose buffer and correct odometry.
    for (Map.Entry<Double, BaseInterpolationRecord> entry :
        m_poseBuffer.getInternalBuffer().tailMap(timestampSeconds).entrySet()) {
      replayOdometryInputs(entry);
    }
  }
  /**
   * Adds a vision measurement to the Kalman Filter. This will correct the odometry pose estimate
   * while still accounting for measurement noise.
   *
   * <p>This method can be called as infrequently as you want, as long as you are calling {@link
   * SwerveDrivePoseEstimator#update} every loop.
   *
   * <p>To promote stability of the pose estimate and make it robust to bad vision data, we
   * recommend only adding vision measurements that are already within one meter or so of the
   * current pose estimate.
   *
   * <p>Note that the vision measurement standard deviations passed into this method will continue
   * to apply to future measurements until a subsequent call to {@link
   * SwerveDrivePoseEstimator#setVisionMeasurementStdDevs(Matrix)} or this method.
   *
   * @param visionRobotPoseMeters The pose of the robot as measured by the vision camera.
   * @param timestampSeconds The timestamp of the vision measurement in seconds. Note that if you
   *     don't use your own time source by calling {@link
   *     SwerveDrivePoseEstimator#updateWithTime(double,Rotation2d,SwerveModulePosition[])}, then
   *     you must use a timestamp with an epoch since FPGA startup (i.e., the epoch of this
   *     timestamp is the same epoch as {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()}).
   *     This means that you should use {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()} as
   *     your time source in this case.
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
}
