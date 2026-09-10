// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.system;

import java.util.function.BiFunction;
import java.util.function.DoubleBinaryOperator;
import java.util.function.DoubleUnaryOperator;
import java.util.function.UnaryOperator;
import org.ejml.simple.SimpleMatrix;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.util.Num;

/** Numerical integration utilities. */
public final class NumericalIntegration {
  private NumericalIntegration() {
    // utility Class
  }

  /**
   * Performs 4th order Runge-Kutta integration of dx/dt = f(x) for dt.
   *
   * @param f The function to integrate. It must take one argument x.
   * @param x The initial value of x.
   * @param dt The time over which to integrate in seconds.
   * @return the integration of dx/dt = f(x) for dt.
   */
  public static double rk4(DoubleUnaryOperator f, double x, double dt) {
    final var h = dt;
    final var k1 = f.applyAsDouble(x);
    final var k2 = f.applyAsDouble(x + h * k1 * 0.5);
    final var k3 = f.applyAsDouble(x + h * k2 * 0.5);
    final var k4 = f.applyAsDouble(x + h * k3);

    return x + h / 6.0 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
  }

  /**
   * Performs 4th order Runge-Kutta integration of dx/dt = f(x, u) for dt.
   *
   * @param f The function to integrate. It must take two arguments x and u.
   * @param x The initial value of x.
   * @param u The value u held constant over the integration period.
   * @param dt The time over which to integrate in seconds.
   * @return the integration of dx/dt = f(x, u) for dt.
   */
  public static double rk4(DoubleBinaryOperator f, double x, double u, double dt) {
    final var h = dt;

    final var k1 = f.applyAsDouble(x, u);
    final var k2 = f.applyAsDouble(x + h * k1 * 0.5, u);
    final var k3 = f.applyAsDouble(x + h * k2 * 0.5, u);
    final var k4 = f.applyAsDouble(x + h * k3, u);

    return x + h / 6.0 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
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
  public static SimpleMatrix rk4(
      BiFunction<SimpleMatrix, SimpleMatrix, SimpleMatrix> f,
      SimpleMatrix x,
      SimpleMatrix u,
      double dt) {
    var h = dt;

    var k1 = f.apply(x, u);
    var k2 = f.apply(x.plus(k1.scale(h * 0.5)), u);
    var k3 = f.apply(x.plus(k2.scale(h * 0.5)), u);
    var k4 = f.apply(x.plus(k3.scale(h)), u);

    return x.plus(k1.plus(k2.scale(2.0)).plus(k3.scale(2.0)).plus(k4).scale(h / 6.0));
  }

  /**
   * Performs 4th order Runge-Kutta integration of dx/dt = f(x, u) for dt.
   *
   * @param <States> A Num representing the states of the system to integrate.
   * @param <Inputs> A Num representing the inputs of the system to integrate.
   * @param f The function to integrate. It must take two arguments x and u.
   * @param x The initial value of x.
   * @param u The value u held constant over the integration period.
   * @param dt The time over which to integrate in seconds.
   * @return the integration of dx/dt = f(x, u) for dt.
   */
  public static <States extends Num, Inputs extends Num> Matrix<States, N1> rk4(
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
      Matrix<States, N1> x,
      Matrix<Inputs, N1> u,
      double dt) {
    final var h = dt;

    Matrix<States, N1> k1 = f.apply(x, u);
    Matrix<States, N1> k2 = f.apply(x.plus(k1.times(h * 0.5)), u);
    Matrix<States, N1> k3 = f.apply(x.plus(k2.times(h * 0.5)), u);
    Matrix<States, N1> k4 = f.apply(x.plus(k3.times(h)), u);

    return x.plus((k1.plus(k2.times(2.0)).plus(k3.times(2.0)).plus(k4)).times(h / 6.0));
  }

  /**
   * Performs 4th order Runge-Kutta integration of dx/dt = f(x) for dt.
   *
   * @param <States> A Num representing the states of the system.
   * @param f The function to integrate. It must take one argument x.
   * @param x The initial value of x.
   * @param dt The time over which to integrate in seconds.
   * @return the integration of dx/dt = f(x) for dt.
   */
  public static <States extends Num> Matrix<States, N1> rk4(
      UnaryOperator<Matrix<States, N1>> f, Matrix<States, N1> x, double dt) {
    final var h = dt;

    Matrix<States, N1> k1 = f.apply(x);
    Matrix<States, N1> k2 = f.apply(x.plus(k1.times(h * 0.5)));
    Matrix<States, N1> k3 = f.apply(x.plus(k2.times(h * 0.5)));
    Matrix<States, N1> k4 = f.apply(x.plus(k3.times(h)));

    return x.plus((k1.plus(k2.times(2.0)).plus(k3.times(2.0)).plus(k4)).times(h / 6.0));
  }

  /**
   * Performs 4th order Runge-Kutta integration of dx/dt = f(t, y) for dt.
   *
   * @param <Rows> Rows in y.
   * @param <Cols> Columns in y.
   * @param f The function to integrate. It must take two arguments t and y.
   * @param t The initial value of t.
   * @param y The initial value of y.
   * @param dt The time over which to integrate in seconds.
   * @return the integration of dx/dt = f(x) for dt.
   */
  public static <Rows extends Num, Cols extends Num> Matrix<Rows, Cols> rk4(
      BiFunction<Double, Matrix<Rows, Cols>, Matrix<Rows, Cols>> f,
      double t,
      Matrix<Rows, Cols> y,
      double dt) {
    final var h = dt;

    Matrix<Rows, Cols> k1 = f.apply(t, y);
    Matrix<Rows, Cols> k2 = f.apply(t + dt * 0.5, y.plus(k1.times(h * 0.5)));
    Matrix<Rows, Cols> k3 = f.apply(t + dt * 0.5, y.plus(k2.times(h * 0.5)));
    Matrix<Rows, Cols> k4 = f.apply(t + dt, y.plus(k3.times(h)));

    return y.plus((k1.plus(k2.times(2.0)).plus(k3.times(2.0)).plus(k4)).times(h / 6.0));
  }

  /**
   * Performs adaptive Tsitouras 5th Order integration of dx/dt = f(x, u) for dt. By default, the
   * max error is 1e-6.
   *
   * @param <States> A Num representing the states of the system to integrate.
   * @param <Inputs> A Num representing the inputs of the system to integrate.
   * @param f The function to integrate. It must take two arguments x and u.
   * @param x The initial value of x.
   * @param u The value u held constant over the integration period.
   * @param dt The time over which to integrate in seconds.
   * @return the integration of dx/dt = f(x, u) for dt.
   */
  @SuppressWarnings("overloads")
  public static <States extends Num, Inputs extends Num> Matrix<States, N1> tsit5(
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
      Matrix<States, N1> x,
      Matrix<Inputs, N1> u,
      double dt) {
    return tsit5(f, x, u, dt, 1e-6);
  }

  /**
   * Performs adaptive Tsitouras 5th Order integration of dx/dt = f(x, u) for dt.
   *
   * @param <States> A Num representing the states of the system to integrate.
   * @param <Inputs> A Num representing the inputs of the system to integrate.
   * @param f The function to integrate. It must take two arguments x and u.
   * @param x The initial value of x.
   * @param u The value u held constant over the integration period.
   * @param dt The time over which to integrate in seconds.
   * @param maxError The maximum acceptable truncation error. Usually a small number like 1e-6.
   * @return the integration of dx/dt = f(x, u) for dt.
   */
  @SuppressWarnings("overloads")
  public static <States extends Num, Inputs extends Num> Matrix<States, N1> tsit5(
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
      Matrix<States, N1> x,
      Matrix<Inputs, N1> u,
      double dt,
      double maxError) {
    // See http://users.uoa.gr/~tsitourasc/RK54_new_v2.pdf Table 1 for the
    // Butcher tableau the following arrays came from.

    // final double[6][6]
    final double[][] A = {
      {0.161},
      {-0.008480655492357, 0.335480655492357},
      {2.897153057105494, -6.359448489975075, 4.362295432869582},
      {5.325864828439257, -11.748883564062828, 7.495539342889837, -0.092495066361755},
      {
        5.861455442946420,
        -12.920969317847110,
        8.159367898576159,
        -0.071584973281401,
        -0.028269050394068
      },
      // a₇ᵢ = bᵢ, i = 1, 2, ··· , 6
      {
        0.096460766818065,
        0.01,
        0.479889650414500,
        1.379008574103742,
        -3.290069515436081,
        2.324710524099774
      }
    };

    // final double[7]
    final double[] b1 = {
      0.09646076681806523,
      0.01,
      0.4798896504144996,
      1.379008574103742,
      -3.290069515436081,
      2.324710524099774,
      0.0
    };

    // final double[7]
    final double[] b2 = {
      0.001780011052226,
      0.000816434459657,
      -0.007880878010262,
      0.144711007173263,
      -0.582357165452555,
      0.458082105929187,
      1.0 / 66.0
    };

    // Loop until dt has elapsed
    double dtElapsed = 0.0;
    double h = dt;
    while (dtElapsed < dt) {
      var k1 = f.apply(x, u);
      var k2 = f.apply(x.plus(k1.times(A[0][0]).times(h)), u);
      var k3 = f.apply(x.plus(k1.times(A[1][0]).plus(k2.times(A[1][1])).times(h)), u);
      var k4 =
          f.apply(
              x.plus(k1.times(A[2][0]).plus(k2.times(A[2][1])).plus(k3.times(A[2][2])).times(h)),
              u);
      var k5 =
          f.apply(
              x.plus(
                  k1.times(A[3][0])
                      .plus(k2.times(A[3][1]))
                      .plus(k3.times(A[3][2]))
                      .plus(k4.times(A[3][3]))
                      .times(h)),
              u);
      var k6 =
          f.apply(
              x.plus(
                  k1.times(A[4][0])
                      .plus(k2.times(A[4][1]))
                      .plus(k3.times(A[4][2]))
                      .plus(k4.times(A[4][3]))
                      .plus(k5.times(A[4][4]))
                      .times(h)),
              u);

      // Since the final row of A and the array b1 have the same coefficients
      // and k7 has no effect on newX, we can reuse the calculation.
      var newX =
          x.plus(
              k1.times(A[5][0])
                  .plus(k2.times(A[5][1]))
                  .plus(k3.times(A[5][2]))
                  .plus(k4.times(A[5][3]))
                  .plus(k5.times(A[5][4]))
                  .plus(k6.times(A[5][5]))
                  .times(h));
      var k7 = f.apply(newX, u);

      double truncationError =
          (k1.times(b1[0] - b2[0])
                  .plus(k2.times(b1[1] - b2[1]))
                  .plus(k3.times(b1[2] - b2[2]))
                  .plus(k4.times(b1[3] - b2[3]))
                  .plus(k5.times(b1[4] - b2[4]))
                  .plus(k6.times(b1[5] - b2[5]))
                  .plus(k7.times(b1[6] - b2[6]))
                  .times(h))
              .normF();

      if (truncationError <= maxError) {
        // Accept the step
        x = newX;
        dtElapsed += h;
      }

      if (truncationError == 0.0) {
        h = dt - dtElapsed;
      } else {
        h = Math.min(0.9 * h * Math.pow(maxError / truncationError, 0.2), dt - dtElapsed);
      }
    }

    return x;
  }

  /**
   * Performs adaptive Tsitouras 5th Order integration of dx/dt = f(t, y) for dt. By default, the
   * max error is 1e-6.
   *
   * @param <Rows> Rows in y.
   * @param <Cols> Columns in y.
   * @param f The function to integrate. It must take two arguments t and y.
   * @param t The initial value of t.
   * @param y The initial value of y.
   * @param dt The time over which to integrate in seconds.
   * @return the integration of dx/dt = f(x, u) for dt.
   */
  @SuppressWarnings("overloads")
  public static <Rows extends Num, Cols extends Num> Matrix<Rows, Cols> tsit5(
      BiFunction<Double, Matrix<Rows, Cols>, Matrix<Rows, Cols>> f,
      double t,
      Matrix<Rows, Cols> y,
      double dt) {
    return tsit5(f, t, y, dt, 1e-6);
  }

  /**
   * Performs adaptive Tsitouras 5th Order integration of dx/dt = f(t, y) for dt.
   *
   * @param <Rows> Rows in y.
   * @param <Cols> Columns in y.
   * @param f The function to integrate. It must take two arguments t and y.
   * @param t The initial value of t.
   * @param y The initial value of y.
   * @param dt The time over which to integrate in seconds.
   * @param maxError The maximum acceptable truncation error. Usually a small number like 1e-6.
   * @return the integration of dx/dt = f(x, u) for dt.
   */
  @SuppressWarnings("overloads")
  public static <Rows extends Num, Cols extends Num> Matrix<Rows, Cols> tsit5(
      BiFunction<Double, Matrix<Rows, Cols>, Matrix<Rows, Cols>> f,
      double t,
      Matrix<Rows, Cols> y,
      double dt,
      double maxError) {
    // See http://users.uoa.gr/~tsitourasc/RK54_new_v2.pdf Table 1 for the
    // Butcher tableau the following arrays came from.

    // final double[6][6]
    final double[][] A = {
      {0.161},
      {-0.008480655492357, 0.335480655492357},
      {2.897153057105494, -6.359448489975075, 4.362295432869582},
      {5.325864828439257, -11.748883564062828, 7.495539342889837, -0.092495066361755},
      {
        5.861455442946420,
        -12.920969317847110,
        8.159367898576159,
        -0.071584973281401,
        -0.028269050394068
      },
      // a₇ᵢ = bᵢ, i = 1, 2, ··· , 6
      {
        0.096460766818065,
        0.01,
        0.479889650414500,
        1.379008574103742,
        -3.290069515436081,
        2.324710524099774
      }
    };

    // final double[7]
    final double[] b1 = {
      0.09646076681806523,
      0.01,
      0.4798896504144996,
      1.379008574103742,
      -3.290069515436081,
      2.324710524099774,
      0.0
    };

    // final double[7]
    final double[] b2 = {
      0.001780011052226,
      0.000816434459657,
      -0.007880878010262,
      0.144711007173263,
      -0.582357165452555,
      0.458082105929187,
      1.0 / 66.0
    };

    // final double[6]
    final double[] c = {0.161, 0.327, 0.9, 0.9800255409045097, 1.0, 1.0};

    // Loop until dt has elapsed
    double dtElapsed = 0.0;
    double h = dt;
    while (dtElapsed < dt) {
      var k1 = f.apply(t, y);
      var k2 = f.apply(t + h * c[0], y.plus(k1.times(A[0][0]).times(h)));
      var k3 = f.apply(t + h * c[1], y.plus(k1.times(A[1][0]).plus(k2.times(A[1][1])).times(h)));
      var k4 =
          f.apply(
              t + h * c[2],
              y.plus(k1.times(A[2][0]).plus(k2.times(A[2][1])).plus(k3.times(A[2][2])).times(h)));
      var k5 =
          f.apply(
              t + h * c[3],
              y.plus(
                  k1.times(A[3][0])
                      .plus(k2.times(A[3][1]))
                      .plus(k3.times(A[3][2]))
                      .plus(k4.times(A[3][3]))
                      .times(h)));
      var k6 =
          f.apply(
              t + h * c[4],
              y.plus(
                  k1.times(A[4][0])
                      .plus(k2.times(A[4][1]))
                      .plus(k3.times(A[4][2]))
                      .plus(k4.times(A[4][3]))
                      .plus(k5.times(A[4][4]))
                      .times(h)));

      // Since the final row of A and the array b1 have the same coefficients
      // and k7 has no effect on newY, we can reuse the calculation.
      var newY =
          y.plus(
              k1.times(A[5][0])
                  .plus(k2.times(A[5][1]))
                  .plus(k3.times(A[5][2]))
                  .plus(k4.times(A[5][3]))
                  .plus(k5.times(A[5][4]))
                  .plus(k6.times(A[5][5]))
                  .times(h));
      var k7 = f.apply(t + h * c[5], newY);

      double truncationError =
          (k1.times(b1[0] - b2[0])
                  .plus(k2.times(b1[1] - b2[1]))
                  .plus(k3.times(b1[2] - b2[2]))
                  .plus(k4.times(b1[3] - b2[3]))
                  .plus(k5.times(b1[4] - b2[4]))
                  .plus(k6.times(b1[5] - b2[5]))
                  .plus(k7.times(b1[6] - b2[6]))
                  .times(h))
              .normF();

      if (truncationError <= maxError) {
        // Accept the step
        y = newY;
        dtElapsed += h;
      }

      if (truncationError == 0.0) {
        h = dt - dtElapsed;
      } else {
        h = Math.min(0.9 * h * Math.pow(maxError / truncationError, 0.2), dt - dtElapsed);
      }
    }

    return y;
  }
}
