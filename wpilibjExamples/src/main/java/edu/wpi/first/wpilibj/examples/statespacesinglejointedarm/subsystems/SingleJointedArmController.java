/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.statespacesinglejointedarm.subsystems;

import edu.wpi.first.wpilibj.controller.StateSpaceLoop;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.MatrixUtils;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;

@SuppressWarnings("MemberName")
public class SingleJointedArmController {
  // State tolerances in meters and meters/sec respectively.
  public static final double kAngleTolerance = 0.05;
  public static final double kAngularVelocityTolerance = 2.0;

  // The current sensor measurement.
  private final Matrix<N1, N1> m_Y;

  // The control loop
  private final StateSpaceLoop<N2, N1, N1> m_loop;

  private boolean m_atReferences;

  public SingleJointedArmController() {
    m_Y = MatrixUtils.zeros(Nat.N1());
    m_loop = SingleJointedArmCoeffs.makeSingleJointedArmLoop();
  }

  public void enable() {
    m_loop.enable();
  }

  public void disable() {
    m_loop.disable();
  }

  /**
   * Sets the references.
   *
   * @param angle           Angle of arm in radians.
   * @param angularVelocity Angular velocity of arm in radians per second.
   */
  public void setReferences(double angle, double angularVelocity) {
    Matrix<N2, N1> nextR = MatrixUtils.vec(Nat.N2()).fill(angle, angularVelocity);
    m_loop.setNextR(nextR);
  }

  public boolean atReferences() {
    return m_atReferences;
  }

  /**
   * Sets the current encoder measurement.
   *
   * @param measuredAngle Angle of arm in radians.
   */
  public void setMeasuredAngle(double measuredAngle) {
    m_Y.set(0, 0, measuredAngle);
  }

  public double getControllerVoltage() {
    return m_loop.getU(0);
  }

  public double getEstimatedAngle() {
    return m_loop.getXhat(0);
  }

  public double getEstimatedAngularVelocity() {
    return m_loop.getXhat(1);
  }

  public double getAngleError() {
    return m_loop.getError().get(0, 0);
  }

  public double getAngularVelocityError() {
    return m_loop.getError().get(1, 0);
  }

  /**
   * Executes the control loop for a cycle.
   */
  public void update() {
    m_loop.correct(m_Y);

    var error = m_loop.getError();
    m_atReferences = Math.abs(error.get(0, 0)) < kAngleTolerance
        && Math.abs(error.get(1, 0)) < kAngularVelocityTolerance;

    m_loop.predict();
  }

  /**
   * Resets any internal state.
   */
  public void reset() {
    m_loop.reset();
  }
}
