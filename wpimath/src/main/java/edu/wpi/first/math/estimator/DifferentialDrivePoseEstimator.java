// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.StateSpaceUtil;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.interpolation.TimeInterpolatableBuffer;
import edu.wpi.first.math.kinematics.DifferentialDriveWheelSpeeds;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.math.numbers.N5;
import edu.wpi.first.util.WPIUtilJNI;
import java.util.function.BiConsumer;

/**
 * This class wraps an {@link edu.wpi.first.math.estimator.UnscentedKalmanFilter Unscented Kalman
 * Filter} to fuse latency-compensated vision measurements with differential drive encoder
 * measurements. It will correct for noisy vision measurements and encoder drift. It is intended to
 * be an easy drop-in for {@link edu.wpi.first.math.kinematics.DifferentialDriveOdometry}; in fact,
 * if you never call {@link DifferentialDrivePoseEstimator#addVisionMeasurement} and only call
 * {@link DifferentialDrivePoseEstimator#update} then this will behave exactly the same as
 * DifferentialDriveOdometry.
 *
 * <p>{@link DifferentialDrivePoseEstimator#update} should be called every robot loop (if your robot
 * loops are faster than the default then you should change the {@link
 * DifferentialDrivePoseEstimator#DifferentialDrivePoseEstimator(Rotation2d, Pose2d, Matrix, Matrix,
 * Matrix, double) nominal delta time}.) {@link DifferentialDrivePoseEstimator#addVisionMeasurement}
 * can be called as infrequently as you want; if you never call it then this class will behave
 * exactly like regular encoder odometry.
 *
 * <p>The state-space system used internally has the following states (x), inputs (u), and outputs
 * (y):
 *
 * <p><strong> x = [x, y, theta, dist_l, dist_r]ᵀ </strong> in the field coordinate system
 * containing x position, y position, heading, left encoder distance, and right encoder distance.
 *
 * <p><strong> u = [v_x, v_y, omega]ᵀ </strong> containing x velocity, y velocity, and angular rate
 * in the field coordinate system.
 *
 * <p>NB: Using velocities make things considerably easier, because it means that teams don't have
 * to worry about getting an accurate model. Basically, we suspect that it's easier for teams to get
 * good encoder data than it is for them to perform system identification well enough to get a good
 * model.
 *
 * <p><strong> y = [x, y, theta]ᵀ </strong> from vision containing x position, y position, and
 * heading; or <strong>y = [dist_l, dist_r, theta] </strong> containing left encoder position, right
 * encoder position, and gyro heading.
 */
public class DifferentialDrivePoseEstimator {
  final UnscentedKalmanFilter<N5, N3, N3> m_observer; // Package-private to allow for unit testing
  private final BiConsumer<Matrix<N3, N1>, Matrix<N3, N1>> m_visionCorrect;
  private final TimeInterpolatableBuffer<Pose2d> m_poseBuffer;

  private final double m_nominalDt; // Seconds
  private double m_prevTimeSeconds = -1.0;

  private Rotation2d m_gyroOffset;
  private Rotation2d m_previousAngle;

  private Matrix<N3, N3> m_visionContR;

  /**
   * Constructs a DifferentialDrivePoseEstimator.
   *
   * @param gyroAngle The current gyro angle.
   * @param initialPoseMeters The starting pose estimate.
   * @param stateStdDevs Standard deviations of model states. Increase these numbers to trust your
   *     model's state estimates less. This matrix is in the form [x, y, theta, dist_l, dist_r]ᵀ,
   *     with units in meters and radians.
   * @param localMeasurementStdDevs Standard deviations of the encoder and gyro measurements.
   *     Increase these numbers to trust sensor readings from encoders and gyros less. This matrix
   *     is in the form [dist_l, dist_r, theta]ᵀ, with units in meters and radians.
   * @param visionMeasurementStdDevs Standard deviations of the vision measurements. Increase these
   *     numbers to trust global measurements from vision less. This matrix is in the form [x, y,
   *     theta]ᵀ, with units in meters and radians.
   */
  public DifferentialDrivePoseEstimator(
      Rotation2d gyroAngle,
      Pose2d initialPoseMeters,
      Matrix<N5, N1> stateStdDevs,
      Matrix<N3, N1> localMeasurementStdDevs,
      Matrix<N3, N1> visionMeasurementStdDevs) {
    this(
        gyroAngle,
        initialPoseMeters,
        stateStdDevs,
        localMeasurementStdDevs,
        visionMeasurementStdDevs,
        0.02);
  }

