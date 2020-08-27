/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.estimator;

import java.util.function.BiConsumer;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveWheelSpeeds;
import edu.wpi.first.wpilibj.math.Discretization;
import edu.wpi.first.wpilibj.math.StateSpaceUtil;
import edu.wpi.first.wpiutil.math.MatBuilder;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N3;
import edu.wpi.first.wpiutil.math.numbers.N4;
import edu.wpi.first.wpiutil.math.numbers.N5;
import edu.wpi.first.wpiutil.math.numbers.N6;

/**
 * This class wraps an
 * {@link edu.wpi.first.wpilibj.estimator.UnscentedKalmanFilter Extended Kalman Filter}
 * to fuse latency-compensated vision
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
 * the {@link DifferentialDrivePoseEstimator#DifferentialDrivePoseEstimator(Rotation2d, Pose2d,
 * Matrix, Matrix, Matrix, double) nominal delta time}.)
 * {@link DifferentialDrivePoseEstimator#addVisionMeasurement} can be called as
 * infrequently as you want; if you never call it then this class will behave
 * exactly like regular encoder odometry.
 *
 * <p>Our state-space system is:
 *
 * <p><strong> x = [[x, y, cos(theta), sin(theta), dist_l, dist_r]]^T </strong>
 * in the field coordinate system (dist_* are wheel distances.)
 *
 * <p><strong> u = [[v_left, v_right, omega]]^T </strong> (robot-relative velocities)
 *  -- NB: using velocities make things considerably easier, because it means that
 * teams don't have to worry about getting an accurate model.
 * Basically, we suspect that it's easier for teams to get good encoder data than it is for
 * them to perform system identification well enough to get a good model.
 *
 * <p><strong>y = [[x, y, cos(theta), sin(theta)]]^T </strong> from vision,
 * or <strong>y = [[dist_l, dist_r, cos(theta), sin(theta)]] </strong> from encoders and gyro.
 */
public class DifferentialDrivePoseEstimator {
  final UnscentedKalmanFilter<N6, N3, N4> m_observer;
  private final BiConsumer<Matrix<N3, N1>, Matrix<N4, N1>> m_visionCorrect;
  private final KalmanFilterLatencyCompensator<N6, N3, N4> m_latencyCompensator;

  private final double m_nominalDt; // Seconds
  private double m_prevTimeSeconds = -1.0;

  private Rotation2d m_gyroOffset;
  private Rotation2d m_previousAngle;

  /**
   * Constructs a DifferentialDrivePoseEstimator.
   *
   * @param gyroAngle                The current gyro angle.
   * @param initialPoseMeters        The starting pose estimate.
   * @param stateStdDevs             Standard deviations of model states. Increase these numbers to
   *                                 trust your wheel and gyro velocities less. This is in the
   *                                 form [x, y, theta, dist_l, dist_r]^T.
   * @param localMeasurementStdDevs  Standard deviations of the encoder and gyro measurements.
   *                                 Increase these numbers to trust encoder distances and gyro
   *                                 angle less. This is in the form [ dist_l, dist_r, theta ]^T.
   * @param visionMeasurementStdDevs Standard deviations of the encoder measurements. Increase
   *                                 these numbers to trust vision less. This is in the form
   *                                 [ x, y, theta ]^T
   */
  public DifferentialDrivePoseEstimator(
          Rotation2d gyroAngle, Pose2d initialPoseMeters,
          Matrix<N5, N1> stateStdDevs,
          Matrix<N3, N1> localMeasurementStdDevs, Matrix<N3, N1> visionMeasurementStdDevs
  ) {
    this(gyroAngle, initialPoseMeters,
            stateStdDevs, localMeasurementStdDevs, visionMeasurementStdDevs, 0.02);
  }

