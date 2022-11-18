// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.StateSpaceUtil;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.interpolation.TimeInterpolatableBuffer;
import edu.wpi.first.math.kinematics.SwerveDriveKinematics;
import edu.wpi.first.math.kinematics.SwerveModulePosition;
import edu.wpi.first.math.kinematics.SwerveModuleState;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.util.WPIUtilJNI;
import java.util.function.BiConsumer;

/**
 * This class wraps an {@link UnscentedKalmanFilter Unscented Kalman Filter} to fuse
 * latency-compensated vision measurements with swerve drive encoder velocity measurements. It will
 * correct for noisy measurements and encoder drift. It is intended to be an easy but more accurate
 * drop-in for {@link edu.wpi.first.math.kinematics.SwerveDriveOdometry}.
 *
 * <p>The generic arguments to this class define the size of the state, input and output vectors
 * used in the underlying {@link UnscentedKalmanFilter Unscented Kalman Filter}. {@link Num States}
 * must be equal to the module count + 3. {@link Num Inputs} must be equal to the module count + 3.
 * {@link Num Outputs} must be equal to the module count + 1.
 *
 * <p>{@link SwerveDrivePoseEstimator#update} should be called every robot loop. If your loops are
 * faster or slower than the default of 20 ms, then you should change the nominal delta time using
 * the secondary constructor: {@link SwerveDrivePoseEstimator#SwerveDrivePoseEstimator(Nat, Nat,
 * Nat, Rotation2d, Pose2d, SwerveModulePosition[], SwerveDriveKinematics, Matrix, Matrix, Matrix,
 * double)}.
 *
 * <p>{@link SwerveDrivePoseEstimator#addVisionMeasurement} can be called as infrequently as you
 * want; if you never call it, then this class will behave mostly like regular encoder odometry.
 *
 * <p>The state-space system used internally has the following states (x), inputs (u), and outputs
 * (y):
 *
 * <p><strong> x = [x, y, theta, s_0, ..., s_n]ᵀ </strong> in the field coordinate system containing
 * x position, y position, and heading, followed by the distance travelled by each wheel.
 *
 * <p><strong> u = [v_x, v_y, omega, v_0, ... v_n]ᵀ </strong> containing x velocity, y velocity, and
 * angular rate in the field coordinate system, followed by the velocity measured at each wheel.
 *
 * <p><strong> y = [x, y, theta]ᵀ </strong> from vision containing x position, y position, and
 * heading; or <strong> y = [theta, s_0, ..., s_n]ᵀ </strong> containing gyro heading, followed by
 * the distance travelled by each wheel.
 */
public class SwerveDrivePoseEstimator<States extends Num, Inputs extends Num, Outputs extends Num> {
  private final UnscentedKalmanFilter<States, Inputs, Outputs> m_observer;
  private final SwerveDriveKinematics m_kinematics;
  private final BiConsumer<Matrix<Inputs, N1>, Matrix<N3, N1>> m_visionCorrect;
  private final TimeInterpolatableBuffer<Pose2d> m_poseBuffer;

  private final Nat<States> m_states;
  private final Nat<Inputs> m_inputs;
  private final Nat<Outputs> m_outputs;

  private final double m_nominalDt; // Seconds
  private double m_prevTimeSeconds = -1.0;

  private Rotation2d m_gyroOffset;
  private Rotation2d m_previousAngle;

  private Matrix<N3, N3> m_visionContR;

  /**
   * Constructs a SwerveDrivePoseEstimator.
   *
   * @param states The size of the state vector.
   * @param inputs The size of the input vector.
   * @param outputs The size of the outputs vector.
   * @param gyroAngle The current gyro angle.
   * @param initialPoseMeters The starting pose estimate.
   * @param modulePositions The current distance measurements and rotations of the swerve modules.
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param stateStdDevs Standard deviations of model states. Increase these numbers to trust your
   *     model's state estimates less. This matrix is in the form [x, y, theta, s_0, ... s_n]ᵀ, with
   *     units in meters and radians, then meters.
   * @param localMeasurementStdDevs Standard deviations of the encoder and gyro measurements.
   *     Increase these numbers to trust sensor readings from encoders and gyros less. This matrix
   *     is in the form [theta, s_0, ... s_n], with units in radians followed by meters.
   * @param visionMeasurementStdDevs Standard deviations of the vision measurements. Increase these
   *     numbers to trust global measurements from vision less. This matrix is in the form [x, y,
   *     theta]ᵀ, with units in meters and radians.
   */
  public SwerveDrivePoseEstimator(
      Nat<States> states,
      Nat<Inputs> inputs,
      Nat<Outputs> outputs,
      Rotation2d gyroAngle,
      SwerveModulePosition[] modulePositions,
      Pose2d initialPoseMeters,
      SwerveDriveKinematics kinematics,
      Matrix<States, N1> stateStdDevs,
      Matrix<Outputs, N1> localMeasurementStdDevs,
      Matrix<N3, N1> visionMeasurementStdDevs) {
    this(
        states,
        inputs,
        outputs,
        gyroAngle,
        modulePositions,
        initialPoseMeters,
        kinematics,
        stateStdDevs,
        localMeasurementStdDevs,
        visionMeasurementStdDevs,
        0.02);
  }

