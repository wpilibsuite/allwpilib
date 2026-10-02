// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.autodiff;

import org.ejml.simple.SimpleMatrix;

/**
 * This class calculates the gradient of a variable with respect to a vector of variables.
 *
 * <p>The gradient is only recomputed if the variable expression is quadratic or higher order.
 */
public class Gradient implements AutoCloseable {
  private long m_handle;
  private int m_rows;

  /**
   * Constructs a Gradient object.
   *
   * @param variable Variable of which to compute the gradient.
   * @param wrt Variable with respect to which to compute the gradient.
   */
  public Gradient(Variable variable, Variable wrt) {
    this(variable, new VariableMatrix(wrt));
  }

  /**
   * Constructs a Gradient object.
   *
   * @param variable Variable of which to compute the gradient.
   * @param wrt Vector of variables with respect to which to compute the gradient.
   */
  public Gradient(Variable variable, VariableMatrix wrt) {
    assert wrt.cols() == 1;

    m_handle = GradientJNI.create(variable.getHandle(), wrt.getHandles());
    m_rows = wrt.rows();
  }

  /**
   * Constructs a Gradient object.
   *
   * @param variable Variable of which to compute the gradient.
   * @param wrt Vector of variables with respect to which to compute the gradient.
   */
  public Gradient(Variable variable, VariableBlock wrt) {
    this(variable, new VariableMatrix(wrt));
  }

  @Override
  public void close() {
    if (m_handle != 0) {
      GradientJNI.destroy(m_handle);
      m_handle = 0;
    }
  }

  /**
   * Returns the gradient as a VariableMatrix.
   *
   * <p>This is useful when constructing optimization problems with derivatives in them.
   *
   * @return The gradient as a VariableMatrix.
   */
  public VariableMatrix get() {
    return new VariableMatrix(m_rows, 1, GradientJNI.get(m_handle));
  }

  /**
   * Evaluates the gradient at wrt's value.
   *
   * @return The gradient at wrt's value.
   */
  public SimpleMatrix value() {
    return GradientJNI.value(m_handle).toSimpleMatrix(m_rows, 1);
  }
}
