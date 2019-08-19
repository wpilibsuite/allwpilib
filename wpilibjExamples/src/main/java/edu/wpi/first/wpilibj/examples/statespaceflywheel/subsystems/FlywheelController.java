package edu.wpi.first.wpilibj.examples.statespaceflywheel.subsystems;

import edu.wpi.first.wpilibj.controller.StateSpaceLoop;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.MatrixUtils;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.numbers.N1;

public class FlywheelController {
  // Angular velocity tolerance in radians/sec.
  public static final double kTolerance = 10.0;

  private Matrix<N1, N1> m_Y;
  private StateSpaceLoop<N1, N1, N1> m_loop;
  private boolean m_atReference = false;

  public FlywheelController() {
    m_Y = MatrixUtils.zeros(Nat.N1());
    m_loop = FlywheelCoeffs.makeFlywheelLoop();
  }

  public void enable() {
    m_loop.enable();
  }

  public void disable() {
    m_loop.disable();
  }

  public void setVelocityReference(double angularVelocity) {
    Matrix<N1, N1> nextR = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(angularVelocity);
    m_loop.setNextR(nextR);
  }

  public boolean atReference() {
    return m_atReference;
  }

  public void setMeasuredVelocity(double angularVelocity) {
    m_Y.set(0, 0, angularVelocity);
  }

  public double getControllerVoltage() {
    return m_loop.getU(0);
  }

  public double getEstimatedVelocity() {
    return m_loop.getXhat(0);
  }

  public double getError() {
    return m_loop.getError().get(0, 0);
  }

  public void update() {
    if(Math.abs(m_loop.getNextR(0)) < 1.0) {
      // Kill power at low angular velocities
      m_loop.disable();
    }

    m_loop.correct(m_Y);

    m_atReference = Math.abs(getError()) < kTolerance && m_loop.getNextR(0) > 1.0;

    m_loop.predict();
  }

  public void reset() {
    m_loop.reset();
  }
}