  /**
   * Constructs a SwerveDrivePoseEstimator.
   *
   * @param states The size of the state vector.
   * @param inputs The size of the input vector.
   * @param outputs The size of the outputs vector.
   * @param gyroAngle The current gyro angle.
   * @param modulePositions The current distance measurements and rotations of the swerve modules.
   * @param initialPoseMeters The starting pose estimate.
   * @param kinematics A correctly-configured kinematics object for your drivetrain.
   * @param stateStdDevs Standard deviations of model states. Increase these numbers to trust your
   *     model's state estimates less. This matrix is in the form [x, y, theta, s_0, ... s_n]ᵀ, with
   *     units in meters and radians, then meters.
   * @param localMeasurementStdDevs Standard deviations of the encoder and gyro measurements.
   *     Increase these numbers to trust sensor readings from encoders and gyros less. This matrix
   *     is in the form [theta, s_0, ... s_n], with units in radians followed by meters.
   * @param visionMeasurementStdDevs Standard deviations of the vision measurements. Increase these
   *     numbers to trust global measurements from vision less. This matrix is in the form [x, y,
   *     theta]ᵀ, with units in meters and radians.
   * @param nominalDtSeconds The time in seconds between each robot loop.
   */
  public SwerveDrivePoseEstimator(
      Nat<States> states,
      Nat<Inputs> inputs,
      Nat<Outputs> outputs,
      Rotation2d gyroAngle,
      SwerveModulePosition[] modulePositions,
      Pose2d initialPoseMeters,
      SwerveDriveKinematics kinematics,
      Matrix<States, N1> stateStdDevs,
      Matrix<Outputs, N1> localMeasurementStdDevs,
      Matrix<N3, N1> visionMeasurementStdDevs,
      double nominalDtSeconds) {
    this.m_states = states;
    this.m_inputs = inputs;
    this.m_outputs = outputs;

    if (states.getNum() != modulePositions.length + 3) {
      throw new IllegalArgumentException(
          String.format(
              "Number of states (%s) must be 3 + "
                  + "the number of modules provided in constructor (%s).",
              states.getNum(), modulePositions.length));
    }

    if (inputs.getNum() != modulePositions.length + 3) {
      throw new IllegalArgumentException(
          String.format(
              "Number of inputs (%s) must be 3 + "
                  + "the number of modules provided in constructor (%s).",
              inputs.getNum(), modulePositions.length));
    }

    if (outputs.getNum() != modulePositions.length + 1) {
      throw new IllegalArgumentException(
          String.format(
              "Number of outputs (%s) must be 3 + "
                  + "the number of modules provided in constructor (%s).",
              outputs.getNum(), modulePositions.length));
    }

    m_nominalDt = nominalDtSeconds;

    m_observer =
        new UnscentedKalmanFilter<>(
            states,
            outputs,
            (x, u) -> u.block(states.getNum(), 1, 0, 0),
            (x, u) -> x.block(states.getNum() - 2, 1, 2, 0),
            stateStdDevs,
            localMeasurementStdDevs,
            AngleStatistics.angleMean(2),
            AngleStatistics.angleMean(0),
            AngleStatistics.angleResidual(2),
            AngleStatistics.angleResidual(0),
            AngleStatistics.angleAdd(2),
            m_nominalDt);
    m_kinematics = kinematics;
    m_poseBuffer = TimeInterpolatableBuffer.createBuffer(1.5);

    // Initialize vision R
    setVisionMeasurementStdDevs(visionMeasurementStdDevs);

    m_visionCorrect =
        (u, y) ->
            m_observer.correct(
                Nat.N3(),
                u,
                y,
                (x, u1) -> x.block(3, 1, 0, 0),
                m_visionContR,
                AngleStatistics.angleMean(2),
                AngleStatistics.angleResidual(2),
                AngleStatistics.angleResidual(2),
                AngleStatistics.angleAdd(2));

    m_gyroOffset = initialPoseMeters.getRotation().minus(gyroAngle);
    m_previousAngle = initialPoseMeters.getRotation();

    var poseVec = StateSpaceUtil.poseTo3dVector(initialPoseMeters);
    Matrix<States, N1> xhat = new Matrix<States, N1>(states, Nat.N1());
    xhat.set(0, 0, poseVec.get(0, 0));
    xhat.set(1, 0, poseVec.get(1, 0));
    xhat.set(2, 0, poseVec.get(2, 0));

    for (int index = 3; index < states.getNum(); index++) {
      xhat.set(index, 0, modulePositions[index - 3].distanceMeters);
    }

    m_observer.setXhat(xhat);
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
    m_observer.reset();
    m_poseBuffer.clear();

    var poseVec = StateSpaceUtil.poseTo3dVector(poseMeters);
    Matrix<States, N1> xhat = new Matrix<States, N1>(m_states, Nat.N1());
    xhat.set(0, 0, poseVec.get(0, 0));
    xhat.set(1, 0, poseVec.get(1, 0));
    xhat.set(2, 0, poseVec.get(2, 0));

    for (int index = 3; index < m_states.getNum(); index++) {
      xhat.set(index, 0, modulePositions[index - 3].distanceMeters);
    }

    m_observer.setXhat(xhat);

    m_prevTimeSeconds = -1;

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
   * SwerveDrivePoseEstimator#update} every loop.
   *
   * <p>To promote stability of the pose estimate and make it robust to bad vision data, we
   * recommend only adding vision measurements that are already within one meter or so of the
   * current pose estimate.
   *
   * @param visionRobotPoseMeters The pose of the robot as measured by the vision camera.
   * @param timestampSeconds The timestamp of the vision measurement in seconds. Note that if you
   *     don't use your own time source by calling {@link SwerveDrivePoseEstimator#updateWithTime}
   *     then you must use a timestamp with an epoch since FPGA startup (i.e. the epoch of this
   *     timestamp is the same epoch as Timer.getFPGATimestamp.) This means that you should use
   *     Timer.getFPGATimestamp as your time source or sync the epochs.
   */
  public void addVisionMeasurement(Pose2d visionRobotPoseMeters, double timestampSeconds) {
    var sample = m_poseBuffer.getSample(timestampSeconds);
    if (sample.isPresent()) {
      m_visionCorrect.accept(
          new Matrix<Inputs, N1>(m_inputs, Nat.N1()),
          StateSpaceUtil.poseTo3dVector(
              getEstimatedPosition().transformBy(visionRobotPoseMeters.minus(sample.get()))));
    }
  }

  /**
   * Add a vision measurement to the Unscented Kalman Filter. This will correct the odometry pose
   * estimate while still accounting for measurement noise.
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
   *     don't use your own time source by calling {@link SwerveDrivePoseEstimator#updateWithTime}
   *     then you must use a timestamp with an epoch since FPGA startup (i.e. the epoch of this
   *     timestamp is the same epoch as Timer.getFPGATimestamp.) This means that you should use
   *     Timer.getFPGATimestamp as your time source in this case.
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
   * Updates the the Unscented Kalman Filter using only wheel encoder information. This should be
   * called every loop, and the correct loop period must be passed into the constructor of this
   * class.
   *
   * @param gyroAngle The current gyro angle.
   * @param moduleStates The current velocities and rotations of the swerve modules.
   * @param modulePositions The current distance measurements and rotations of the swerve modules.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d update(
      Rotation2d gyroAngle,
      SwerveModuleState[] moduleStates,
      SwerveModulePosition[] modulePositions) {
    return updateWithTime(WPIUtilJNI.now() * 1.0e-6, gyroAngle, moduleStates, modulePositions);
  }

  /**
   * Updates the the Unscented Kalman Filter using only wheel encoder information. This should be
   * called every loop, and the correct loop period must be passed into the constructor of this
   * class.
   *
   * @param currentTimeSeconds Time at which this method was called, in seconds.
   * @param gyroAngle The current gyroscope angle.
   * @param moduleStates The current velocities and rotations of the swerve modules.
   * @param modulePositions The current distance measurements and rotations of the swerve modules.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d updateWithTime(
      double currentTimeSeconds,
      Rotation2d gyroAngle,
      SwerveModuleState[] moduleStates,
      SwerveModulePosition[] modulePositions) {
    double dt = m_prevTimeSeconds >= 0 ? currentTimeSeconds - m_prevTimeSeconds : m_nominalDt;
    m_prevTimeSeconds = currentTimeSeconds;

    var angle = gyroAngle.plus(m_gyroOffset);
    var omega = angle.minus(m_previousAngle).getRadians() / dt;

    var chassisSpeeds = m_kinematics.toChassisSpeeds(moduleStates);
    var fieldRelativeVelocities =
        new Translation2d(chassisSpeeds.vxMetersPerSecond, chassisSpeeds.vyMetersPerSecond)
            .rotateBy(angle);

    var u = new Matrix<Inputs, N1>(m_inputs, Nat.N1());

    u.set(0, 0, fieldRelativeVelocities.getX());
    u.set(1, 0, fieldRelativeVelocities.getY());
    u.set(2, 0, omega);
    for (int index = 3; index < m_inputs.getNum(); index++) {
      u.set(index, 0, moduleStates[index - 3].speedMetersPerSecond);
    }

    m_previousAngle = angle;

    var localY = new Matrix<Outputs, N1>(m_outputs, Nat.N1());
    localY.set(0, 0, angle.getRadians());
    for (int index = 1; index < m_outputs.getNum(); index++) {
      localY.set(index, 0, modulePositions[index - 1].distanceMeters);
    }

    m_poseBuffer.addSample(currentTimeSeconds, getEstimatedPosition());
    m_observer.predict(u, dt);
    m_observer.correct(u, localY);

    return getEstimatedPosition();
  }
}
