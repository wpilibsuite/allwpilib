// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.autodiff;

import org.ejml.simple.SimpleMatrix;

/**
 * This class calculates the Hessian of a variable with respect to a vector of variables.
 *
 * <p>The gradient tree is cached so subsequent Hessian calculations are faster, and the Hessian is
 * only recomputed if the variable expression is nonlinear.
 */
public class Hessian implements AutoCloseable {
  private long m_handle;
  private int m_rows;

  /**
   * Constructs a Hessian object.
   *
   * @param variable Variable of which to compute the Hessian.
   * @param wrt Variable with respect to which to compute the Hessian.
   */
  public Hessian(Variable variable, Variable wrt) {
    this(variable, new VariableMatrix(wrt));
  }

  /**
   * Constructs a Hessian object.
   *
   * @param variable Variable of which to compute the Hessian.
   * @param wrt Vector of variables with respect to which to compute the Hessian.
   */
  public Hessian(Variable variable, VariableMatrix wrt) {
    assert wrt.cols() == 1;

    m_handle = HessianJNI.create(variable.getHandle(), wrt.getHandles());
    m_rows = wrt.rows();
  }

  /**
   * Constructs a Hessian object.
   *
   * @param variable Variable of which to compute the Hessian.
   * @param wrt Vector of variables with respect to which to compute the Hessian.
   */
  public Hessian(Variable variable, VariableBlock wrt) {
    this(variable, new VariableMatrix(wrt));
  }

  @Override
  public void close() {
    if (m_handle != 0) {
      HessianJNI.destroy(m_handle);
      m_handle = 0;
    }
  }

  /**
   * Returns the Hessian as a VariableMatrix.
   *
   * <p>This is useful when constructing optimization problems with derivatives in them.
   *
   * @return The Hessian as a VariableMatrix.
   */
  public VariableMatrix get() {
    return new VariableMatrix(m_rows, m_rows, HessianJNI.get(m_handle));
  }

  /**
   * Evaluates the Hessian at wrt's value.
   *
   * @return The Hessian at wrt's value.
   */
  public SimpleMatrix value() {
    return HessianJNI.value(m_handle).toSimpleMatrix(m_rows, m_rows);
  }
}
