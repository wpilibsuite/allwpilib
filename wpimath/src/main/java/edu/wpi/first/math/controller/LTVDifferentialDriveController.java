// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.DARE;
import edu.wpi.first.math.InterpolatingMatrixTreeMap;
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
 * outputs. This is different from a Ramsete controller's nested hierarchy where the top-level
 * controller has a pose reference and chassis velocity command outputs, and the low-level
 * controller has wheel velocity references and voltage outputs. Flat hierarchies are easier to tune
 * in one shot. Furthermore, this controller is more optimal in the "least-squares error" sense than
 * a controller based on Ramsete.
 *
 * <p>See section 8.7 in Controls Engineering in FRC for a derivation of the control law we used
 * shown in theorem 8.7.4.
 */
public class LTVDifferentialDriveController {
  private final double m_trackwidth;

  // LUT from drivetrain linear velocity to LQR gain
  private final InterpolatingMatrixTreeMap<Double, N2, N5> m_table =
      new InterpolatingMatrixTreeMap<>();

  private Matrix<N5, N1> m_error = new Matrix<>(Nat.N5(), Nat.N1());
  private Matrix<N5, N1> m_tolerance = new Matrix<>(Nat.N5(), Nat.N1());

  /** States of the drivetrain system. */
  private enum State {
    kX(0),
    kY(1),
    kHeading(2),
    kLeftVelocity(3),
    kRightVelocity(4);

    public final int value;

    State(int i) {
      this.value = i;
    }
  }

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
   * @throws IllegalArgumentException if max velocity of plant with 12 V input &lt;= 0 m/s or &gt;=
   *     15 m/s.
   */
  public LTVDifferentialDriveController(
      LinearSystem<N2, N2, N2> plant,
      double trackwidth,
      Vector<N5> qelems,
      Vector<N2> relems,
      double dt) {
    m_trackwidth = trackwidth;

    // Control law derivation is in section 8.7 of
    // https://file.tavsys.net/control/controls-engineering-in-frc.pdf
    var A =
        MatBuilder.fill(
            Nat.N5(),
            Nat.N5(),
            0.0,
            0.0,
            0.0,
            0.5,
            0.5,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            -1.0 / m_trackwidth,
            1.0 / m_trackwidth,
            0.0,
            0.0,
            0.0,
            plant.getA(0, 0),
            plant.getA(0, 1),
            0.0,
            0.0,
            0.0,
            plant.getA(1, 0),
            plant.getA(1, 1));
    var B =
        MatBuilder.fill(
            Nat.N5(),
            Nat.N2(),
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            plant.getB(0, 0),
            plant.getB(0, 1),
            plant.getB(1, 0),
            plant.getB(1, 1));
    var Q = StateSpaceUtil.makeCostMatrix(qelems);
    var R = StateSpaceUtil.makeCostMatrix(relems);

    // dx/dt = Ax + Bu
    // 0 = Ax + Bu
    // Ax = -Bu
    // x = -A⁻¹Bu
    double maxV =
        plant.getA().solve(plant.getB().times(VecBuilder.fill(12.0, 12.0))).times(-1.0).get(0, 0);

    if (maxV <= 0.0) {
      throw new IllegalArgumentException(
          "Max velocity of plant with 12 V input must be greater than 0 m/s.");
    }
    if (maxV >= 15.0) {
      throw new IllegalArgumentException(
          "Max velocity of plant with 12 V input must be less than 15 m/s.");
    }

    for (double velocity = -maxV; velocity < maxV; velocity += 0.01) {
      // The DARE is ill-conditioned if the velocity is close to zero, so don't
      // let the system stop.
      if (Math.abs(velocity) < 1e-4) {
        A.set(State.kY.value, State.kHeading.value, 1e-4);
      } else {
        A.set(State.kY.value, State.kHeading.value, velocity);
      }

      var discABPair = Discretization.discretizeAB(A, B, dt);
      var discA = discABPair.getFirst();
      var discB = discABPair.getSecond();

      var S = DARE.dareNoPrecond(discA, discB, Q, R);

      // K = (BᵀSB + R)⁻¹BᵀSA
      m_table.put(
          velocity,
          discB
              .transpose()
              .times(S)
              .times(discB)
              .plus(R)
              .solve(discB.transpose().times(S).times(discA)));
    }
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
    // theorem 9.6.3 of https://tavsys.net/controls-in-frc.
    var x =
        VecBuilder.fill(
            currentPose.getX(),
            currentPose.getY(),
            currentPose.getRotation().getRadians(),
            leftVelocity,
            rightVelocity);

    var inRobotFrame = Matrix.eye(Nat.N5());
    inRobotFrame.set(0, 0, Math.cos(x.get(State.kHeading.value, 0)));
    inRobotFrame.set(0, 1, Math.sin(x.get(State.kHeading.value, 0)));
    inRobotFrame.set(1, 0, -Math.sin(x.get(State.kHeading.value, 0)));
    inRobotFrame.set(1, 1, Math.cos(x.get(State.kHeading.value, 0)));

    var r =
        VecBuilder.fill(
            poseRef.getX(),
            poseRef.getY(),
            poseRef.getRotation().getRadians(),
            leftVelocityRef,
            rightVelocityRef);
    m_error = r.minus(x);
    m_error.set(
        State.kHeading.value, 0, MathUtil.angleModulus(m_error.get(State.kHeading.value, 0)));

    double velocity = (leftVelocity + rightVelocity) / 2.0;
    var K = m_table.get(velocity);

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
        desiredState.poseMeters,
        desiredState.velocityMetersPerSecond
            * (1 - (desiredState.curvatureRadPerMeter * m_trackwidth / 2.0)),
        desiredState.velocityMetersPerSecond
            * (1 + (desiredState.curvatureRadPerMeter * m_trackwidth / 2.0)));
  }
}
