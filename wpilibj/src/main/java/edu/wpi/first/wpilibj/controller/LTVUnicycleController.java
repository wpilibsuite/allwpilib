package edu.wpi.first.wpilibj.controller;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.trajectory.Trajectory;
import edu.wpi.first.wpiutil.math.MatBuilder;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;
import edu.wpi.first.wpiutil.math.numbers.N3;

/**
 * A Linear Time-Varying Cascaded Unicycle Controller for differential drive
 * robots. Similar to RAMSETE, this controller combines feedback and feedforward
 * to output ChassisSpeeds to guide a robot along a trajectory. However, this
 * controller utilizes tolerances grounded in reality to pick gains rather than
 * magical Beta and Zeta gains.
 */
public class LTVUnicycleController {

  @SuppressWarnings("MemberName")
  private final Matrix<N2, N3> m_K0;
  @SuppressWarnings("MemberName")
  private final Matrix<N2, N3> m_K1;
  Pose2d m_poseError;
  Pose2d m_poseTolerance;

  /**
   * Construct a LTV Unicycle Controller.
   *
   * @param qElms     The maximum desired error tolerance for the robot's state, in
   *                  the form [X, Y, Heading]^T. Units are meters and radians.
   * @param rElms     The maximum desired control effort by the feedback controller,
   *                  in the form [vMax, wMax]^T. Units are meters per second and
   *                  radians per second. Note that this is not the maximum speed of
   *                  the robot, but rather the maximum effort the feedback controller
   *                  should apply on top of the trajectory feedforward.
   * @param dtSeconds The nominal dt of this controller. With command based this is 0.020.
   */
  @SuppressWarnings("ParameterName")
  public LTVUnicycleController(Matrix<N3, N1> qElms, Matrix<N2, N1> rElms, double dtSeconds) {

    var a0 = new MatBuilder<>(Nat.N3(), Nat.N3()).fill(0, 0, 0, 0, 0, 1e-9, 0, 0, 0);
    var a1 = new MatBuilder<>(Nat.N3(), Nat.N3()).fill(0, 0, 0, 0, 0, 1, 0, 0, 0);
    var b = new MatBuilder<>(Nat.N3(), Nat.N2()).fill(1, 0, 0, 0, 0, 1);

    m_K0 = new LinearQuadraticRegulator<N3, N2, N2>(
            Nat.N3(), Nat.N2(), a0, b, qElms, rElms, dtSeconds
    ).getK();

    m_K1 = new LinearQuadraticRegulator<N3, N2, N2>(
            Nat.N3(), Nat.N2(), a1, b, qElms, rElms, dtSeconds
    ).getK();
  }

  /**
   * Returns if the controller is at the reference pose on the trajectory.
   * Note that this is different than if the robot has traversed the entire
   * trajectory. The tolerance is set by the {@link #setTolerance(Pose2d)}
   * method.
   *
   * @return If the robot is within the specified tolerance of the
   */
  public boolean atReference() {
    var eTranslate = m_poseError.getTranslation();
    var eRotate = m_poseError.getRotation();
    var tolTranslate = m_poseTolerance.getTranslation();
    var tolRotate = m_poseTolerance.getRotation();
    return Math.abs(eTranslate.getX()) < tolTranslate.getX()
            && Math.abs(eTranslate.getY()) < tolTranslate.getY()
            && Math.abs(eRotate.getRadians()) < tolRotate.getRadians();
  }

  /**
   * Set the tolerance for if the robot is {@link #atReference()} or not.
   *
   * @param poseTolerance The new pose tolerance.
   */
  public void setTolerance(final Pose2d poseTolerance) {
    this.m_poseTolerance = poseTolerance;
  }

  /**
   * Returns the next output of the controller.
   *
   * <p>The reference pose, linear velocity, and angular velocity should come
   * from a {@link Trajectory}.
   *
   * @param currentPose                   The current position of the robot.
   * @param poseRef                       The desired pose of the robot.
   * @param linearVelocityRefMetersPerSec The desired linear velocity of the robot.
   * @param angularVelocityRefRadPerSec   The desired angular velocity of the robot.
   * @return The next calculated output.
   */
  public ChassisSpeeds calculate(Pose2d currentPose, Pose2d poseRef,
                                 double linearVelocityRefMetersPerSec,
                                 double angularVelocityRefRadPerSec) {

    m_poseError = poseRef.relativeTo(currentPose);

    var kx = m_K0.get(0, 0);
    var ky0 = m_K0.get(0, 1);
    var ky1 = m_K1.get(0, 1);
    var ktheta1 = m_K1.get(0, 2);

    var v = linearVelocityRefMetersPerSec;
    var sqrtAbsV = Math.sqrt(Math.abs(v));
    var K = new MatBuilder<>(Nat.N2(), Nat.N3())
            .fill(kx, 0, 0, 0,
                    (ky0 + (ky1 - ky0) * sqrtAbsV) * Math.signum(v),
                    ktheta1 * sqrtAbsV);

    var error = new MatBuilder<>(Nat.N3(), Nat.N1()).fill(
            m_poseError.getTranslation().getX(),
            m_poseError.getTranslation().getY(),
            m_poseError.getRotation().getRadians());

    @SuppressWarnings("LocalVariableName")
    var u = K.times(error);

    return new ChassisSpeeds(
            linearVelocityRefMetersPerSec + u.get(0, 0),
            0.0,
            angularVelocityRefRadPerSec + u.get(1, 0));
  }

  /**
   * Returns the next output of the controller.
   *
   * <p>The reference pose and desired state should come from a {@link Trajectory}.
   *
   * @param currentPose  The current pose.
   * @param desiredState The desired pose, linear velocity, and angular velocity
   *                     from a trajectory.
   */
  public ChassisSpeeds calculate(Pose2d currentPose, Trajectory.State desiredState) {
    return calculate(currentPose, desiredState.poseMeters,
            desiredState.velocityMetersPerSecond,
            desiredState.velocityMetersPerSecond * desiredState.curvatureRadPerMeter);
  }

}
