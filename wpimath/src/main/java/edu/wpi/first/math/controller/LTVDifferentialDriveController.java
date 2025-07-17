// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.DARE;
import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.StateSpaceUtil;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.Vector;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.numbers.N5;
import edu.wpi.first.math.system.Discretization;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.trajectory.Trajectory;

/**
 * The linear time-varying differential drive controller has a similar form to the LQR, but the
 * model used to compute the controller gain is the nonlinear differential drive model linearized
 * around the drivetrain's current state. We precompute gains for important places in our
 * state-space, then interpolate between them with a lookup table to save computational resources.
 *
 * <p>This controller has a flat hierarchy with pose and wheel velocity references and voltage
 * outputs. This is different from a unicycle controller's nested hierarchy where the top-level
 * controller has a pose reference and chassis velocity command outputs, and the low-level
 * controller has wheel velocity references and voltage outputs. Flat hierarchies are easier to tune
 * in one shot.
 *
 * <p>See section 8.7 in Controls Engineering in FRC for a derivation of the control law we used
 * shown in theorem 8.7.4.
 */
public class LTVDifferentialDriveController {
  private final double m_trackwidth;

  // Continuous velocity dynamics
  private final Matrix<N2, N2> m_A;
  private final Matrix<N2, N2> m_B;

  // LQR cost matrices
  private final Matrix<N5, N5> m_Q;
  private final Matrix<N2, N2> m_R;

  private final double m_dt;

  private Matrix<N5, N1> m_error = new Matrix<>(Nat.N5(), Nat.N1());
  private Matrix<N5, N1> m_tolerance = new Matrix<>(Nat.N5(), Nat.N1());

  /**
   * Constructs a linear time-varying differential drive controller.
   *
   * <p>See <a
   * href="https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-intro.html#lqr-tuning">https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-intro.html#lqr-tuning</a>
   * for how to select the tolerances.
   *
   * @param plant The differential drive velocity plant.
   * @param trackwidth The distance between the differential drive's left and right wheels in
   *     meters.
   * @param qelems The maximum desired error tolerance for each state.
   * @param relems The maximum desired control effort for each input.
   * @param dt Discretization timestep in seconds.
   */
  public LTVDifferentialDriveController(
      LinearSystem<N2, N2, N2> plant,
      double trackwidth,
      Vector<N5> qelems,
      Vector<N2> relems,
      double dt) {
    m_trackwidth = trackwidth;
    m_A = plant.getA();
    m_B = plant.getB();
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
    return Math.abs(m_error.get(0, 0)) < m_tolerance.get(0, 0)
        && Math.abs(m_error.get(1, 0)) < m_tolerance.get(1, 0)
        && Math.abs(m_error.get(2, 0)) < m_tolerance.get(2, 0)
        && Math.abs(m_error.get(3, 0)) < m_tolerance.get(3, 0)
        && Math.abs(m_error.get(4, 0)) < m_tolerance.get(4, 0);
  }

  /**
   * Sets the pose error which is considered tolerable for use with atReference().
   *
   * @param poseTolerance Pose error which is tolerable.
   * @param leftVelocityTolerance Left velocity error which is tolerable in meters per second.
   * @param rightVelocityTolerance Right velocity error which is tolerable in meters per second.
   */
  public void setTolerance(
      Pose2d poseTolerance, double leftVelocityTolerance, double rightVelocityTolerance) {
    m_tolerance =
        VecBuilder.fill(
            poseTolerance.getX(),
            poseTolerance.getY(),
            poseTolerance.getRotation().getRadians(),
            leftVelocityTolerance,
            rightVelocityTolerance);
  }

