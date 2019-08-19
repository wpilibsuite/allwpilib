package edu.wpi.first.wpilibj.examples.statespacesinglejointedarm.subsystems;

import edu.wpi.first.wpilibj.controller.StateSpaceLoop;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.MatrixUtils;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;

public class SingleJointedArmController {
  public static final double kAngleTolerance = 0.05;
  public static final double kAngularVelocityTolerance = 2.0;

  private Matrix<N1, N1> m_Y;
  private StateSpaceLoop<N2, N1, N1> m_loop;
  private boolean m_atReferences = false;

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

  public void setReferences(double angle, double angularVelocity) {
    Matrix<N2, N1> nextR = MatrixUtils.vec(Nat.N2()).fill(angle, angularVelocity);
    m_loop.setNextR(nextR);
  }

  public boolean atReferences() {
    return m_atReferences;
  }

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

  public void update() {
    m_loop.correct(m_Y);

    var error = m_loop.getError();
    m_atReferences = Math.abs(error.get(0, 0)) < kAngleTolerance
        && Math.abs(error.get(1, 0)) < kAngularVelocityTolerance;

    m_loop.predict();
  }

  public void reset() {
    m_loop.reset();
  }
}
