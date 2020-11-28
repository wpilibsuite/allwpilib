/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.estimator;

import java.util.function.BiConsumer;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;
import edu.wpi.first.wpilibj.kinematics.MecanumDriveKinematics;
import edu.wpi.first.wpilibj.kinematics.MecanumDriveWheelSpeeds;
import edu.wpi.first.wpilibj.math.Discretization;
import edu.wpi.first.wpilibj.math.StateSpaceUtil;
import edu.wpi.first.wpiutil.WPIUtilJNI;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N3;

/**
 * This class wraps an {@link UnscentedKalmanFilter Unscented Kalman Filter} to fuse
 * latency-compensated vision measurements with mecanum drive encoder velocity measurements.
 * It will correct for noisy measurements and encoder drift. It is intended to be an easy
 * but more accurate drop-in for {@link edu.wpi.first.wpilibj.kinematics.MecanumDriveOdometry}.
 *
 * <p>{@link MecanumDrivePoseEstimator#update} should be called every robot loop. If
 * your loops are faster or slower than the default of 0.02s, then you should change
 * the nominal delta time using the secondary constructor:
 * {@link MecanumDrivePoseEstimator#MecanumDrivePoseEstimator(Rotation2d, Pose2d,
 * MecanumDriveKinematics, Matrix, Matrix, Matrix, double)}.
 *
 * <p>{@link MecanumDrivePoseEstimator#addVisionMeasurement} can be called as
 * infrequently as you want; if you never call it, then this class will behave mostly like regular
 * encoder odometry.
 *
 * <p>Our state-space system is:
 *
 * <p><strong> x = [[x, y, theta]]^T </strong> in the field-coordinate system.
 *
 * <p><strong> u = [[vx, vy, theta]]^T </strong> in the field-coordinate system.
 *
 * <p><strong> y = [[x, y, theta]]^T </strong> in field coords from vision,
 * or <strong> y = [[theta]]^T </strong> from the gyro.
 */
public class MecanumDrivePoseEstimator {
  private final UnscentedKalmanFilter<N3, N3, N1> m_observer;
  private final MecanumDriveKinematics m_kinematics;
  private final BiConsumer<Matrix<N3, N1>, Matrix<N3, N1>> m_visionCorrect;
  private final KalmanFilterLatencyCompensator<N3, N3, N1> m_latencyCompensator;

  private final double m_nominalDt; // Seconds
  private double m_prevTimeSeconds = -1.0;

  private Rotation2d m_gyroOffset;
  private Rotation2d m_previousAngle;

  /**
   * Constructs a MecanumDrivePoseEstimator.
   *
   * @param gyroAngle                The current gyro angle.
   * @param initialPoseMeters        The starting pose estimate.
   * @param kinematics               A correctly-configured kinematics object for your drivetrain.
   * @param stateStdDevs             Standard deviations of model states. Increase these numbers to
   *                                 trust your wheel and gyro velocities less.
   * @param localMeasurementStdDevs  Standard deviations of the gyro measurement. Increase this
   *                                 number to trust gyro angle measurements less.
   * @param visionMeasurementStdDevs Standard deviations of the vision measurements. Increase
   *                                 these numbers to trust vision less.
   */
  public MecanumDrivePoseEstimator(
          Rotation2d gyroAngle, Pose2d initialPoseMeters, MecanumDriveKinematics kinematics,
          Matrix<N3, N1> stateStdDevs, Matrix<N1, N1> localMeasurementStdDevs,
          Matrix<N3, N1> visionMeasurementStdDevs
  ) {
    this(gyroAngle, initialPoseMeters, kinematics, stateStdDevs, localMeasurementStdDevs,
            visionMeasurementStdDevs, 0.02);
  }

