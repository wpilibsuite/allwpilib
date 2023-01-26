// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.interpolation.Interpolatable;
import edu.wpi.first.math.interpolation.TimeInterpolatableBuffer;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;
import edu.wpi.first.math.kinematics.MecanumDriveOdometry;
import edu.wpi.first.math.kinematics.MecanumDriveWheelPositions;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.util.WPIUtilJNI;
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
public class MecanumDrivePoseEstimator {
  private final MecanumDriveKinematics m_kinematics;
  private final MecanumDriveOdometry m_odometry;
  private final Matrix<N3, N1> m_q = new Matrix<>(Nat.N3(), Nat.N1());
  private Matrix<N3, N3> m_visionK = new Matrix<>(Nat.N3(), Nat.N3());
  private Matrix<N3, N3> m_visionKSum = Matrix.eye(Nat.N3());
  private Pose2d m_poseEstimate;

  private final TimeInterpolatableBuffer<InterpolationRecord> m_poseBuffer =
      TimeInterpolatableBuffer.createBuffer(1.5);

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
    m_poseEstimate = m_odometry.getPoseMeters();

    for (int i = 0; i < 3; ++i) {
      m_q.set(i, 0, stateStdDevs.get(i, 0) * stateStdDevs.get(i, 0));
    }

    // Initialize vision R
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
    m_poseEstimate = poseMeters;
    m_poseBuffer.clear();
    m_visionKSum = Matrix.eye(Nat.N3());
  }

  /**
   * Gets the estimated robot pose.
   *
   * @return The estimated robot pose in meters.
   */
  public Pose2d getEstimatedPosition() {
    return m_poseEstimate;
  }

  /**
   * Gets the estimated robot pose at timestampSeconds.
   *
   * @param timestampSeconds The timestamp of the vision measurement in seconds. Note that if you
   *     don't use your own time source by calling {@link
   *     MecanumDrivePoseEstimator#updateWithTime(double,Rotation2d,MecanumDriveWheelPositions)}
   *     then you must use a timestamp with an epoch since FPGA startup (i.e., the epoch of this
   *     timestamp is the same epoch as {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()}.)
   *     This means that you should use {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()} as
   *     your time source or sync the epochs.
   * @return The estimated robot pose in meters.
   */
  public Pose2d getEstimatedPosition(double timestampSeconds) {
    if (m_poseBuffer.getInternalBuffer().isEmpty()) {
      return m_poseEstimate;
    }
    // current to old odometry delta
    var delta =
        new Transform2d(
            m_odometry.getPoseMeters(), m_poseBuffer.getSample(timestampSeconds).get().poseMeters);
    return m_poseEstimate.transformBy(delta);
  }

  /**
   * Adds a vision measurement to the Kalman Filter. This will correct the odometry pose estimate
   * while still accounting for measurement noise.
   *
   * <p>This method can be called as infrequently as you want, as long as you are calling {@link
   * MecanumDrivePoseEstimator#update} every loop.
   *
   * <p>To promote stability of the pose estimate and make it robust to bad vision data, we
   * recommend only adding vision measurements that are already within one meter or so of the
   * current pose estimate.
   *
   * @param visionRobotPoseMeters The pose of the robot as measured by the vision camera.
   * @param timestampSeconds The timestamp of the vision measurement in seconds. Note that if you
   *     don't use your own time source by calling {@link
   *     MecanumDrivePoseEstimator#updateWithTime(double,Rotation2d,MecanumDriveWheelPositions)}
   *     then you must use a timestamp with an epoch since FPGA startup (i.e., the epoch of this
   *     timestamp is the same epoch as {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()}.)
   *     This means that you should use {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp()} as
   *     your time source or sync the epochs.
   */
  public void addVisionMeasurement(Pose2d visionRobotPoseMeters, double timestampSeconds) {
    // Step 1: Get the odometry pose measured at the moment the vision measurement was made.
    var sample = m_poseBuffer.getSample(timestampSeconds);

    if (sample.isEmpty()) {
      return;
    }

    // Step 2: Get the odometry delta in the time since the vision measurement,
    var delta = new Transform2d(sample.get().poseMeters, m_odometry.getPoseMeters());
    // and apply its inverse to the current pose estimate to get the estimate at the time
    // of the vision measurement
    var old_estimate = m_poseEstimate.transformBy(delta.inverse());

    // Step 3: Measure the twist between the "old" estimated pose and the vision pose
    var twist = old_estimate.log(visionRobotPoseMeters);

    // Step 4: We should not trust the twist entirely, so instead we scale this twist by a Kalman
    // gain matrix representing how much we trust vision measurements compared to our current pose.
    var k_times_twist = m_visionK.times(VecBuilder.fill(twist.dx, twist.dy, twist.dtheta));

    // Step 5: The Kalman gains scale the vision twists independent of other vision measurements,
    // so applying multiple vision measurements before the next update will have different results
    // depending on the order they are applied in. We can sum the Kalman gains applied before the
    // next update and use that to effectively average the twist for multiple vision measurements.
    var weighted_k_times_twist =
        k_times_twist.elementTimes(m_visionKSum.diag().extractColumnVector(0).elementPower(-1));
    m_visionKSum = m_visionKSum.plus(m_visionK);

    // Step 5: Convert back to Twist2d.
    double[] scaledTwistVals = weighted_k_times_twist.getData();
    var scaledTwist = new Twist2d(scaledTwistVals[0], scaledTwistVals[1], scaledTwistVals[2]);

    // Step 6: Apply scaled twist to the "old" estimated pose
    old_estimate = old_estimate.exp(scaledTwist);

    // Step 7: Re-apply odometry delta to get the "current" estimated pose
    m_poseEstimate = old_estimate.transformBy(delta);
  }

  /**
   * Adds a vision measurement to the Kalman Filter. This will correct the odometry pose estimate
   * while still accounting for measurement noise.
   *
   * <p>This method can be called as infrequently as you want, as long as you are calling {@link
   * MecanumDrivePoseEstimator#update} every loop.
   *
   * <p>To promote stability of the pose estimate and make it robust to bad vision data, we
   * recommend only adding vision measurements that are already within one meter or so of the
   * current pose estimate.
   *
   * <p>Note that the vision measurement standard deviations passed into this method will continue
   * to apply to future measurements until a subsequent call to {@link
   * MecanumDrivePoseEstimator#setVisionMeasurementStdDevs(Matrix)} or this method.
   *
   * @param visionRobotPoseMeters The pose of the robot as measured by the vision camera.
   * @param timestampSeconds The timestamp of the vision measurement in seconds. Note that if you
   *     don't use your own time source by calling {@link
   *     MecanumDrivePoseEstimator#updateWithTime(double,Rotation2d,MecanumDriveWheelPositions)},
   *     then you must use a timestamp with an epoch since FPGA startup (i.e., the epoch of this
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

  /**
   * Updates the pose estimator with wheel encoder and gyro information. This should be called every
   * loop.
   *
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The distances driven by each wheel.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d update(Rotation2d gyroAngle, MecanumDriveWheelPositions wheelPositions) {
    return updateWithTime(WPIUtilJNI.now() * 1.0e-6, gyroAngle, wheelPositions);
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
    // update odometry
    var lastOdom = m_odometry.getPoseMeters();
    var currOdom = m_odometry.update(gyroAngle, wheelPositions);
    m_poseBuffer.addSample(
        currentTimeSeconds,
        new InterpolationRecord(
            currOdom,
            new MecanumDriveWheelPositions(
                wheelPositions.frontLeftMeters,
                wheelPositions.frontRightMeters,
                wheelPositions.rearLeftMeters,
                wheelPositions.rearRightMeters)));

    // apply odometry update to pose estimate
    m_poseEstimate = m_poseEstimate.transformBy(new Transform2d(lastOdom, currOdom));

    // reset kalman gain sum matrix
    m_visionKSum = Matrix.eye(Nat.N3());

    return getEstimatedPosition();
  }

  /**
   * Represents an odometry record. The record contains the inputs provided as well as the pose that
   * was observed based on these inputs, as well as the previous record and its inputs.
   */
  private class InterpolationRecord implements Interpolatable<InterpolationRecord> {
    // The pose observed given the current sensor inputs and the previous pose.
    private final Pose2d poseMeters;

    // The distances traveled by each wheel encoder.
    private final MecanumDriveWheelPositions wheelPositions;

    /**
     * Constructs an Interpolation Record with the specified parameters.
     *
     * @param pose The pose observed given the current sensor inputs and the previous pose.
     * @param wheelPositions The distances traveled by each wheel encoder.
     */
    private InterpolationRecord(Pose2d poseMeters, MecanumDriveWheelPositions wheelPositions) {
      this.poseMeters = poseMeters;
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
        var gyro_lerp = poseMeters.getRotation().interpolate(endValue.poseMeters.getRotation(), t);

        // Create a twist to represent this change based on the interpolated sensor inputs.
        Twist2d twist = m_kinematics.toTwist2d(wheels_delta);
        twist.dtheta = gyro_lerp.minus(poseMeters.getRotation()).getRadians();

        return new InterpolationRecord(poseMeters.exp(twist), wheels_lerp);
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
      return Objects.equals(wheelPositions, record.wheelPositions)
          && Objects.equals(poseMeters, record.poseMeters);
    }

    @Override
    public int hashCode() {
      return Objects.hash(wheelPositions, poseMeters);
    }
  }
}
