/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.statespacedifferentialdrive.subsystems;

import edu.wpi.first.wpilibj.controller.StateSpaceLoop;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.MatrixUtils;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;
import edu.wpi.first.wpiutil.math.numbers.N4;

@SuppressWarnings("MemberName")
public class DifferentialDriveController {
  // State tolerances in meters and meters/sec respectively.
  public static final double kPositionTolerance = 0.05;
  public static final double kVelocityTolerance = 2.0;

  // The current sensor measurements.
  private final Matrix<N2, N1> m_Y;

  // The control loop
  private final StateSpaceLoop<N4, N2, N2> m_loop;

  private boolean m_atReferences;

  public DifferentialDriveController() {
    m_Y = MatrixUtils.zeros(Nat.N2());
    m_loop = DifferentialDriveCoeffs.makeDifferentialDriveLoop();
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
   * @param leftPosition  Position of left side in meters.
   * @param leftVelocity  Velocity of left side in meters per second.
   * @param rightPosition Position of right side in meters.
   * @param rightVelocity Velocity of right side in meters per second.
   */
  public void setReferences(double leftPosition, double leftVelocity,
                            double rightPosition, double rightVelocity) {
    Matrix<N4, N1> nextR = MatrixUtils.vec(Nat.N4())
        .fill(leftPosition, leftVelocity, rightPosition, rightVelocity);

    m_loop.setNextR(nextR);
  }

  public boolean atReferences() {
    return m_atReferences;
  }

  /**
   * Sets the current encoder measurements.
   *
   * @param leftPosition  Position of left side in meters.
   * @param rightPosition Position of right side in meters.
   */
  public void setMeasuredStates(double leftPosition, double rightPosition) {
    m_Y.set(0, 0, leftPosition);
    m_Y.set(1, 0, rightPosition);
  }

  public double getControllerLeftVoltage() {
    return m_loop.getU(0);
  }

  public double getControllerRightVoltage() {
    return m_loop.getU(1);
  }

  public double getEstimatedLeftPosition() {
    return m_loop.getXhat(0);
  }

  public double getEstimatedLeftVelocity() {
    return m_loop.getXhat(1);
  }

  public double getEstimatedRightPosition() {
    return m_loop.getXhat(2);
  }

  public double getEstimatedRightVelocity() {
    return m_loop.getXhat(3);
  }

  public double getLeftPositionError() {
    return m_loop.getError().get(0, 0);
  }

  public double getLeftVelocityError() {
    return m_loop.getError().get(1, 0);
  }

  public double getRightPositionError() {
    return m_loop.getError().get(2, 0);
  }

  public double getRightVelocityError() {
    return m_loop.getError().get(3, 0);
  }

  /**
   * Executes the control loop for a cycle.
   */
  public void update() {
    m_loop.correct(m_Y);

    var error = m_loop.getError();
    m_atReferences = Math.abs(error.get(0, 0)) < kPositionTolerance
        && Math.abs(error.get(1, 0)) < kVelocityTolerance
        && Math.abs(error.get(2, 0)) < kPositionTolerance
        && Math.abs(error.get(3, 0)) < kVelocityTolerance;

    m_loop.predict();
  }

  /**
   * Resets any internal state.
   */
  public void reset() {
    m_loop.reset();
  }
}
