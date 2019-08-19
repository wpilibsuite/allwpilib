package edu.wpi.first.wpilibj.examples.statespacedifferentialdrive.subsystems;

import edu.wpi.first.wpilibj.controller.StateSpaceLoop;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.MatrixUtils;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;
import edu.wpi.first.wpiutil.math.numbers.N4;

public class DifferentialDriveController {
  public static final double kPositionTolerance = 0.05;
  public static final double kVelocityTolerance = 2.0;

  private Matrix<N2, N1> m_Y;
  private StateSpaceLoop<N4, N2, N2> m_loop;
  private boolean m_atReferences = false;

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

  public void setReferences(double leftPosition, double leftVelocity,
                            double rightPosition, double rightVelocity) {
    Matrix<N4, N1> nextR = MatrixUtils.vec(Nat.N4())
        .fill(leftPosition, leftVelocity, rightPosition, rightVelocity);

    m_loop.setNextR(nextR);
  }

  public boolean atReferences() {
    return m_atReferences;
  }

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

  public void update() {
    m_loop.correct(m_Y);

    var error = m_loop.getError();
    m_atReferences = Math.abs(error.get(0, 0)) < kPositionTolerance
        && Math.abs(error.get(1, 0)) < kVelocityTolerance
        && Math.abs(error.get(2, 0)) < kPositionTolerance
        && Math.abs(error.get(3, 0)) < kVelocityTolerance;

    m_loop.predict();
  }

  public void reset() {
    m_loop.reset();
  }
}

