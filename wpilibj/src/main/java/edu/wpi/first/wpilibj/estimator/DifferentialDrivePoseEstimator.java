package edu.wpi.first.wpilibj.estimator;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveWheelSpeeds;
import edu.wpi.first.wpilibj.math.StateSpaceUtils;
import edu.wpi.first.wpiutil.math.MatBuilder;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N3;

/**
 * This class wraps an Extended Kalman Filter to fuse latency-compensated vision
 * measurements with differential drive encoder measurements. It will correct
 * for noisy vision measurements and encoder drift. It is intended to be an easy
 * drop-in for
 * {@link edu.wpi.first.wpilibj.kinematics.DifferentialDriveOdometry}; in fact,
 * if you never call {@link DifferentialDrivePoseEstimator#addVisionMeasurement}
 * and only call {@link DifferentialDrivePoseEstimator#update} then this will
 * behave exactly the same as DifferentialDriveOdometry.
 *
 * <p>{@link DifferentialDrivePoseEstimator#update} should be called every robot
 * loop (if your robot loops are faster than the default then you should change
 * the
 * {@link DifferentialDrivePoseEstimator#DifferentialDrivePoseEstimator(Rotation2d,
 * Pose2d, Matrix, Matrix, double) nominal delta time}.)
 * {@link DifferentialDrivePoseEstimator#addVisionMeasurement} can be called as
 * infrequently as you want; if you never call it then this class will behave
 * exactly like regular encoder odometry.
 *
 * <p>Our state-space system is:
 *
 * <p>x = [[x, y, dtheta]]^T in the field coordinate system.
 *
 * <p>u = [[vx, vy, omega]]^T (robot-relative velocities) -- NB: using velocities make things
 * considerably easier, because it means that teams don't have to worry about getting an accurate
 * model. Basically, we suspect that it's easier for teams to get good encoder data than it is for
 * them to perform system identification well enough to get a good model.
 *
 * <p>y = [[x, y, theta]]^T from vision
 */
public class DifferentialDrivePoseEstimator {
  private final UnscentedKalmanFilter<N3, N3, N3> m_observer;
  private final KalmanFilterLatencyCompensator<N3, N3, N3> m_latencyCompensator;

  private final double m_nominalDt; // Seconds
  private double m_prevTimeSeconds = -1.0;

  private Rotation2d m_gyroOffset;
  private Rotation2d m_previousAngle;

  /**
   * Constructs a DifferentialDrivePose estimator.
   *
   * @param gyroAngle          The current gyro angle.
   * @param initialPoseMeters  The starting pose estimate.
   * @param stateStdDevs       Standard deviations of model states. Increase these numbers to trust
   *                           your encoders less.
   * @param measurementStdDevs Standard deviations of the measurements. Increase these numbers to
   *                           trust vision less.
   */
  public DifferentialDrivePoseEstimator(
          Rotation2d gyroAngle, Pose2d initialPoseMeters,
          Matrix<N3, N1> stateStdDevs, Matrix<N3, N1> measurementStdDevs
  ) {
    this(gyroAngle, initialPoseMeters, stateStdDevs, measurementStdDevs, 0.02);
  }

  /**
   * Constructs a DifferentialDrivePose estimator.
   *
   * @param gyroAngle          The current gyro angle.
   * @param initialPoseMeters  The starting pose estimate.
   * @param stateStdDevs       Standard deviations of model states. Increase these numbers to trust
   *                           your encoders less.
   * @param measurementStdDevs Standard deviations of the measurements. Increase these numbers to
   *                           trust vision less.
   * @param nominalDtSeconds   The time in seconds between each robot loop.
   */
  @SuppressWarnings("ParameterName")
  public DifferentialDrivePoseEstimator(
          Rotation2d gyroAngle, Pose2d initialPoseMeters,
          Matrix<N3, N1> stateStdDevs, Matrix<N3, N1> measurementStdDevs,
          double nominalDtSeconds
  ) {
    m_nominalDt = nominalDtSeconds;

    m_observer = new UnscentedKalmanFilter<>(
            Nat.N3(), Nat.N3(), Nat.N3(),
            this::f, (x, u) -> x,
            stateStdDevs, measurementStdDevs,
            m_nominalDt
    );
    m_latencyCompensator = new KalmanFilterLatencyCompensator<>();

    m_gyroOffset = initialPoseMeters.getRotation().minus(gyroAngle);
    m_previousAngle = initialPoseMeters.getRotation();
    m_observer.setXhat(StateSpaceUtils.poseToVector(initialPoseMeters));
  }

  @SuppressWarnings({"ParameterName", "MethodName"})
  private Matrix<N3, N1> f(Matrix<N3, N1> x, Matrix<N3, N1> u) {
    // Apply a rotation matrix. Note that we do *not* add x--Runge-Kutta does that for us.
    var theta = x.get(2, 0);
    var toFieldRotation = new MatBuilder<>(Nat.N3(), Nat.N3()).fill(
            Math.cos(theta), -Math.sin(theta), 0,
            Math.sin(theta), Math.cos(theta), 0,
            0, 0, 1
    );
    return toFieldRotation.times(u);
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
   *                              {@link DifferentialDrivePoseEstimator#updateWithTime} then you
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
   * @param gyroAngle                      The current gyro angle.
   * @param wheelVelocitiesMetersPerSecond The velocities of the wheels in meters per second.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d update(
          Rotation2d gyroAngle,
          DifferentialDriveWheelSpeeds wheelVelocitiesMetersPerSecond
  ) {
    return updateWithTime(Timer.getFPGATimestamp(), gyroAngle, wheelVelocitiesMetersPerSecond);
  }

  /**
   * Updates the the Extended Kalman Filter using only wheel encoder information.
   * Note that this should be called every loop.
   *
   * @param currentTimeSeconds             Time at which this method was called, in seconds.
   * @param gyroAngle                      The current gyro angle.
   * @param wheelVelocitiesMetersPerSecond The velocities of the wheels in meters per second.
   * @return The estimated pose of the robot in meters.
   */
  @SuppressWarnings("LocalVariableName")
  public Pose2d updateWithTime(
          double currentTimeSeconds, Rotation2d gyroAngle,
          DifferentialDriveWheelSpeeds wheelVelocitiesMetersPerSecond
  ) {
    double dt = m_prevTimeSeconds >= 0 ? currentTimeSeconds - m_prevTimeSeconds : m_nominalDt;
    m_prevTimeSeconds = currentTimeSeconds;

    var angle = gyroAngle.plus(m_gyroOffset);
    // Diff drive forward kinematics:
    // v_c = (v_l + v_r) / 2
    var wheelVels = wheelVelocitiesMetersPerSecond;
    var u = VecBuilder.fill(
            (wheelVels.leftMetersPerSecond + wheelVels.rightMetersPerSecond) / 2, 0,
            angle.minus(m_previousAngle).getRadians() / dt
    );
    m_previousAngle = angle;

    m_latencyCompensator.addObserverState(m_observer, u, currentTimeSeconds);
    m_observer.predict(u, dt);

    return getEstimatedPosition();
  }
}