  /**
   * Constructs a DifferentialDrivePoseEstimator. Vectors with heading states are converted internally to
   * cos/sin form.
   *
   * @param gyroAngle                The current gyro angle.
   * @param initialPoseMeters        The starting pose estimate.
   * @param stateStdDevs             Standard deviations of model states. Increase these numbers to
   *                                 trust your wheel and gyro velocities less. In the form
   *                                 [x_field, y_field, theta, dist_l, dist_r]^T.
   * @param localMeasurementStdDevs  Standard deviations of the encoder and gyro measurements.
   *                                 Increase these numbers to trust encoder distances and gyro
   *                                 angle less. In the form [dist_l, dist_r, heading]^T.
   * @param visionMeasurementStdDevs Standard deviations of the encoder measurements. Increase
   *                                 these numbers to trust vision less. In the form
   *                                 [x, y, theta]^T.
   * @param nominalDtSeconds         The time in seconds between each robot loop.
   */
  @SuppressWarnings("ParameterName")
  public DifferentialDrivePoseEstimator(
          Rotation2d gyroAngle, Pose2d initialPoseMeters,
          Matrix<N5, N1> stateStdDevs,
          Matrix<N3, N1> localMeasurementStdDevs, Matrix<N3, N1> visionMeasurementStdDevs,
          double nominalDtSeconds
  ) {
    m_nominalDt = nominalDtSeconds;

    var cosStateStdDevs = VecBuilder.fill(stateStdDevs.get(0, 0), stateStdDevs.get(1, 0),
        Math.cos(stateStdDevs.get(2, 0)), Math.sin(stateStdDevs.get(2, 0)),
        stateStdDevs.get(3,0), stateStdDevs.get(4, 0));
    var cosMeasurementStdDevs = VecBuilder.fill(localMeasurementStdDevs.get(0, 0),
        localMeasurementStdDevs.get(1, 0), Math.cos(localMeasurementStdDevs.get(2, 0)),
        Math.sin(localMeasurementStdDevs.get(2, 0)));

    m_observer = new UnscentedKalmanFilter<>(
        Nat.N6(), Nat.N4(),
        this::f,
        this::localMeasurementModel,
        cosStateStdDevs, cosMeasurementStdDevs,
        m_nominalDt
    );
    m_latencyCompensator = new KalmanFilterLatencyCompensator<>();

    var visionContR = StateSpaceUtil.makeCovarianceMatrix(Nat.N4(), VecBuilder.fill(
        visionMeasurementStdDevs.get(0, 0),
        visionMeasurementStdDevs.get(1, 0),
        Math.cos(visionMeasurementStdDevs.get(2, 0)),
        Math.sin(visionMeasurementStdDevs.get(2, 0))));
    var visionDiscR = Discretization.discretizeR(visionContR, m_nominalDt);

    m_visionCorrect = (u, y) -> m_observer.correct(
        Nat.N4(), u, y,
        (x, u_) -> x.block(Nat.N4(), Nat.N1(), 0, 0),
        visionDiscR
    );

    m_gyroOffset = initialPoseMeters.getRotation().minus(gyroAngle);
    m_previousAngle = initialPoseMeters.getRotation();
    m_observer.setXhat(fillStateVector(initialPoseMeters, 0.0, 0.0));
  }

  /**
   * Local measurement model. y = [[dist_l, dist_r, cos(theta), sin(theta)]]
   * Recall that x = [[x_field, y_field, cos(theta), sin(theta), dist_l, dist_r]]^T.
   */
  private Matrix<N4, N1> localMeasurementModel(Matrix<N6, N1> x, Matrix<N3, N1> u) {
    return VecBuilder.fill(x.get(4, 0), x.get(5, 0), x.get(2, 0), x.get(3, 0));
  }

  /**
   * The dynamics of a differential drivetrain.
   * @param x State vector, <strong> x = [[x_field, y_field, cos(theta), sin(theta), dist_l, dist_r]]^T </strong>
   * @param u input vector, <strong> u = [[v_left, v_right, omega]]^T </strong>
   * @return x-dot, <strong> x-dot = [[vx_field, vy_field, d/dt cos(theta), d/dt sin(theta), vel_left, vel_right]]^T </strong>
   */
  @SuppressWarnings({"ParameterName", "MethodName"})
  protected Matrix<N6, N1> f(Matrix<N6, N1> x, Matrix<N3, N1> u) {
    // Apply a rotation matrix. Note that we do *not* add x--Runge-Kutta does that for us.

    var cosTheta = x.get(2, 0);
    var sinTheta = x.get(3, 0);

    // We want vx and vy to be in the field frame, so we apply a rotation matrix.
    // to u_Field = u = [[vx_field, vy_field, omega]]^T
    var chassisVel_local = VecBuilder.fill((u.get(0, 0) + u.get(1, 0)) / 2.0, 0.0);
    var toFieldRotation = new MatBuilder<>(Nat.N2(), Nat.N2()).fill(
            cosTheta, -sinTheta,
            sinTheta, cosTheta
    );
    var chassisVel_field = toFieldRotation.times(chassisVel_local);

    // dcos(theta)/dt = -sin(theta) * dtheta/dt = -sin(theta) * omega
    var dcosTheta = -sinTheta * u.get(2, 0);
    // dsin(theta)/dt = cos(theta) * omega
    var dsinTheta = cosTheta * u.get(2, 0);

    // As x = [[x_field, y_field, cos(theta), sin(theta), dist_l, dist_r]]^T,
    // we need to return x-dot = [[vx_field, vy_field, d/dt cos(theta), d/dt sin(theta), vel_left, vel_right]]^T
    // Assuming  no wheel slip, vx = (v_left + v_right) / 2, and vy = 0;

    return VecBuilder.fill(chassisVel_field.get(0, 0), chassisVel_field.get(1, 0),
        dcosTheta, dsinTheta,
        u.get(0, 0), u.get(1, 0));
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
    m_observer.setXhat(fillStateVector(poseMeters, 0.0, 0.0));
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
            new Rotation2d(m_observer.getXhat(2), m_observer.getXhat(3))
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
    m_latencyCompensator.applyPastGlobalMeasurement(
            Nat.N4(),
            m_observer, m_nominalDt,
            StateSpaceUtil.poseTo4dVector(visionRobotPoseMeters),
            m_visionCorrect,
            timestampSeconds
    );
  }