  /**
   * Constructs a DifferentialDrivePoseEstimator.
   *
   * @param gyroAngle The current gyro angle.
   * @param initialPoseMeters The starting pose estimate.
   * @param stateStdDevs Standard deviations of model states. Increase these numbers to trust your
   *     model's state estimates less. This matrix is in the form [x, y, theta, dist_l, dist_r]ᵀ,
   *     with units in meters and radians.
   * @param localMeasurementStdDevs Standard deviations of the encoder and gyro measurements.
   *     Increase these numbers to trust sensor readings from encoders and gyros less. This matrix
   *     is in the form [dist_l, dist_r, theta]ᵀ, with units in meters and radians.
   * @param visionMeasurementStdDevs Standard deviations of the vision measurements. Increase these
   *     numbers to trust global measurements from vision less. This matrix is in the form [x, y,
   *     theta]ᵀ, with units in meters and radians.
   * @param nominalDtSeconds The time in seconds between each robot loop.
   */
  @SuppressWarnings("ParameterName")
  public DifferentialDrivePoseEstimator(
      Rotation2d gyroAngle,
      Pose2d initialPoseMeters,
      Matrix<N5, N1> stateStdDevs,
      Matrix<N3, N1> localMeasurementStdDevs,
      Matrix<N3, N1> visionMeasurementStdDevs,
      double nominalDtSeconds) {
    m_nominalDt = nominalDtSeconds;

    m_observer =
        new UnscentedKalmanFilter<>(
            Nat.N5(),
            Nat.N3(),
            this::f,
            (x, u) -> VecBuilder.fill(x.get(3, 0), x.get(4, 0), x.get(2, 0)),
            stateStdDevs,
            localMeasurementStdDevs,
            AngleStatistics.angleMean(2),
            AngleStatistics.angleMean(2),
            AngleStatistics.angleResidual(2),
            AngleStatistics.angleResidual(2),
            AngleStatistics.angleAdd(2),
            m_nominalDt);
    m_poseBuffer = TimeInterpolatableBuffer.createBuffer(1.5);

    // Initialize vision R
    setVisionMeasurementStdDevs(visionMeasurementStdDevs);

    m_visionCorrect =
        (u, y) ->
            m_observer.correct(
                Nat.N3(),
                u,
                y,
                (x, u1) -> new Matrix<>(x.getStorage().extractMatrix(0, 3, 0, 1)),
                m_visionContR,
                AngleStatistics.angleMean(2),
                AngleStatistics.angleResidual(2),
                AngleStatistics.angleResidual(2),
                AngleStatistics.angleAdd(2));

    m_gyroOffset = initialPoseMeters.getRotation().minus(gyroAngle);
    m_previousAngle = initialPoseMeters.getRotation();
    m_observer.setXhat(fillStateVector(initialPoseMeters, 0.0, 0.0));
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
    m_visionContR = StateSpaceUtil.makeCovarianceMatrix(Nat.N3(), visionMeasurementStdDevs);
  }