  /**
   * Returns the left and right output voltages of the LTV controller.
   *
   * <p>The reference pose, linear velocity, and angular velocity should come from a drivetrain
   * trajectory.
   *
   * @param currentPose The current pose.
   * @param leftVelocity The current left velocity in meters per second.
   * @param rightVelocity The current right velocity in meters per second.
   * @param poseRef The desired pose.
   * @param leftVelocityRef The desired left velocity in meters per second.
   * @param rightVelocityRef The desired right velocity in meters per second.
   * @return Left and right output voltages of the LTV controller.
   */
  public DifferentialDriveWheelVoltages calculate(
      Pose2d currentPose,
      double leftVelocity,
      double rightVelocity,
      Pose2d poseRef,
      double leftVelocityRef,
      double rightVelocityRef) {
    // This implements the linear time-varying differential drive controller in
    // theorem 8.7.4 of https://controls-in-frc.link/
    //
    //     [x ]
    //     [y ]       [Vₗ]
    // x = [θ ]   u = [Vᵣ]
    //     [vₗ]
    //     [vᵣ]

    double velocity = (leftVelocity + rightVelocity) / 2.0;

    // The DARE is ill-conditioned if the velocity is close to zero, so don't
    // let the system stop.
    if (Math.abs(velocity) < 1e-4) {
      velocity = 1e-4;
    }

    var r =
        VecBuilder.fill(
            poseRef.getX(),
            poseRef.getY(),
            poseRef.getRotation().getRadians(),
            leftVelocityRef,
            rightVelocityRef);
    var x =
        VecBuilder.fill(
            currentPose.getX(),
            currentPose.getY(),
            currentPose.getRotation().getRadians(),
            leftVelocity,
            rightVelocity);

    m_error = r.minus(x);
    m_error.set(2, 0, MathUtil.angleModulus(m_error.get(2, 0)));

    // spotless:off
    var A = MatBuilder.fill(Nat.N5(), Nat.N5(),
        0.0, 0.0, 0.0, 0.5, 0.5,
        0.0, 0.0, velocity, 0.0, 0.0,
        0.0, 0.0, 0.0, -1.0 / m_trackwidth, 1.0 / m_trackwidth,
        0.0, 0.0, 0.0, m_A.get(0, 0), m_A.get(0, 1),
        0.0, 0.0, 0.0, m_A.get(1, 0), m_A.get(1, 1));
    var B = MatBuilder.fill(Nat.N5(), Nat.N2(),
        0.0, 0.0,
        0.0, 0.0,
        0.0, 0.0,
        m_B.get(0, 0), m_B.get(0, 1),
        m_B.get(1, 0), m_B.get(1, 1));
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

    // spotless:off
    var inRobotFrame = MatBuilder.fill(Nat.N5(), Nat.N5(),
        Math.cos(x.get(2, 0)), Math.sin(x.get(2, 0)), 0.0, 0.0, 0.0,
        -Math.sin(x.get(2, 0)), Math.cos(x.get(2, 0)), 0.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 1.0);
    // spotless:on

    var u = K.times(inRobotFrame).times(m_error);

    return new DifferentialDriveWheelVoltages(u.get(0, 0), u.get(1, 0));
  }

  /**
   * Returns the left and right output voltages of the LTV controller.
   *
   * <p>The reference pose, linear velocity, and angular velocity should come from a drivetrain
   * trajectory.
   *
   * @param currentPose The current pose.
   * @param leftVelocity The left velocity in meters per second.
   * @param rightVelocity The right velocity in meters per second.
   * @param desiredState The desired pose, linear velocity, and angular velocity from a trajectory.
   * @return The left and right output voltages of the LTV controller.
   */
  public DifferentialDriveWheelVoltages calculate(
      Pose2d currentPose,
      double leftVelocity,
      double rightVelocity,
      Trajectory.State desiredState) {
    // v = (v_r + v_l) / 2     (1)
    // w = (v_r - v_l) / (2r)  (2)
    // k = w / v               (3)
    //
    // v_l = v - wr
    // v_l = v - (vk)r
    // v_l = v(1 - kr)
    //
    // v_r = v + wr
    // v_r = v + (vk)r
    // v_r = v(1 + kr)
    return calculate(
        currentPose,
        leftVelocity,
        rightVelocity,
        desiredState.pose,
        desiredState.velocity * (1 - (desiredState.curvature * m_trackwidth / 2.0)),
        desiredState.velocity * (1 + (desiredState.curvature * m_trackwidth / 2.0)));
  }
}
