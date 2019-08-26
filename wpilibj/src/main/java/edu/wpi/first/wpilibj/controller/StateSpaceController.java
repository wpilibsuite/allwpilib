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
 * Contains the controller coefficients and logic for a state-space controller.
 *
 * <p>State-space controllers generally use the control law u = -Kx. The
 * feedforward used is u_ff = K_ff * (r_k+1 - A * r_k).
 *
 * <p>For more on the underlying math, read
 * https://file.tavsys.net/control/state-space-guide.pdf.
 */
@SuppressWarnings({"ClassTypeParameterName", "MemberName", "ParameterName"})
public class StateSpaceController<States extends Num, Inputs extends Num, Outputs extends Num> {
  private final StateSpacePlant<States, Inputs, Outputs> m_plant;
  @SuppressWarnings("LineLength")
  private final List<StateSpaceControllerCoeffs<States, Inputs, Outputs>> m_coefficients = new ArrayList<>();
  private int m_index;
  private boolean m_enabled;

  // Current reference.
  private Matrix<States, N1> m_r;

  // Computed controller output after being capped.
  private Matrix<Inputs, N1> m_u;

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param controllerCoeffs Controller coefficients.
   * @param plant            The plant used for the feedforward calculation.
   */
  public StateSpaceController(StateSpaceControllerCoeffs<States, Inputs, Outputs> controllerCoeffs,
                              StateSpacePlant<States, Inputs, Outputs> plant) {
    m_plant = plant;
    addCoefficients(controllerCoeffs);
    reset();
  }

  /**
   * Enables controller.
   */
  public void enable() {
    m_enabled = true;
  }

  /**
   * Disables controller, zeroing controller output U.
   */
  public void disable() {
    m_enabled = false;
    m_u = MatrixUtils.zeros(m_plant.getInputs());
  }

  /**
   * Returns the controller matrix K.
   */
  public Matrix<Inputs, States> getK() {
    return getCoefficients().getK();
  }

  /**
   * Returns an element of the controller matrix K.
   *
   * @param i Row of K.
   * @param j Column of K.
   */
  public double getK(int i, int j) {
    return getK().get(i, j);
  }

  /**
   * Returns the feedforward controller matrix Kff.
   */
  public Matrix<Inputs, States> getKff() {
    return getCoefficients().getKff();
  }

  /**
   * Returns an element of the feedforward controller matrix Kff.
   *
   * @param i Row of Kff.
   * @param j Column of Kff.
   */
  public double getKff(int i, int j) {
    return getKff().get(i, j);
  }

  /**
   * Returns the reference vector r.
   */
  public Matrix<States, N1> getR() {
    return m_r;
  }

  /**
   * Returns an element of the reference vector r.
   *
   * @param i Row of r.
   */
  public double getR(int i) {
    return getR().get(i, 0);
  }

  /**
   * Returns the control input vector u.
   */
  public Matrix<Inputs, N1> getU() {
    return m_u;
  }

  /**
   * Returns an element of the control input vector u.
   *
   * @param i Row of u.
   */
  public double getU(int i) {
    return getU().get(i, 0);
  }

  /**
   * Resets the controller.
   */
  public void reset() {
    m_r = MatrixUtils.zeros(m_plant.getStates());
    m_u = MatrixUtils.zeros(m_plant.getInputs());
  }

  /**
   * Update controller without setting a new reference.
   *
   * @param x The current state x.
   */
  public void update(Matrix<States, N1> x) {
    if (m_enabled) {
      m_u = getK().times(m_r.minus(x)).plus(getKff().times(m_r.minus(m_plant.getA().times(m_r))));
      capU();
    }
  }

  /**
   * Set a new reference and update the controller.
   *
   * @param x     The current state x.
   * @param nextR The next reference vector r.
   */
  public void update(Matrix<States, N1> x, Matrix<States, N1> nextR) {
    if (m_enabled) {
      m_u = getK().times(m_r.minus(x)).plus(getKff().times(nextR.minus(m_plant.getA().times(m_r))));
      capU();
      m_r = nextR;
    }
  }

  private void capU() {
    for (int i = 0; i < m_plant.getInputs().getNum(); ++i) {
      if (getU(i) > getCoefficients().getUmax(i)) {
        m_u.set(i, 0, getCoefficients().getUmax(i));
      } else if (getU(i) < getCoefficients().getUmin(i)) {
        m_u.set(i, 0, getCoefficients().getUmin(i));
      }
    }
  }

  /**
   * Sets the current controller to be "index", clamped to be within range. This
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
   * Returns the currnet controller index.
   */
  public int getIndex() {
    return m_index;
  }

  /**
   * Adds the given coefficients to the controller for gain scheduling.
   */
  public void addCoefficients(StateSpaceControllerCoeffs<States, Inputs, Outputs> coefficients) {
    m_coefficients.add(coefficients);
  }

  /**
   * Returns the controller coefficients with the given index.
   *
   * @param index Index of coefficients.
   */
  public StateSpaceControllerCoeffs<States, Inputs, Outputs> getCoefficients(int index) {
    return m_coefficients.get(index);
  }

  /**
   * Returns the current controller coefficients.
   */
  public StateSpaceControllerCoeffs<States, Inputs, Outputs> getCoefficients() {
    return getCoefficients(getIndex());
  }

  /**
   * Returns whether the controller is currently enabled.
   *
   * @return If the controller's output is enabled
   */
  public boolean isEnabled() {
    return m_enabled;
  }
}
