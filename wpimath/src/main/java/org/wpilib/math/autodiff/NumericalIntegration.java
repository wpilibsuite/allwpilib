// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.autodiff;

import java.util.function.BiFunction;

/** Numerical integration utilities. */
public final class NumericalIntegration {
  private NumericalIntegration() {
    // Utility class.
  }

  /**
   * Performs 4th order Runge-Kutta integration of dx/dt = f(x, u) for dt.
   *
   * @param f The function to integrate. It must take two arguments x and u.
   * @param x The initial value of x.
   * @param u The value u held constant over the integration period.
   * @param dt The time over which to integrate.
   * @return the integration of dx/dt = f(x, u) for dt.
   */
  public static VariableMatrix rk4(
      BiFunction<VariableMatrix, VariableMatrix, VariableMatrix> f,
      VariableBlock x,
      VariableBlock u,
      double dt) {
    return rk4(f, new VariableMatrix(x), new VariableMatrix(u), dt);
  }

  /**
   * Performs 4th order Runge-Kutta integration of dx/dt = f(x, u) for dt.
   *
   * @param f The function to integrate. It must take two arguments x and u.
   * @param x The initial value of x.
   * @param u The value u held constant over the integration period.
   * @param dt The time over which to integrate.
   * @return the integration of dx/dt = f(x, u) for dt.
   */
  public static VariableMatrix rk4(
      BiFunction<VariableMatrix, VariableMatrix, VariableMatrix> f,
      VariableBlock x,
      VariableMatrix u,
      double dt) {
    return rk4(f, new VariableMatrix(x), u, dt);
  }

  /**
   * Performs 4th order Runge-Kutta integration of dx/dt = f(x, u) for dt.
   *
   * @param f The function to integrate. It must take two arguments x and u.
   * @param x The initial value of x.
   * @param u The value u held constant over the integration period.
   * @param dt The time over which to integrate.
   * @return the integration of dx/dt = f(x, u) for dt.
   */
  public static VariableMatrix rk4(
      BiFunction<VariableMatrix, VariableMatrix, VariableMatrix> f,
      VariableMatrix x,
      VariableBlock u,
      double dt) {
    return rk4(f, x, new VariableMatrix(u), dt);
  }

  /**
   * Performs 4th order Runge-Kutta integration of dx/dt = f(x, u) for dt.
   *
   * @param f The function to integrate. It must take two arguments x and u.
   * @param x The initial value of x.
   * @param u The value u held constant over the integration period.
   * @param dt The time over which to integrate.
   * @return the integration of dx/dt = f(x, u) for dt.
   */
  public static VariableMatrix rk4(
      BiFunction<VariableMatrix, VariableMatrix, VariableMatrix> f,
      VariableMatrix x,
      VariableMatrix u,
      double dt) {
    var h = dt;

    var k1 = f.apply(x, u);
    var k2 = f.apply(x.plus(k1.times(h * 0.5)), u);
    var k3 = f.apply(x.plus(k2.times(h * 0.5)), u);
    var k4 = f.apply(x.plus(k3.times(h)), u);

    return x.plus(k1.plus(k2.times(2.0)).plus(k3.times(2.0)).plus(k4).times(h / 6.0));
  }
}
