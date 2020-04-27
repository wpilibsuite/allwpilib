/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.system;

import java.util.function.BiFunction;
import java.util.function.DoubleFunction;
import java.util.function.Function;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;

public final class RungeKutta {
  private RungeKutta() {
    // utility Class
  }

  /**
   * Performs Runge Kutta integration (4th order).
   *
   * @param f         The function to integrate, which takes one argument x.
   * @param x         The initial value of x.
   * @param dtSeconds The time over which to integrate.
   * @return the integration of dx/dt = f(x) for dt.
   */
  @SuppressWarnings("ParameterName")
  public static double rungeKutta(
          DoubleFunction<Double> f,
          double x,
          double dtSeconds
  ) {
    final var halfDt = 0.5 * dtSeconds;
    final var k1 = f.apply(x);
    final var k2 = f.apply(x + k1 * halfDt);
    final var k3 = f.apply(x + k2 * halfDt);
    final var k4 = f.apply(x + k3 * dtSeconds);
    return x + dtSeconds / 6.0 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
  }

  /**
   * Performs Runge Kutta integration (4th order).
   *
   * @param f         The function to integrate. It must take two arguments x and u.
   * @param x         The initial value of x.
   * @param u         The value u held constant over the integration period.
   * @param dtSeconds The time over which to integrate.
   * @return The result of Runge Kutta integration (4th order).
   */
  @SuppressWarnings("ParameterName")
  public static double rungeKutta(
          BiFunction<Double, Double, Double> f,
          double x, Double u, double dtSeconds
  ) {
    final var halfDt = 0.5 * dtSeconds;
    final var k1 = f.apply(x, u);
    final var k2 = f.apply(x + k1 * halfDt, u);
    final var k3 = f.apply(x + k2 * halfDt, u);
    final var k4 = f.apply(x + k3 * dtSeconds, u);
    return x + dtSeconds / 6.0 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
  }

  /**
   * Performs 4th order Runge-Kutta integration of dx/dt = f(x, u) for dt.
   *
   * @param <S>  A Num representing the states of the system to integrate.
   * @param <I>  A Num representing the inputs of the system to integrate.
   * @param f         The function to integrate. It must take two arguments x and u.
   * @param x         The initial value of x.
   * @param u         The value u held constant over the integration period.
   * @param dtSeconds The time over which to integrate.
   * @return the integration of dx/dt = f(x, u) for dt.
   */
  @SuppressWarnings("ParameterName")
  public static <S extends Num, I extends Num> Matrix<S, N1> rungeKutta(
          BiFunction<Matrix<S, N1>, Matrix<I, N1>, Matrix<S, N1>> f,
          Matrix<S, N1> x, Matrix<I, N1> u, double dtSeconds) {

    final var halfDt = 0.5 * dtSeconds;
    Matrix<S, N1> k1 = f.apply(x, u);
    Matrix<S, N1> k2 = f.apply(x.plus(k1.times(halfDt)), u);
    Matrix<S, N1> k3 = f.apply(x.plus(k2.times(halfDt)), u);
    Matrix<S, N1> k4 = f.apply(x.plus(k3.times(dtSeconds)), u);
    return x.plus((k1.plus(k2.times(2.0)).plus(k3.times(2.0)).plus(k4)).times(dtSeconds).div(6.0));
  }

  /**
   * Performs 4th order Runge-Kutta integration of dx/dt = f(x) for dt.
   *
   * @param <S>  A Num prepresenting the states of the system.
   * @param f         The function to integrate. It must take one argument x.
   * @param x         The initial value of x.
   * @param dtSeconds The time over which to integrate.
   * @return 4th order Runge-Kutta integration of dx/dt = f(x) for dt.
   */
  @SuppressWarnings("ParameterName")
  public static <S extends Num> Matrix<S, N1> rungeKutta(
          Function<Matrix<S, N1>, Matrix<S, N1>> f,
          Matrix<S, N1> x, double dtSeconds) {

    final var halfDt = 0.5 * dtSeconds;
    Matrix<S, N1> k1 = f.apply(x);
    Matrix<S, N1> k2 = f.apply(x.plus(k1.times(halfDt)));
    Matrix<S, N1> k3 = f.apply(x.plus(k2.times(halfDt)));
    Matrix<S, N1> k4 = f.apply(x.plus(k3.times(dtSeconds)));
    return x.plus((k1.plus(k2.times(2.0)).plus(k3.times(2.0)).plus(k4)).times(dtSeconds).div(6.0));
  }

}