  @SuppressWarnings({"ParameterName", "MethodName"})
  private Matrix<N5, N1> f(Matrix<N5, N1> x, Matrix<N3, N1> u) {
    // Apply a rotation matrix. Note that we do *not* add x--Runge-Kutta does that for us.
    var theta = x.get(2, 0);
    var toFieldRotation =
        new MatBuilder<>(Nat.N5(), Nat.N5())
            .fill(
                Math.cos(theta),
                -Math.sin(theta),
                0,
                0,
                0,
                Math.sin(theta),
                Math.cos(theta),
                0,
                0,
                0,
                0,
                0,
                1,
                0,
                0,
                0,
                0,
                0,
                1,
                0,
                0,
                0,
                0,
                0,
                1);
    return toFieldRotation.times(
        VecBuilder.fill(u.get(0, 0), u.get(1, 0), u.get(2, 0), u.get(0, 0), u.get(1, 0)));
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>You NEED to reset your encoders (to zero) when calling this method.
   *
   * <p>The gyroscope angle does not need to be reset here on the user's robot code. The library
   * automatically takes care of offsetting the gyro angle.
   *
   * @param poseMeters The position on the field that your robot is at.
   * @param gyroAngle The angle reported by the gyroscope.
   */
  public void resetPosition(Pose2d poseMeters, Rotation2d gyroAngle) {
    // Reset state estimate and error covariance
    m_observer.reset();
    m_poseBuffer.clear();

    m_observer.setXhat(fillStateVector(poseMeters, 0.0, 0.0));

    m_gyroOffset = getEstimatedPosition().getRotation().minus(gyroAngle);
    m_previousAngle = poseMeters.getRotation();
  }

  /**
   * Gets the pose of the robot at the current time as estimated by the Unscented Kalman Filter.
   *
   * @return The estimated robot pose in meters.
   */
  public Pose2d getEstimatedPosition() {
    return new Pose2d(
        m_observer.getXhat(0), m_observer.getXhat(1), new Rotation2d(m_observer.getXhat(2)));
  }

  /**
   * Add a vision measurement to the Unscented Kalman Filter. This will correct the odometry pose
   * estimate while still accounting for measurement noise.
   *
   * <p>This method can be called as infrequently as you want, as long as you are calling {@link
   * DifferentialDrivePoseEstimator#update} every loop.
   *
   * <p>To promote stability of the pose estimate and make it robust to bad vision data, we
   * recommend only adding vision measurements that are already within one meter or so of the
   * current pose estimate.
   *
   * @param visionRobotPoseMeters The pose of the robot as measured by the vision camera.
   * @param timestampSeconds The timestamp of the vision measurement in seconds. Note that if you
   *     don't use your own time source by calling {@link
   *     DifferentialDrivePoseEstimator#updateWithTime} then you must use a timestamp with an epoch
   *     since FPGA startup (i.e. the epoch of this timestamp is the same epoch as
   *     Timer.getFPGATimestamp.) This means that you should use Timer.getFPGATimestamp as your time
   *     source in this case.
   */
  public void addVisionMeasurement(Pose2d visionRobotPoseMeters, double timestampSeconds) {
    var sample = m_poseBuffer.getSample(timestampSeconds);
    if (sample.isPresent()) {
      m_visionCorrect.accept(
          new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.0, 0.0, 0.0),
          StateSpaceUtil.poseTo3dVector(
              getEstimatedPosition().transformBy(visionRobotPoseMeters.minus(sample.get()))));
    }
  }

  /**
   * Add a vision measurement to the Unscented Kalman Filter. This will correct the odometry pose
   * estimate while still accounting for measurement noise.
   *
   * <p>This method can be called as infrequently as you want, as long as you are calling {@link
   * DifferentialDrivePoseEstimator#update} every loop.
   *
   * <p>To promote stability of the pose estimate and make it robust to bad vision data, we
   * recommend only adding vision measurements that are already within one meter or so of the
   * current pose estimate.
   *
   * <p>Note that the vision measurement standard deviations passed into this method will continue
   * to apply to future measurements until a subsequent call to {@link
   * DifferentialDrivePoseEstimator#setVisionMeasurementStdDevs(Matrix)} or this method.
   *
   * @param visionRobotPoseMeters The pose of the robot as measured by the vision camera.
   * @param timestampSeconds The timestamp of the vision measurement in seconds. Note that if you
   *     don't use your own time source by calling {@link
   *     DifferentialDrivePoseEstimator#updateWithTime} then you must use a timestamp with an epoch
   *     since FPGA startup (i.e. the epoch of this timestamp is the same epoch as
   *     Timer.getFPGATimestamp.) This means that you should use Timer.getFPGATimestamp as your time
   *     source in this case.
   * @param visionMeasurementStdDevs Standard deviations of the vision measurements. Increase these
   *     numbers to trust global measurements from vision less. This matrix is in the form [x, y,
   *     theta]ᵀ, with units in meters and radians.
   */
  public void addVisionMeasurement(
      Pose2d visionRobotPoseMeters,
      double timestampSeconds,
      Matrix<N3, N1> visionMeasurementStdDevs) {
    setVisionMeasurementStdDevs(visionMeasurementStdDevs);
    addVisionMeasurement(visionRobotPoseMeters, timestampSeconds);
  }

