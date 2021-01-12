// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.math.StateSpaceUtil;
import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;
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
 * @param <States> The number of states of the system.
 * @param <Inputs> The number of inputs to the system.
 * @param <Outputs> The number of outputs of the system.
 */
@SuppressWarnings("ClassTypeParameterName")
public class LinearSystemSim<States extends Num, Inputs extends Num, Outputs extends Num> {
  // The plant that represents the linear system.
  protected final LinearSystem<States, Inputs, Outputs> m_plant;

  // Variables for state, output, and input.
  @SuppressWarnings("MemberName")
  protected Matrix<States, N1> m_x;

  @SuppressWarnings("MemberName")
  protected Matrix<Outputs, N1> m_y;

  @SuppressWarnings("MemberName")
  protected Matrix<Inputs, N1> m_u;

  // The standard deviations of measurements, used for adding noise
  // to the measurements.
  protected final Matrix<Outputs, N1> m_measurementStdDevs;

  /**
   * Creates a simulated generic linear system.
   *
   * @param system The system to simulate.
   */
  public LinearSystemSim(LinearSystem<States, Inputs, Outputs> system) {
    this(system, null);
  }

  /**
   * Creates a simulated generic linear system with measurement noise.
   *
   * @param system The system being controlled.
   * @param measurementStdDevs Standard deviations of measurements. Can be null if no noise is
   *     desired.
   */
  public LinearSystemSim(
      LinearSystem<States, Inputs, Outputs> system, Matrix<Outputs, N1> measurementStdDevs) {
    this.m_plant = system;
    this.m_measurementStdDevs = measurementStdDevs;

    m_x = new Matrix<>(new SimpleMatrix(system.getA().getNumRows(), 1));
    m_u = new Matrix<>(new SimpleMatrix(system.getB().getNumCols(), 1));
    m_y = new Matrix<>(new SimpleMatrix(system.getC().getNumRows(), 1));
  }

  /**
   * Updates the simulation.
   *
   * @param dtSeconds The time between updates.
   */
  @SuppressWarnings("LocalVariableName")
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
  @SuppressWarnings("ParameterName")
  public void setInput(Matrix<Inputs, N1> u) {
    this.m_u = clampInput(u);
  }

  /**
   * Sets the system inputs.
   *
   * @param row The row in the input matrix to set.
   * @param value The value to set the row to.
   */
  public void setInput(int row, double value) {
    m_u.set(row, 0, value);
    m_u = clampInput(m_u);
  }

  /**
   * Sets the system inputs.
   *
   * @param u An array of doubles that represent the inputs of the system.
   */
  @SuppressWarnings("ParameterName")
  public void setInput(double... u) {
    if (u.length != m_u.getNumRows()) {
      throw new MatrixDimensionException(
          "Malformed input! Got " + u.length + " elements instead of " + m_u.getNumRows());
    }
    m_u = new Matrix<>(new SimpleMatrix(m_u.getNumRows(), 1, true, u));
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
   * Returns the current drawn by this simulated system. Override this method to add a custom
   * current calculation.
   *
   * @return The current drawn by this simulated mechanism.
   */
  public double getCurrentDrawAmps() {
    return 0.0;
  }

  /**
   * Updates the state estimate of the system.
   *
   * @param currentXhat The current state estimate.
   * @param u The system inputs (usually voltage).
   * @param dtSeconds The time difference between controller updates.
   * @return The new state.
   */
  @SuppressWarnings("ParameterName")
  protected Matrix<States, N1> updateX(
      Matrix<States, N1> currentXhat, Matrix<Inputs, N1> u, double dtSeconds) {
    return m_plant.calculateX(currentXhat, u, dtSeconds);
  }

  /**
   * Clamp the input vector such that no element exceeds the given voltage. If any does, the
   * relative magnitudes of the input will be maintained.
   *
   * @param u The input vector.
   * @return The normalized input.
   */
  protected Matrix<Inputs, N1> clampInput(Matrix<Inputs, N1> u) {
    return StateSpaceUtil.normalizeInputVector(u, RobotController.getBatteryVoltage());
  }
}
