package edu.wpi.first.wpilibj.estimator;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.geometry.*;
import edu.wpi.first.wpilibj.kinematics.SwerveDriveKinematics;
import edu.wpi.first.wpilibj.kinematics.SwerveModuleState;
import edu.wpi.first.wpilibj.math.StateSpaceUtils;
import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpiutil.math.MatBuilder;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.MatrixUtils;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N3;

public class SwerveDrivePoseEstimator {
  private final KalmanFilter<N3, N3, N3> m_observer;
  private final SwerveDriveKinematics m_kinematics;
  private final KalmanFilterLatencyCompensator<N3, N3, N3> m_latencyCompensator;

  private final double m_nominalDt; // Seconds
  private double m_prevTimeSeconds = -1.0;

  private Rotation2d m_gyroOffset;
  private Rotation2d m_previousAngle;

  /**
   * Constructs a SwerveDrivePose estimator.
   *
   * @param gyroAngle          The current gyro angle.
   * @param initialPoseMeters  The starting pose estimate.
   * @param stateStdDevs       Standard deviations of model states. Increase these numbers to trust
   *                           your encoders less.
   * @param measurementStdDevs Standard deviations of the measurements. Increase these numbers to
   *                           trust vision less.
   */
  public SwerveDrivePoseEstimator(
          Rotation2d gyroAngle, Pose2d initialPoseMeters, SwerveDriveKinematics kinematics,
          Matrix<N3, N1> stateStdDevs, Matrix<N3, N1> measurementStdDevs
  ) {
    this(gyroAngle, initialPoseMeters, kinematics, stateStdDevs, measurementStdDevs, 0.02);
  }

  /**
   * Constructs a SwerveDrivePose estimator.
   *
   * @param gyroAngle          The current gyro angle.
   * @param initialPoseMeters  The starting pose estimate.
   * @param stateStdDevs       Standard deviations of model states. Increase these numbers to trust
   *                           your encoders less.
   * @param measurementStdDevs Standard deviations of the measurements. Increase these numbers to
   *                           trust vision less.
   * @param nominalDtSeconds   The time in seconds between each robot loop.
   */
  public SwerveDrivePoseEstimator(
          Rotation2d gyroAngle, Pose2d initialPoseMeters, SwerveDriveKinematics kinematics,
          Matrix<N3, N1> stateStdDevs, Matrix<N3, N1> measurementStdDevs, double nominalDtSeconds
  ) {
    m_nominalDt = nominalDtSeconds;

    var observerSystem = new LinearSystem<>(
            Nat.N3(), Nat.N3(), Nat.N3(),
            MatrixUtils.zeros(Nat.N3(), Nat.N3()), // A
            MatrixUtils.eye(Nat.N3()), // B
            MatrixUtils.eye(Nat.N3()), // C
            MatrixUtils.zeros(Nat.N3(), Nat.N3()), // D
            new MatBuilder<>(Nat.N3(), Nat.N1()).fill( // uMin
                    Double.NEGATIVE_INFINITY, Double.NEGATIVE_INFINITY, Double.NEGATIVE_INFINITY
            ),
            new MatBuilder<>(Nat.N3(), Nat.N1()).fill( // uMax
                    Double.POSITIVE_INFINITY, Double.POSITIVE_INFINITY, Double.POSITIVE_INFINITY
            )
    );
    m_observer = new KalmanFilter<>(Nat.N3(), Nat.N3(), observerSystem, stateStdDevs,
            measurementStdDevs, nominalDtSeconds);
    m_kinematics = kinematics;
    m_latencyCompensator = new KalmanFilterLatencyCompensator<>();

    m_gyroOffset = initialPoseMeters.getRotation().minus(gyroAngle);
    m_previousAngle = initialPoseMeters.getRotation();
    m_observer.setXhat(StateSpaceUtils.poseToVector(initialPoseMeters));
  }

