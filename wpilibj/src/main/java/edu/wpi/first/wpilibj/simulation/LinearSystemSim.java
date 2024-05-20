// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.StateSpaceUtil;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.system.LinearSystem;
import org.ejml.MatrixDimensionException;
import org.ejml.simple.SimpleMatrix;

/**
 * This class helps simulate linear systems. To use this class, do the following in the {@link
 * edu.wpi.first.wpilibj.IterativeRobotBase#simulationPeriodic} method.
 *
 * <p>Call {@link #setInput(double...)} with the inputs to the system (usually voltage).
 *
 * <p>Call {@link #update} to update the simulation.
 *
 * <p>Set simulated sensor readings with the simulated positions in {@link #getOutput()}
 *
 * @param <States> Number of states of the system.
 * @param <Inputs> Number of inputs to the system.
 * @param <Outputs> Number of outputs of the system.
 */
public class LinearSystemSim<States extends Num, Inputs extends Num, Outputs extends Num> {
  /** The plant that represents the linear system. */
  protected final LinearSystem<States, Inputs, Outputs> m_plant;

  /** State vector. */
  protected Matrix<States, N1> m_x;

  /** Input vector. */
  protected Matrix<Inputs, N1> m_u;

  /** Output vector. */
  protected Matrix<Outputs, N1> m_y;

  /** The standard deviations of measurements, used for adding noise to the measurements. */
  protected final Matrix<Outputs, N1> m_measurementStdDevs;

  /**
   * Creates a simulated generic linear system with measurement noise.
   *
   * @param system The system being controlled.
   * @param measurementStdDevs Standard deviations of measurements. Can be empty if no noise is
   *     desired. If present must have same number of items as Outputs
   */
  public LinearSystemSim(
      LinearSystem<States, Inputs, Outputs> system, double... measurementStdDevs) {
    if (measurementStdDevs.length != 0 && measurementStdDevs.length != system.getC().getNumRows()) {
      throw new MatrixDimensionException(
          "Malformed measurementStdDevs! Got "
              + measurementStdDevs.length
              + " elements instead of "
              + system.getC().getNumRows());
    }
    this.m_plant = system;
    if (measurementStdDevs.length == 0) {
      m_measurementStdDevs = new Matrix<>(new SimpleMatrix(system.getC().getNumRows(), 1));
    } else {
      m_measurementStdDevs = new Matrix<>(new SimpleMatrix(measurementStdDevs));
    }
    m_x = new Matrix<>(new SimpleMatrix(system.getA().getNumRows(), 1));
    m_u = new Matrix<>(new SimpleMatrix(system.getB().getNumCols(), 1));
    m_y = new Matrix<>(new SimpleMatrix(system.getC().getNumRows(), 1));
  }

  /**
   * Updates the simulation.
   *
   * @param dtSeconds The time between updates.
   */
  public void update(double dtSeconds) {
    // Update X. By default, this is the linear system dynamics X = Ax + Bu
    m_x = updateX(m_x, m_u, dtSeconds);

    // y = cx + du
    m_y = m_plant.calculateY(m_x, m_u);

    // Add measurement noise.
    if (m_measurementStdDevs != null) {
      m_y = m_y.plus(StateSpaceUtil.makeWhiteNoiseVector(m_measurementStdDevs));
    }
  }

  /**
   * Returns the current output of the plant.
   *
   * @return The current output of the plant.
   */
  public Matrix<Outputs, N1> getOutput() {
    return m_y;
  }

  /**
   * Returns an element of the current output of the plant.
   *
   * @param row The row to return.
   * @return An element of the current output of the plant.
   */
  public double getOutput(int row) {
    return m_y.get(row, 0);
  }

  /**
   * Sets the system inputs (usually voltages).
   *
   * @param u The system inputs.
   */
  public void setInput(Matrix<Inputs, N1> u) {
    this.m_u = u;
  }

  /**
   * Sets the system inputs.
   *
   * @param row The row in the input matrix to set.
   * @param value The value to set the row to.
   */
  public void setInput(int row, double value) {
    m_u.set(row, 0, value);
  }

  /**
   * Sets the system inputs.
   *
   * @param u An array of doubles that represent the inputs of the system.
   */
  public void setInput(double... u) {
    if (u.length != m_u.getNumRows()) {
      throw new MatrixDimensionException(
          "Malformed input! Got " + u.length + " elements instead of " + m_u.getNumRows());
    }
    m_u = new Matrix<>(new SimpleMatrix(m_u.getNumRows(), 1, true, u));
  }

  /**
   * Returns the current input of the plant.
   *
   * @return The current input of the plant.
   */
  public Matrix<Inputs, N1> getInput() {
    return m_u;
  }

  /**
   * Returns an element of the current input of the plant.
   *
   * @param row The row to return.
   * @return An element of the current input of the plant.
   */
  public double getInput(int row) {
    return m_u.get(row, 0);
  }

  /**
   * Sets the system state.
   *
   * @param state The new state.
   */
  public void setState(Matrix<States, N1> state) {
    m_x = state;
  }

  /**
   * Updates the state estimate of the system.
   *
   * @param currentXhat The current state estimate.
   * @param u The system inputs (usually voltage).
   * @param dtSeconds The time difference between controller updates.
   * @return The new state.
   */
  protected Matrix<States, N1> updateX(
      Matrix<States, N1> currentXhat, Matrix<Inputs, N1> u, double dtSeconds) {
    return m_plant.calculateX(currentXhat, u, dtSeconds);
  }

  /**
   * Clamp the input vector such that no element exceeds the maximum allowed value. If any does, the
   * relative magnitudes of the input will be maintained.
   *
   * @param maxInput The maximum magnitude of the input vector after clamping.
   */
  protected void clampInput(double maxInput) {
    m_u = StateSpaceUtil.desaturateInputVector(m_u, maxInput);
  }
}
