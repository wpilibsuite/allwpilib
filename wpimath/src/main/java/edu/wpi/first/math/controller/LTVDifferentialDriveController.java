// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import edu.wpi.first.math.InterpolatingMatrixTreeMap;
import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.StateSpaceUtil;
import edu.wpi.first.math.Vector;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.numbers.N5;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.trajectory.Trajectory;

/**
 * The linear time-varying differential drive controller has a similar form to the LQR, but the
 * model used to compute the controller gain is the nonlinear model linearized around the
 * drivetrain's current state. We precomputed gains for important places in our state-space, then
 * interpolated between them with a LUT to save computational resources.
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

  /** Motor voltages for a differential drive. */
  @SuppressWarnings("MemberName")
  public static class WheelVoltages {
    public double left;
    public double right;

    public WheelVoltages() {}

    public WheelVoltages(double left, double right) {
      this.left = left;
      this.right = right;
    }
  }

  /** States of the drivetrain system. */
  enum State {
    kX(0),
    kY(1),
    kHeading(2),
    kLeftVelocity(3),
    kRightVelocity(4);

    @SuppressWarnings("MemberName")
    public final int value;

    @SuppressWarnings("ParameterName")
    State(int i) {
      this.value = i;
    }
  }

  /**
   * Constructs a linear time-varying differential drive controller.
   *
   * @param plant The drivetrain velocity plant.
   * @param trackwidth The drivetrain's trackwidth in meters.
   * @param qelems The maximum desired error tolerance for each state.
   * @param relems The maximum desired control effort for each input.
   * @param dt Discretization timestep in seconds.
   */
  @SuppressWarnings("LocalVariableName")
  public LTVDifferentialDriveController(
      LinearSystem<N2, N2, N2> plant,
      double trackwidth,
      Vector<N5> qelems,
      Vector<N2> relems,
      double dt) {
    m_trackwidth = trackwidth;

    var A =
        new MatBuilder<>(Nat.N5(), Nat.N5())
            .fill(
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
        new MatBuilder<>(Nat.N5(), Nat.N2())
            .fill(
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
        plant
            .getA()
            .solve(plant.getB().times(new MatBuilder<>(Nat.N2(), Nat.N1()).fill(12.0, 12.0)))
            .times(-1.0)
            .get(0, 0);

    var x = new Matrix<>(Nat.N5(), Nat.N1());
    for (double velocity = -maxV; velocity < maxV; velocity += 0.01) {
      x.set(State.kLeftVelocity.value, 0, velocity);
      x.set(State.kRightVelocity.value, 0, velocity);

      // The DARE is ill-conditioned if the velocity is close to zero, so don't
      // let the system stop.
      if (Math.abs(velocity) < 1e-4) {
        m_table.put(velocity, new Matrix<>(Nat.N2(), Nat.N5()));
      } else {
        A.set(State.kY.value, State.kHeading.value, velocity);
        m_table.put(velocity, new LinearQuadraticRegulator<N5, N2, N5>(A, B, Q, R, dt).getK());
      }
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
        new MatBuilder<>(Nat.N5(), Nat.N1())
            .fill(
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
  @SuppressWarnings("LocalVariableName")
  public WheelVoltages calculate(
      Pose2d currentPose,
      double leftVelocity,
      double rightVelocity,
      Pose2d poseRef,
      double leftVelocityRef,
      double rightVelocityRef) {
    // This implements the linear time-varying differential drive controller in
    // theorem 9.6.3 of https://tavsys.net/controls-in-frc.
    var x =
        new MatBuilder<>(Nat.N5(), Nat.N1())
            .fill(
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
        new MatBuilder<>(Nat.N5(), Nat.N1())
            .fill(
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

    return new WheelVoltages(u.get(0, 0), u.get(1, 0));
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
  public WheelVoltages calculate(
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
