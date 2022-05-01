// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.StateSpaceUtil;
import edu.wpi.first.math.Vector;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.math.trajectory.Trajectory;

/**
 * The linear time-varying unicycle controller has a similar form to the LQR, but the model used to
 * compute the controller gain is the nonlinear model linearized around the drivetrain's current
 * state.
 *
 * <p>See section 8.9 in Controls Engineering in FRC for a derivation of the control law we used
 * shown in theorem 8.9.1.
 */
@SuppressWarnings("MemberName")
public class LTVUnicycleController {
  private final Matrix<N3, N3> m_A = new Matrix<>(Nat.N3(), Nat.N3());
  private final Matrix<N3, N2> m_B =
      new MatBuilder<>(Nat.N3(), Nat.N2()).fill(1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
  private final Matrix<N3, N3> m_Q;
  private final Matrix<N2, N2> m_R;
  private final double m_dt;

  private Pose2d m_poseError;
  private Pose2d m_poseTolerance;
  private boolean m_enabled = true;

  /** States of the drivetrain system. */
  enum State {
    kX(0),
    kY(1),
    kHeading(2);

    @SuppressWarnings("MemberName")
    public final int value;

    @SuppressWarnings("ParameterName")
    State(int i) {
      this.value = i;
    }
  }

  /** Inputs of the drivetrain system. */
  enum Input {
    kLeftVelocity(3),
    kRightVelocity(4);

    @SuppressWarnings("MemberName")
    public final int value;

    @SuppressWarnings("ParameterName")
    Input(int i) {
      this.value = i;
    }
  }

  /**
   * Constructs a linear time-varying unicycle controller.
   *
   * @param qelems The maximum desired error tolerance for each state.
   * @param relems The maximum desired control effort for each input.
   * @param dt Discretization timestep in seconds.
   */
  public LTVUnicycleController(Vector<N3> qelems, Vector<N2> relems, double dt) {
    m_dt = dt;
    m_Q = StateSpaceUtil.makeCostMatrix(qelems);
    m_R = StateSpaceUtil.makeCostMatrix(relems);
  }

  /**
   * Returns true if the pose error is within tolerance of the reference.
   *
   * @return True if the pose error is within tolerance of the reference.
   */
  public boolean atReference() {
    final var eTranslate = m_poseError.getTranslation();
    final var eRotate = m_poseError.getRotation();
    final var tolTranslate = m_poseTolerance.getTranslation();
    final var tolRotate = m_poseTolerance.getRotation();
    return Math.abs(eTranslate.getX()) < tolTranslate.getX()
        && Math.abs(eTranslate.getY()) < tolTranslate.getY()
        && Math.abs(eRotate.getRadians()) < tolRotate.getRadians();
  }

  /**
   * Sets the pose error which is considered tolerable for use with atReference().
   *
   * @param poseTolerance Pose error which is tolerable.
   */
  public void setTolerance(Pose2d poseTolerance) {
    m_poseTolerance = poseTolerance;
  }

  /**
   * Returns the linear and angular velocity outputs of the LTV controller.
   *
   * <p>The reference pose, linear velocity, and angular velocity should come from a drivetrain
   * trajectory.
   *
   * @param currentPose The current pose.
   * @param poseRef The desired pose.
   * @param linearVelocityRef The desired linear velocity in meters per second.
   * @param angularVelocityRef The desired angular velocity in radians per second.
   * @return The linear and angular velocity outputs of the LTV controller.
   */
  public ChassisSpeeds calculate(
      Pose2d currentPose, Pose2d poseRef, double linearVelocityRef, double angularVelocityRef) {
    if (!m_enabled) {
      return new ChassisSpeeds(linearVelocityRef, 0.0, angularVelocityRef);
    }

    m_poseError = poseRef.relativeTo(currentPose);

    if (Math.abs(linearVelocityRef) < 1e-4) {
      m_A.set(State.kY.value, State.kHeading.value, 1e-4);
    } else {
      m_A.set(State.kY.value, State.kHeading.value, linearVelocityRef);
    }
    var e =
        new MatBuilder<>(Nat.N3(), Nat.N1())
            .fill(m_poseError.getX(), m_poseError.getY(), m_poseError.getRotation().getRadians());
    var u = new LinearQuadraticRegulator<N3, N2, N3>(m_A, m_B, m_Q, m_R, m_dt).getK().times(e);

    return new ChassisSpeeds(
        linearVelocityRef + u.get(0, 0), 0.0, angularVelocityRef + u.get(1, 0));
  }

  /**
   * Returns the next output of the LTV controller.
   *
   * <p>The reference pose, linear velocity, and angular velocity should come from a drivetrain
   * trajectory.
   *
   * @param currentPose The current pose.
   * @param desiredState The desired pose, linear velocity, and angular velocity from a trajectory.
   * @return The linear and angular velocity outputs of the LTV controller.
   */
  public ChassisSpeeds calculate(Pose2d currentPose, Trajectory.State desiredState) {
    return calculate(
        currentPose,
        desiredState.poseMeters,
        desiredState.velocityMetersPerSecond,
        desiredState.velocityMetersPerSecond * desiredState.curvatureRadPerMeter);
  }

  /**
   * Enables and disables the controller for troubleshooting purposes.
   *
   * @param enabled If the controller is enabled or not.
   */
  public void setEnabled(boolean enabled) {
    m_enabled = enabled;
  }
}