  /**
   * Updates the the Extended Kalman Filter using only wheel encoder information.
   * Note that this should be called every loop.
   *
   * @param gyroAngle                      The current gyro angle.
   * @param wheelVelocitiesMetersPerSecond The velocities of the wheels in meters per second.
   * @param distanceLeftMeters             The total distance travelled by the left wheel in meters
   *                                       since the last time you called
   *                                       {@link DifferentialDrivePoseEstimator#resetPosition}.
   * @param distanceRightMeters            The total distance travelled by the right wheel in meters
   *                                       since the last time you called
   *                                       {@link DifferentialDrivePoseEstimator#resetPosition}.
   * @return The estimated pose of the robot in meters.
   */
  public Pose2d update(
          Rotation2d gyroAngle,
          DifferentialDriveWheelSpeeds wheelVelocitiesMetersPerSecond,
          double distanceLeftMeters, double distanceRightMeters
  ) {
    return updateWithTime(
            Timer.getFPGATimestamp(), gyroAngle, wheelVelocitiesMetersPerSecond,
            distanceLeftMeters, distanceRightMeters
    );
  }

  /**
   * Updates the the Extended Kalman Filter using only wheel encoder information.
   * Note that this should be called every loop.
   *
   * @param currentTimeSeconds             Time at which this method was called, in seconds.
   * @param gyroAngle                      The current gyro angle.
   * @param wheelVelocitiesMetersPerSecond The velocities of the wheels in meters per second.
   * @param distanceLeftMeters             The total distance travelled by the left wheel in meters
   *                                       since the last time you called
   *                                       {@link DifferentialDrivePoseEstimator#resetPosition}.
   * @param distanceRightMeters            The total distance travelled by the right wheel in meters
   *                                       since the last time you called
   *                                       {@link DifferentialDrivePoseEstimator#resetPosition}.
   * @return The estimated pose of the robot in meters.
   */
  @SuppressWarnings({"LocalVariableName", "ParameterName"})
  public Pose2d updateWithTime(
          double currentTimeSeconds, Rotation2d gyroAngle,
          DifferentialDriveWheelSpeeds wheelVelocitiesMetersPerSecond,
          double distanceLeftMeters, double distanceRightMeters
  ) {
    double dt = m_prevTimeSeconds >= 0 ? currentTimeSeconds - m_prevTimeSeconds : m_nominalDt;
    m_prevTimeSeconds = currentTimeSeconds;

    var angle = gyroAngle.plus(m_gyroOffset);
    // Diff drive forward kinematics:
    // v_c = (v_l + v_r) / 2
    var wheelVels = wheelVelocitiesMetersPerSecond;
    var u = VecBuilder.fill(
            wheelVels.leftMetersPerSecond, wheelVels.rightMetersPerSecond,
            angle.minus(m_previousAngle).getRadians() / dt
    );
    m_previousAngle = angle;

    var localY = VecBuilder.fill(distanceLeftMeters, distanceRightMeters, angle.getCos(), angle.getSin());
    m_latencyCompensator.addObserverState(m_observer, u, localY, currentTimeSeconds);
    m_observer.predict(u, dt);
    m_observer.correct(u, localY);

    return getEstimatedPosition();
  }

  private static Matrix<N6, N1> fillStateVector(Pose2d pose, double leftDist, double rightDist) {
    return VecBuilder.fill(
            pose.getTranslation().getX(),
            pose.getTranslation().getY(),
            pose.getRotation().getCos(),
            pose.getRotation().getSin(),
            leftDist,
            rightDist
    );
  }
}
