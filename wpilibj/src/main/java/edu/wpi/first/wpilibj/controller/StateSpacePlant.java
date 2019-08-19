/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import java.util.ArrayList;
import java.util.List;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.MatrixUtils;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;

@SuppressWarnings({"ClassTypeParameterName", "MemberName", "ParameterName"})
public class StateSpacePlant<States extends Num, Inputs extends Num, Outputs extends Num> {
  @SuppressWarnings("LineLength")
  private final List<StateSpacePlantCoeffs<States, Inputs, Outputs>> m_coefficients = new ArrayList<>();
  private int m_index;

  // The number of states, inputs, and outputs of this plant.
  protected final Nat<States> kStates;
  protected final Nat<Inputs> kInputs;
  protected final Nat<Outputs> kOutputs;

  private Matrix<States, N1> m_x;
  private Matrix<Outputs, N1> m_y;

  /**
   * Constructs a plant with the given coefficients.
   *
   * @param plantCoeffs Plant coefficients.
   */
  public StateSpacePlant(StateSpacePlantCoeffs<States, Inputs, Outputs> plantCoeffs) {
    addCoefficients(plantCoeffs);
    kStates = getCoefficients().getStates();
    kInputs = getCoefficients().getInputs();
    kOutputs = getCoefficients().getOutputs();
    reset();
  }

  /**
   * Returns the system matrix A.
   */
  public Matrix<States, States> getA() {
    return getCoefficients().getA();
  }

  /**
   * Returns an element of the system matrix A.
   *
   * @param i Row of A.
   * @param j Column of A.
   */
  public double getA(int i, int j) {
    return getA().get(i, j);

  }

  /**
   * Returns the input matrix B.
   */
  public Matrix<States, Inputs> getB() {
    return getCoefficients().getB();
  }

  /**
   * Returns an element of the input matrix B.
   *
   * @param i Row of B.
   * @param j Column of B.
   */
  public double getB(int i, int j) {
    return getB().get(i, j);
  }

  /**
   * Returns the output matrix C.
   */
  public Matrix<Outputs, States> getC() {
    return getCoefficients().getC();
  }

  /**
   * Returns an element of the output matrix C.
   *
   * @param i Row of C.
   * @param j Column of C.
   */
  public double getC(int i, int j) {
    return getC().get(i, j);
  }

  /**
   * Returns the feedthrough matrix D.
   */
  public Matrix<Outputs, Inputs> getD() {
    return getCoefficients().getD();
  }

  /**
   * Returns an element of the feedthrough matrix D.
   *
   * @param i Row of D.
   * @param j Column of D.
   */
  public double getD(int i, int j) {
    return getD().get(i, j);
  }

  /**
   * Returns the current state X.
   */
  public Matrix<States, N1> getX() {
    return m_x;
  }

  /**
   * Returns an element of the current state x.
   *
   * @param i Row of x.
   */
  public double getX(int i) {
    return getX().get(i, 0);
  }

  /**
   * Returns the current measurement vector y.
   */
  public Matrix<Outputs, N1> getY() {
    return m_y;
  }

  /**
   * Returns an element of the current measurement vector y.
   *
   * @param i Row of y.
   */
  public double getY(int i) {
    return getY().get(i, 0);
  }

  /**
   * Set the initial state x.
   *
   * @param x The initial state.
   */
  public void setX(Matrix<States, N1> x) {
    m_x = x;
  }

  /**
   * Set an element of the initial state x.
   *
   * @param i     Row of x.
   * @param value Value of element of x.
   */
  public void setX(int i, double value) {
    m_x.set(i, 0, value);
  }

  /**
   * Set the current measurement y.
   *
   * @param y The current measurement.
   */
  public void setY(Matrix<Outputs, N1> y) {
    m_y = y;
  }

  /**
   * Set an element of the current measurement y.
   *
   * @param i     Row of y.
   * @param value Value of element of y.
   */
  public void setY(int i, double value) {
    m_y.set(i, 0, value);
  }

  /**
   * Adds the given coefficients to the plant for gain scheduling.
   *
   * @param coefficients Plant coefficients.
   */
  public void addCoefficients(StateSpacePlantCoeffs<States, Inputs, Outputs> coefficients) {
    m_coefficients.add(coefficients);
  }

  /**
   * Returns the plant coefficients with the given index.
   *
   * @param index Index of coefficients.
   */
  public StateSpacePlantCoeffs<States, Inputs, Outputs> getCoefficients(int index) {
    return m_coefficients.get(index);
  }

  /**
   * Returns the current plant coefficients.
   */
  public StateSpacePlantCoeffs<States, Inputs, Outputs> getCoefficients() {
    return getCoefficients(getIndex());
  }

  /**
   * Sets the current plant to be "index", clamped to be within range. This can
   * be used for gain scheduling to make the current model match changed plant
   * behavior.
   *
   * @param index The plant index.
   */
  public void setIndex(int index) {
    if (index < 0) {
      m_index = 0;
    } else if (index >= m_coefficients.size()) {
      m_index = m_coefficients.size() - 1;
    } else {
      m_index = index;
    }
  }

  /**
   * Returns the current plant index.
   */
  public int getIndex() {
    return m_index;
  }

  /**
   * Resets the plant.
   */
  public void reset() {
    m_x = MatrixUtils.zeros(kStates);
  }

  /**
   * Computes the new x and y given the control input.
   *
   * @param x The current state.
   * @param u The control input.
   */
  public void update(Matrix<States, N1> x, Matrix<Inputs, N1> u) {
    m_x = updateX(x, u);
    m_y = updateY(x, u);
  }

  /**
   * Computes the new x given the old x and the control input.
   *
   * <p>This is used by state observers directly to run updates based on state
   * estimate.
   *
   * @param x The current state.
   * @param u The control input.
   */
  public Matrix<States, N1> updateX(Matrix<States, N1> x, Matrix<Inputs, N1> u) {
    return getA().times(x).plus(getB().times(u));
  }

  /**
   * Computes the new y given the control input.
   *
   * <p>This is used by state observers directly to run updates based on state
   * estimate.
   *
   * @param x The current state.
   * @param u The control input.
   */
  public Matrix<Outputs, N1> updateY(Matrix<States, N1> x, Matrix<Inputs, N1> u) {
    return getC().times(x).plus(getD().times(u));
  }

  /**
   * Returns the number of states of this plant.
   *
   * @return The number of states.
   */
  public Nat<States> getStates() {
    return kStates;
  }

  /**
   * Returns the number of inputs of this plant.
   *
   * @return The number of inputs.
   */
  public Nat<Inputs> getInputs() {
    return kInputs;
  }

  /**
   * Returns the number of outputs of this plant.
   *
   * @return The number of outputs.
   */
  public Nat<Outputs> getOutputs() {
    return kOutputs;
  }
}
