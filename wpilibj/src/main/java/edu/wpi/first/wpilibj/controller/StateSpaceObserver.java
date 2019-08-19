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
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;

/**
 * Luenberger observers combine predictions from a model and measurements to
 * give an estimate of the true system state.
 *
 * <p>Luenberger observers use an L gain matrix to determine whether to trust the
 * model or measurements more. Kalman filter theory uses statistics to compute
 * an optimal L gain (alternatively called the Kalman gain, K) which minimizes
 * the sum of squares error in the state estimate.
 *
 * <p>Luenberger observers run the prediction and correction steps simultaneously
 * while Kalman filters run them sequentially. To implement a discrete-time
 * Kalman filter as a Luenberger observer, use the following mapping:
 * <pre>C = H, L = A * K</pre>
 * (H is the measurement matrix).
 *
 * <p>For more on the underlying math, read
 * https://file.tavsys.net/control/state-space-guide.pdf.
 */
@SuppressWarnings({"ClassTypeParameterName", "MemberName", "ParameterName"})
public class StateSpaceObserver<States extends Num, Inputs extends Num, Outputs extends Num> {
  private final StateSpacePlant<States, Inputs, Outputs> m_plant;
  @SuppressWarnings("LineLength")
  private final List<StateSpaceObserverCoeffs<States, Inputs, Outputs>> m_coefficients = new ArrayList<>();
  private int m_index;

  /**
   * Internal state estimate.
   */
  private Matrix<States, N1> m_Xhat;

  /**
   * Constructs a state-space observer with the given coefficients and plant.
   *
   * @param observerCoeffs Observer coefficients.
   * @param plant          The plant used for the prediction step.
   */
  public StateSpaceObserver(StateSpaceObserverCoeffs<States, Inputs, Outputs> observerCoeffs,
                            StateSpacePlant<States, Inputs, Outputs> plant) {
    m_plant = plant;
    addCoefficients(observerCoeffs);
    reset();
  }

  /**
   * Returns the Kalman gain matrix K.
   */
  public Matrix<States, Outputs> getK() {
    return getCoefficients().getK();
  }

  /**
   * Returns an element of the Kalman gain matrix K.
   *
   * @param i Row of K.
   * @param j Column of K.
   */
  public double getK(int i, int j) {
    return getK().get(i, j);
  }

  /**
   * Returns the state estimate x-hat.
   */
  public Matrix<States, N1> getXhat() {
    return m_Xhat;
  }

  /**
   * Returns an element of the state estimate x-hat.
   *
   * @param i Row of x-hat.
   */
  public double getXhat(int i) {
    return getXhat().get(i, 0);
  }

  /**
   * Set initial state estimate x-hat.
   *
   * @param xHat The state estimate x-hat.
   */
  public void setXhat(Matrix<States, N1> xHat) {
    m_Xhat = xHat;
  }

  /**
   * Set an element of the initial state estimate x-hat.
   *
   * @param i     Row of x-hat.
   * @param value Value for element of x-hat.
   */
  public void setXhat(int i, double value) {
    m_Xhat.set(i, 0, value);
  }

  /**
   * Resets the observer.
   */
  public void reset() {
    m_Xhat = MatrixUtils.zeros(m_plant.getStates());
  }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param newU New control input from controller.
   */
  public void predict(Matrix<Inputs, N1> newU) {
    m_Xhat = m_plant.updateX(getXhat(), newU);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   */
  public void correct(Matrix<Inputs, N1> u, Matrix<Outputs, N1> y) {

    m_Xhat = m_Xhat.plus(getK().times(y.minus(m_plant.updateY(getXhat(), u))));
  }

  /**
   * Adds the given coefficients to the observer for gain scheduling.
   *
   * @param coefficients Observer coefficients.
   */
  public void addCoefficients(StateSpaceObserverCoeffs<States, Inputs, Outputs> coefficients) {
    m_coefficients.add(coefficients);
  }

  /**
   * Returns the observer coefficients with the given index.
   *
   * @param index Index of coefficients.
   */
  public StateSpaceObserverCoeffs<States, Inputs, Outputs> getCoefficients(int index) {
    return m_coefficients.get(index);
  }

  /**
   * Returns the current observer coefficients.
   */
  public StateSpaceObserverCoeffs<States, Inputs, Outputs> getCoefficients() {
    return getCoefficients(getIndex());
  }

  /**
   * Sets the current observer to be "index", clamped to be within range. This
   * can be used for gain scheduling.
   *
   * @param index The controller index.
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
   * Returns the current observer index.
   */
  public int getIndex() {
    return m_index;
  }
}
