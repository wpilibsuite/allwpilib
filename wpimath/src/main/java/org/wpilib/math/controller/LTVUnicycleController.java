// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller;

import org.wpilib.math.DARE;
import org.wpilib.math.MatBuilder;
import org.wpilib.math.Matrix;
import org.wpilib.math.Nat;
import org.wpilib.math.StateSpaceUtil;
import org.wpilib.math.VecBuilder;
import org.wpilib.math.Vector;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.kinematics.ChassisSpeeds;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.numbers.N3;
import org.wpilib.math.system.Discretization;
import org.wpilib.math.trajectory.Trajectory;

/**
 * The linear time-varying unicycle controller has a similar form to the LQR, but the model used to
 * compute the controller gain is the nonlinear unicycle model linearized around the drivetrain's
 * current state.
 *
 * <p>See section 8.9 in Controls Engineering in FRC for a derivation of the control law we used
 * shown in theorem 8.9.1.
 */
public class LTVUnicycleController {
  // LQR cost matrices
  private Matrix<N3, N3> m_Q;
  private Matrix<N2, N2> m_R;

  private final double m_dt;

  private Pose2d m_poseError;
  private Pose2d m_poseTolerance;
  private boolean m_enabled = true;

  /**
   * Constructs a linear time-varying unicycle controller with default maximum desired error
   * tolerances of (x = 0.0625 m, y = 0.125 m, heading = 2 rad), default maximum desired control
   * effort of (linear velocity = 1 m/s, angular velocity = 2 rad/s), and default maximum Velocity
   * of 9 m/s.
   *
   * @param dt Discretization timestep in seconds.
   */
  public LTVUnicycleController(double dt) {
    this(VecBuilder.fill(0.0625, 0.125, 2.0), VecBuilder.fill(1.0, 2.0), dt);
  }

  /**
   * Constructs a linear time-varying unicycle controller.
   *
   * <p>See <a
   * href="https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-intro.html#lqr-tuning">https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-intro.html#lqr-tuning</a>
   * for how to select the tolerances.
   *
   * <p>The default maximum Velocity is 9 m/s.
   *
   * @param qelems The maximum desired error tolerance for each state (x, y, heading).
   * @param relems The maximum desired control effort for each input (linear velocity, angular
   *     velocity).
   * @param dt Discretization timestep in seconds.
   */
  public LTVUnicycleController(Vector<N3> qelems, Vector<N2> relems, double dt) {
    m_Q = StateSpaceUtil.makeCostMatrix(qelems);
    m_R = StateSpaceUtil.makeCostMatrix(relems);
    m_dt = dt;
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
    // The change in global pose for a unicycle is defined by the following
    // three equations.
    //
    // ẋ = v cosθ
    // ẏ = v sinθ
    // θ̇ = ω
    //
    // Here's the model as a vector function where x = [x  y  θ]ᵀ and
    // u = [v  ω]ᵀ.
    //
    //           [v cosθ]
    // f(x, u) = [v sinθ]
    //           [  ω   ]
    //
    // To create an LQR, we need to linearize this.
    //
    //               [0  0  −v sinθ]                  [cosθ  0]
    // ∂f(x, u)/∂x = [0  0   v cosθ]    ∂f(x, u)/∂u = [sinθ  0]
    //               [0  0     0   ]                  [ 0    1]
    //
    // We're going to make a cross-track error controller, so we'll apply a
    // clockwise rotation matrix to the global tracking error to transform it
    // into the robot's coordinate frame. Since the cross-track error is always
    // measured from the robot's coordinate frame, the model used to compute the
    // LQR should be linearized around θ = 0 at all times.
    //
    //     [0  0  −v sin0]        [cos0  0]
    // A = [0  0   v cos0]    B = [sin0  0]
    //     [0  0     0   ]        [ 0    1]
    //
    //     [0  0  0]              [1  0]
    // A = [0  0  v]          B = [0  0]
    //     [0  0  0]              [0  1]

    if (!m_enabled) {
      return new ChassisSpeeds(linearVelocityRef, 0.0, angularVelocityRef);
    }

    // The DARE is ill-conditioned if the velocity is close to zero, so don't
    // let the system stop.
    if (Math.abs(linearVelocityRef) < 1e-4) {
      linearVelocityRef = 1e-4;
    }

    m_poseError = poseRef.relativeTo(currentPose);

    // spotless:off
    var A = MatBuilder.fill(Nat.N3(), Nat.N3(),
        0.0, 0.0, 0.0,
        0.0, 0.0, linearVelocityRef,
        0.0, 0.0, 0.0);
    var B = MatBuilder.fill(Nat.N3(), Nat.N2(),
        1.0, 0.0,
        0.0, 0.0,
        0.0, 1.0);
    // spotless:on

    var discABPair = Discretization.discretizeAB(A, B, m_dt);
    var discA = discABPair.getFirst();
    var discB = discABPair.getSecond();

    var S = DARE.dareNoPrecond(discA, discB, m_Q, m_R);

    // K = (BᵀSB + R)⁻¹BᵀSA
    var K =
        discB
            .transpose()
            .times(S)
            .times(discB)
            .plus(m_R)
            .solve(discB.transpose().times(S).times(discA));

    var e =
        MatBuilder.fill(
            Nat.N3(),
            Nat.N1(),
            m_poseError.getX(),
            m_poseError.getY(),
            m_poseError.getRotation().getRadians());
    var u = K.times(e);

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
        desiredState.pose,
        desiredState.velocity,
        desiredState.velocity * desiredState.curvature);
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
