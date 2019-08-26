package edu.wpi.first.wpilibj.examples.statespaceelevator.subsystems;

import edu.wpi.first.wpilibj.controller.StateSpaceLoop;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.MatrixUtils;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;

@SuppressWarnings("MemberName")
public class ElevatorController {
  public static final double kPositionTolerance = 0.05;
  public static final double kVelocityTolerance = 2.0;

  private final Matrix<N1, N1> m_Y;
  private final StateSpaceLoop<N2, N1, N1> m_loop;
  private boolean m_atReferences;

  public ElevatorController() {
    m_Y = MatrixUtils.zeros(Nat.N1(), Nat.N1());
    m_loop = ElevatorCoeffs.makeElevatorLoop();
  }

  public void enable() {
    m_loop.enable();
  }

  public void disable() {
    m_loop.disable();
  }

  public void setReferences(double position, double velocity) {
    Matrix<N2, N1> nextR = MatrixUtils.vec(Nat.N2()).fill(position, velocity);
    m_loop.setNextR(nextR);
  }

  public boolean atReferences() {
    return m_atReferences;
  }

  public void setMeasuredPosition(double measuredPosition) {
    m_Y.set(0, 0, measuredPosition);
  }

  public double getControllerVoltage() {
    return m_loop.getU(0);
  }

  public double getEstimatedPosition() {
    return m_loop.getXhat(0);
  }

  public double getEstimatedVelocity() {
    return m_loop.getXhat(1);
  }

  public double getPositionError() {
    return m_loop.getError().get(0, 0);
  }

  public double getVelocityError() {
    return m_loop.getError().get(1, 0);
  }

  /**
   * Run the loop's correct and predict steps,
   * and update m_atReferences to reflect the new state of the control loop.
   */
  public void update() {
    m_loop.correct(m_Y);

    var error = m_loop.getError();
    m_atReferences = Math.abs(error.get(0, 0)) < kPositionTolerance
        && Math.abs(error.get(1, 0)) < kVelocityTolerance;
    m_loop.predict();
  }


  public void reset() {
    m_loop.reset();
  }
}
