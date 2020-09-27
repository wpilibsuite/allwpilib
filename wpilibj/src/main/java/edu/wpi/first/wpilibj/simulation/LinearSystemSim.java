/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.math.StateSpaceUtil;
import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;
import org.ejml.MatrixDimensionException;
import org.ejml.simple.SimpleMatrix;

/**
 * This class helps simulate linear systems. To use this class, do the following in the
 * {@link edu.wpi.first.wpilibj.IterativeRobotBase#simulationPeriodic} method.
 *
 * <p>Call {@link #setInput(double...)} with the inputs to the system (usually voltage).
 *
 * <p>Call {@link #update} to update the simulation.
 *
 * <p>Set simulated sensor readings with the simulated positions in {@link #getOutput(int)}
 *
 * @param <States> The number of states of the system.
 * @param <Inputs> The number of inputs to the system.
 * @param <Outputs> The number of outputs of the system.
 */
@SuppressWarnings("ClassTypeParameterName")
public class LinearSystemSim<States extends Num, Inputs extends Num,
      Outputs extends Num> {

  protected final LinearSystem<States, Inputs, Outputs> m_plant;

  @SuppressWarnings("MemberName")
  protected Matrix<States, N1> m_x;
  @SuppressWarnings("MemberName")
  protected Matrix<Outputs, N1> m_y;
  @SuppressWarnings("MemberName")
  protected Matrix<Inputs, N1> m_u;
  protected final Matrix<Outputs, N1> m_measurementStdDevs;

  /**
   * Create a SimLinearSystem. This simulator uses an {@link LinearSystem} to simulate
   * the state of the system. In simulationPeriodic, users should first set inputs from motors, update
   * the simulation and write simulated outputs to sensors.
   *
   * @param system             The system being controlled.
   * @param measurementStdDevs Standard deviations of measurements. Can be null if addNoise is false.
   */
  public LinearSystemSim(LinearSystem<States, Inputs, Outputs> system, Matrix<Outputs, N1> measurementStdDevs) {
    this.m_plant = system;
    this.m_measurementStdDevs = measurementStdDevs;

    m_x = new Matrix<>(new SimpleMatrix(system.getA().getNumRows(), 1));
    m_u = new Matrix<>(new SimpleMatrix(system.getB().getNumCols(), 1));
    m_y = new Matrix<>(new SimpleMatrix(system.getC().getNumRows(), 1));
  }

  protected Matrix<States, N1> updateX(Matrix<States, N1> currentXhat, Matrix<Inputs, N1> u, double dtSeconds) {
    return m_plant.calculateX(currentXhat, u, dtSeconds);
  }

  /**
   * Updates the simulation.
   *
   * @param dtSeconds The time that's passed since the last {@link #update(double)} call.
   */
  @SuppressWarnings("LocalVariableName")
  public void update(double dtSeconds) {
    // Update X. By default, this is the linear system dynamics X = Ax + Bu
    m_x = updateX(m_x, m_u, dtSeconds);

    // y = cx + du
    m_y = m_plant.calculateY(m_x, m_u);
    if (m_measurementStdDevs != null) {
      m_y = m_y.plus(StateSpaceUtil.makeWhiteNoiseVector(m_measurementStdDevs));
    }
  }

  public Matrix<Outputs, N1> getY() {
    return m_y;
  }

  public double getY(int row) {
    return m_y.get(row, 0);
  }

  public void setInput(Matrix<Inputs, N1> u) {
    this.m_u = u;
  }

  public void setInput(int row, double value) {
    m_u.set(row, 0, value);
  }

  public void setInput(double... u) {
    if (u.length != m_u.getNumRows()) {
      throw new MatrixDimensionException("Malformed input! Got " + u.length
            + " elements instead of " + m_u.getNumRows());
    }
    m_u = new Matrix<>(new SimpleMatrix(m_u.getNumRows(), 1, true, u));
  }

  public double getInput(int row) {
    return m_u.get(row, 0);
  }

  public Matrix<Inputs, N1> getInput() {
    return m_u;
  }

  public Matrix<Outputs, N1> getOutput() {
    return m_y;
  }

  public double getOutput(int row) {
    return m_y.get(row, 0);
  }

  /**
   * Sets the system state.
   *
   * @param state The state.
   */
  public void setState(Matrix<States, N1> state) {
    m_x = state;
  }

  /**
   * Get the current drawn by this simulated system. Override this method to add current
   * calculation.
   *
   * @return The currently drawn current.
   */
  public double getCurrentDrawAmps() {
    return 0.0;
  }
}
