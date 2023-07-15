// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Num;
import java.util.function.BiFunction;

public final class RungeKuttaTimeVarying {
  private RungeKuttaTimeVarying() {
    // Utility class
  }

  /**
   * Performs 4th order Runge-Kutta integration of dx/dt = f(t, y) for dt.
   *
   * @param <Rows> Rows in y.
   * @param <Cols> Columns in y.
   * @param f The function to integrate. It must take two arguments t and y.
   * @param t The initial value of t.
   * @param y The initial value of y.
   * @param dtSeconds The time over which to integrate.
   */
  public static <Rows extends Num, Cols extends Num> Matrix<Rows, Cols> rungeKuttaTimeVarying(
      BiFunction<Double, Matrix<Rows, Cols>, Matrix<Rows, Cols>> f,
      double t,
      Matrix<Rows, Cols> y,
      double dtSeconds) {
    final var h = dtSeconds;

    Matrix<Rows, Cols> k1 = f.apply(t, y);
    Matrix<Rows, Cols> k2 = f.apply(t + dtSeconds * 0.5, y.plus(k1.times(h * 0.5)));
    Matrix<Rows, Cols> k3 = f.apply(t + dtSeconds * 0.5, y.plus(k2.times(h * 0.5)));
    Matrix<Rows, Cols> k4 = f.apply(t + dtSeconds, y.plus(k3.times(h)));

    return y.plus((k1.plus(k2.times(2.0)).plus(k3.times(2.0)).plus(k4)).times(h / 6.0));
  }
}