  /**
   * Resets the robot's position on the field.
   *
   * <p>You NEED to reset your encoders (to zero) when calling this method.
   *
   * <p>The gyroscope angle does not need to be reset here on the user's robot code.
   * The library automatically takes care of offsetting the gyro angle.
   *
   * @param poseMeters The position on the field that your robot is at.
   * @param gyroAngle  The angle reported by the gyroscope.
   */
  public void resetPosition(Pose2d poseMeters, Rotation2d gyroAngle) {
    m_previousAngle = poseMeters.getRotation();
    m_gyroOffset = getEstimatedPosition().getRotation().minus(gyroAngle);
  }

  /**
   * Gets the pose of the robot at the current time as estimated by the Extended Kalman Filter.
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
   * Add a vision measurement to the Extended Kalman Filter. This will correct the
   * odometry pose estimate while still accounting for measurement noise.
   *
   * <p>This method can be called as infrequently as you want, as long as you are
   * calling {@link DifferentialDrivePoseEstimator#update} every loop.
   *
   * @param visionRobotPoseMeters The pose of the robot as measured by the vision
   *                              camera.
   * @param timestampSeconds      The timestamp of the vision measurement in seconds. Note that if
   *                              you don't use your own time source by calling
   *                              {@link SwerveDrivePoseEstimator#updateWithTime} then you
   *                              must use a timestamp with an epoch since FPGA startup
   *                              (i.e. the epoch of this timestamp is the same epoch as
   *                              {@link edu.wpi.first.wpilibj.Timer#getFPGATimestamp
   *                              Timer.getFPGATimestamp}.) This means that you should
   *                              use Timer.getFPGATimestamp as your time source in
   *                              this case.
   */
  public void addVisionMeasurement(Pose2d visionRobotPoseMeters, double timestampSeconds) {
    m_latencyCompensator.applyPastMeasurement(
            m_observer, m_nominalDt,
            StateSpaceUtils.poseToVector(visionRobotPoseMeters), timestampSeconds
    );
  }

  /**
   * Updates the the Extended Kalman Filter using only wheel encoder information.
   * Note that this should be called every loop.
   *
   * @param gyroAngle   The current gyro angle.
   * @param wheelStates Velocities and rotations of the swerve modules.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d update(
          Rotation2d gyroAngle,
          SwerveModuleState... wheelStates
  ) {
    return updateWithTime(Timer.getFPGATimestamp(), gyroAngle, wheelStates);
  }

  /**
   * Updates the the Kalman Filter using only wheel encoder information.
   * Note that this should be called every loop (and the correct loop period must
   * be passed into the constructor of this class.)
   *
   * @param currentTimeSeconds Time at which this method was called, in seconds.
   * @param gyroAngle          The current gyroscope angle.
   * @param wheelStates        Velocities and rotations of the swerve modules.
   * @return The estimated pose of the robot in meters.
   */
  @SuppressWarnings("LocalVariableName")
  public Pose2d updateWithTime(
          double currentTimeSeconds,
          Rotation2d gyroAngle, SwerveModuleState... wheelStates
  ) {
    double dt = m_prevTimeSeconds >= 0 ? currentTimeSeconds - m_prevTimeSeconds : m_nominalDt;
    m_prevTimeSeconds = currentTimeSeconds;

    var angle = gyroAngle.plus(m_gyroOffset);
    var omega = angle.minus(m_previousAngle).getRadians() / dt;

    var chassisSpeeds = m_kinematics.toChassisSpeeds(wheelStates);
    var fieldRelativeVelocities = new Translation2d(
            chassisSpeeds.vxMetersPerSecond, chassisSpeeds.vyMetersPerSecond
    ).rotateBy(angle);

    var u = new MatBuilder<>(Nat.N3(), Nat.N1()).fill(
            fieldRelativeVelocities.getX(),
            fieldRelativeVelocities.getY(),
            omega
    );
    m_previousAngle = angle;

    m_latencyCompensator.addObserverState(m_observer, u, currentTimeSeconds);
    m_observer.predict(u, dt);

    return new Pose2d(
            m_observer.getXhat(0),
            m_observer.getXhat(1),
            new Rotation2d(m_observer.getXhat(2))
    );
  }
}