  /**
   * Updates the the Unscented Kalman Filter using only wheel encoder information. Note that this
   * should be called every loop.
   *
   * @param gyroAngle The current gyro angle.
   * @param wheelVelocitiesMetersPerSecond The velocities of the wheels in meters per second.
   * @param distanceLeftMeters The total distance travelled by the left wheel in meters since the
   *     last time you called {@link DifferentialDrivePoseEstimator#resetPosition}.
   * @param distanceRightMeters The total distance travelled by the right wheel in meters since the
   *     last time you called {@link DifferentialDrivePoseEstimator#resetPosition}.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d update(
      Rotation2d gyroAngle,
      DifferentialDriveWheelSpeeds wheelVelocitiesMetersPerSecond,
      double distanceLeftMeters,
      double distanceRightMeters) {
    return updateWithTime(
        WPIUtilJNI.now() * 1.0e-6,
        gyroAngle,
        wheelVelocitiesMetersPerSecond,
        distanceLeftMeters,
        distanceRightMeters);
  }

  /**
   * Updates the the Unscented Kalman Filter using only wheel encoder information. Note that this
   * should be called every loop.
   *
   * @param currentTimeSeconds Time at which this method was called, in seconds.
   * @param gyroAngle The current gyro angle.
   * @param wheelVelocitiesMetersPerSecond The velocities of the wheels in meters per second.
   * @param distanceLeftMeters The total distance travelled by the left wheel in meters since the
   *     last time you called {@link DifferentialDrivePoseEstimator#resetPosition}.
   * @param distanceRightMeters The total distance travelled by the right wheel in meters since the
   *     last time you called {@link DifferentialDrivePoseEstimator#resetPosition}.
   * @return The estimated pose of the robot in meters.
   */
  @SuppressWarnings({"LocalVariableName", "ParameterName"})
  public Pose2d updateWithTime(
      double currentTimeSeconds,
      Rotation2d gyroAngle,
      DifferentialDriveWheelSpeeds wheelVelocitiesMetersPerSecond,
      double distanceLeftMeters,
      double distanceRightMeters) {
    double dt = m_prevTimeSeconds >= 0 ? currentTimeSeconds - m_prevTimeSeconds : m_nominalDt;
    m_prevTimeSeconds = currentTimeSeconds;

    var angle = gyroAngle.plus(m_gyroOffset);
    // Diff drive forward kinematics:
    // v_c = (v_l + v_r) / 2
    var wheelVels = wheelVelocitiesMetersPerSecond;
    var u =
        VecBuilder.fill(
            (wheelVels.leftMetersPerSecond + wheelVels.rightMetersPerSecond) / 2,
            0,
            angle.minus(m_previousAngle).getRadians() / dt);
    m_previousAngle = angle;

    var localY = VecBuilder.fill(distanceLeftMeters, distanceRightMeters, angle.getRadians());
    m_poseBuffer.addSample(currentTimeSeconds, getEstimatedPosition());
    m_observer.predict(u, dt);
    m_observer.correct(u, localY);

    return getEstimatedPosition();
  }

  private static Matrix<N5, N1> fillStateVector(Pose2d pose, double leftDist, double rightDist) {
    return VecBuilder.fill(
        pose.getTranslation().getX(),
        pose.getTranslation().getY(),
        pose.getRotation().getRadians(),
        leftDist,
        rightDist);
  }
}
