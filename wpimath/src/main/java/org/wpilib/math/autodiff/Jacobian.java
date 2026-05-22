// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.autodiff;

import org.ejml.simple.SimpleMatrix;

/**
 * This class calculates the Jacobian of a vector of variables with respect to a vector of
 * variables.
 *
 * <p>The Jacobian is only recomputed if the variable expression is quadratic or higher order.
 */
public class Jacobian implements AutoCloseable {
  private long m_handle;
  private int m_rows;
  private int m_cols;

  /**
   * Constructs a Jacobian object.
   *
   * @param variable Variable of which to compute the Jacobian.
   * @param wrt Variable with respect to which to compute the Jacobian.
   */
  public Jacobian(Variable variable, Variable wrt) {
    this(new VariableMatrix(variable), new VariableMatrix(wrt));
  }

  /**
   * Constructs a Jacobian object.
   *
   * @param variable Variable of which to compute the Jacobian.
   * @param wrt Vector of variables with respect to which to compute the Jacobian.
   */
  public Jacobian(Variable variable, VariableMatrix wrt) {
    this(new VariableMatrix(variable), wrt);
  }

  /**
   * Constructs a Jacobian object.
   *
   * @param variable Variable of which to compute the Jacobian.
   * @param wrt Vector of variables with respect to which to compute the Jacobian.
   */
  public Jacobian(Variable variable, VariableBlock wrt) {
    this(new VariableMatrix(variable), new VariableMatrix(wrt));
  }

  /**
   * Constructs a Jacobian object.
   *
   * @param variables Vector of variables of which to compute the Jacobian.
   * @param wrt Vector of variables with respect to which to compute the Jacobian.
   */
  public Jacobian(VariableMatrix variables, VariableMatrix wrt) {
    assert variables.cols() == 1;
    assert wrt.cols() == 1;

    m_handle = JacobianJNI.create(variables.getHandles(), wrt.getHandles());
    m_rows = variables.rows();
    m_cols = wrt.rows();
  }

  /**
   * Constructs a Jacobian object.
   *
   * @param variables Vector of variables of which to compute the Jacobian.
   * @param wrt Vector of variables with respect to which to compute the Jacobian.
   */
  public Jacobian(VariableMatrix variables, VariableBlock wrt) {
    this(variables, new VariableMatrix(wrt));
  }

  @Override
  public void close() {
    if (m_handle != 0) {
      JacobianJNI.destroy(m_handle);
      m_handle = 0;
    }
  }

  /**
   * Returns the Jacobian as a VariableMatrix.
   *
   * <p>This is useful when constructing optimization problems with derivatives in them.
   *
   * @return The Jacobian as a VariableMatrix.
   */
  public VariableMatrix get() {
    return new VariableMatrix(m_rows, m_cols, JacobianJNI.get(m_handle));
  }

  /**
   * Evaluates the Jacobian at wrt's value.
   *
   * @return The Jacobian at wrt's value.
   */
  public SimpleMatrix value() {
    return JacobianJNI.value(m_handle).toSimpleMatrix(m_rows, m_cols);
  }
}