  /**
   * Constructs a MecanumDrivePoseEstimator.
   *
   * @param gyroAngle                The current gyro angle.
   * @param initialPoseMeters        The starting pose estimate.
   * @param kinematics               A correctly-configured kinematics object for your drivetrain.
   * @param stateStdDevs             Standard deviations of model states. Increase these numbers to
   *                                 trust your wheel and gyro velocities less.
   * @param localMeasurementStdDevs  Standard deviations of the gyro measurement. Increase this
   *                                 number to trust gyro angle measurements less.
   * @param visionMeasurementStdDevs Standard deviations of the vision measurements. Increase
   *                                 these numbers to trust vision less.
   * @param nominalDtSeconds         The time in seconds between each robot loop.
   */
  @SuppressWarnings("ParameterName")
  public MecanumDrivePoseEstimator(
          Rotation2d gyroAngle, Pose2d initialPoseMeters, MecanumDriveKinematics kinematics,
          Matrix<N3, N1> stateStdDevs, Matrix<N1, N1> localMeasurementStdDevs,
          Matrix<N3, N1> visionMeasurementStdDevs, double nominalDtSeconds
  ) {
    m_nominalDt = nominalDtSeconds;

    m_observer = new UnscentedKalmanFilter<>(
        Nat.N3(), Nat.N1(),
        (x_, u) -> u,
        (x, u_) -> x.extractRowVector(2),
        stateStdDevs,
        localMeasurementStdDevs,
        AngleStatistics.angleMean(2),
        AngleStatistics.angleMean(0),
        AngleStatistics.angleResidual(2),
        AngleStatistics.angleResidual(0),
        AngleStatistics.angleAdd(2),
        m_nominalDt
    );
    m_kinematics = kinematics;
    m_latencyCompensator = new KalmanFilterLatencyCompensator<>();

    var visionContR = StateSpaceUtil.makeCovarianceMatrix(Nat.N3(), visionMeasurementStdDevs);
    var visionDiscR = Discretization.discretizeR(visionContR, m_nominalDt);

    m_visionCorrect = (u, y) -> m_observer.correct(
        Nat.N3(), u, y,
        (x, u_) -> x,
        visionDiscR,
        AngleStatistics.angleMean(2),
        AngleStatistics.angleResidual(2),
        AngleStatistics.angleResidual(2),
        AngleStatistics.angleAdd(2)
    );

    m_gyroOffset = initialPoseMeters.getRotation().minus(gyroAngle);
    m_previousAngle = initialPoseMeters.getRotation();
    m_observer.setXhat(StateSpaceUtil.poseTo3dVector(initialPoseMeters));
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>You NEED to reset your encoders (to zero) when calling this method.
   *
   * <p>The gyroscope angle does not need to be reset in the user's robot code.
   * The library automatically takes care of offsetting the gyro angle.
   *
   * @param poseMeters The position on the field that your robot is at.
   * @param gyroAngle  The angle reported by the gyroscope.
   */
  public void resetPosition(Pose2d poseMeters, Rotation2d gyroAngle) {
    m_previousAngle = poseMeters.getRotation();
    m_gyroOffset = getEstimatedPosition().getRotation().minus(gyroAngle);
    m_observer.setXhat(StateSpaceUtil.poseTo3dVector(poseMeters));
  }

  /**
   * Gets the pose of the robot at the current time as estimated by the Unscented Kalman Filter.
   *
   * @return The estimated robot pose in meters.
   */
  public Pose2d getEstimatedPosition() {
    return new Pose2d(
            m_observer.getXhat(0),
            m_observer.getXhat(1),
            new Rotation2d(m_observer.getXhat(2))
    );
  }

  /**
   * Add a vision measurement to the Unscented Kalman Filter. This will correct the
   * odometry pose estimate while still accounting for measurement noise.
   *
   * <p>This method can be called as infrequently as you want, as long as you are
   * calling {@link MecanumDrivePoseEstimator#update} every loop.
   *
   * @param visionRobotPoseMeters The pose of the robot as measured by the vision
   *                              camera.
   * @param timestampSeconds      The timestamp of the vision measurement in seconds. Note that if
   *                              you don't use your own time source by calling
   *                              {@link MecanumDrivePoseEstimator#updateWithTime} then you
   *                              must use a timestamp with an epoch since FPGA startup
   *                              (i.e. the epoch of this timestamp is the same epoch as
   *                              Timer.getFPGATimestamp.) This means that you should
   *                              use Timer.getFPGATimestamp as your time source
   *                              or sync the epochs.
   */
  public void addVisionMeasurement(Pose2d visionRobotPoseMeters, double timestampSeconds) {
    m_latencyCompensator.applyPastGlobalMeasurement(
            Nat.N3(),
            m_observer, m_nominalDt,
            StateSpaceUtil.poseTo3dVector(visionRobotPoseMeters),
            m_visionCorrect,
            timestampSeconds
    );
  }

  /**
   * Updates the the Unscented Kalman Filter using only wheel encoder information.
   * This should be called every loop, and the correct loop period must be passed
   * into the constructor of this class.
   *
   * @param gyroAngle   The current gyro angle.
   * @param wheelSpeeds The current speeds of the mecanum drive wheels.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d update(Rotation2d gyroAngle, MecanumDriveWheelSpeeds wheelSpeeds) {
    return updateWithTime(WPIUtilJNI.now() * 1.0e-6, gyroAngle, wheelSpeeds);
  }

  /**
   * Updates the the Unscented Kalman Filter using only wheel encoder information.
   * This should be called every loop, and the correct loop period must be passed
   * into the constructor of this class.
   *
   * @param currentTimeSeconds Time at which this method was called, in seconds.
   * @param gyroAngle          The current gyroscope angle.
   * @param wheelSpeeds        The current speeds of the mecanum drive wheels.
   * @return The estimated pose of the robot in meters.
   */
  @SuppressWarnings("LocalVariableName")
  public Pose2d updateWithTime(double currentTimeSeconds, Rotation2d gyroAngle,
                               MecanumDriveWheelSpeeds wheelSpeeds) {
    double dt = m_prevTimeSeconds >= 0 ? currentTimeSeconds - m_prevTimeSeconds : m_nominalDt;
    m_prevTimeSeconds = currentTimeSeconds;

    var angle = gyroAngle.plus(m_gyroOffset);
    var omega = angle.minus(m_previousAngle).getRadians() / dt;

    var chassisSpeeds = m_kinematics.toChassisSpeeds(wheelSpeeds);
    var fieldRelativeVelocities =
            new Translation2d(chassisSpeeds.vxMetersPerSecond, chassisSpeeds.vyMetersPerSecond)
                    .rotateBy(angle);

    var u = VecBuilder.fill(
            fieldRelativeVelocities.getX(),
            fieldRelativeVelocities.getY(),
            omega
    );
    m_previousAngle = angle;

    var localY = VecBuilder.fill(angle.getRadians());
    m_latencyCompensator.addObserverState(m_observer, u, localY, currentTimeSeconds);
    m_observer.predict(u, dt);
    m_observer.correct(u, localY);

    return getEstimatedPosition();
  }
}
