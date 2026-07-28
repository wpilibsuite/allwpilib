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
   * @param dtSeconds The time over which to integrate.
   * @return the integration of dx/dt = f(x, u) for dt.
   */
  @SuppressWarnings("overloads")
  public static <States extends Num, Inputs extends Num> Matrix<States, N1> tsit5(
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
      Matrix<States, N1> x,
      Matrix<Inputs, N1> u,
      double dtSeconds) {
    return tsit5(f, x, u, dtSeconds, 1e-6);
  }

  /**
   * Performs adaptive Tsitouras 5th Order integration of dx/dt = f(x, u) for dt.
   *
   * @param <States> A Num representing the states of the system to integrate.
   * @param <Inputs> A Num representing the inputs of the system to integrate.
   * @param f The function to integrate. It must take two arguments x and u.
   * @param x The initial value of x.
   * @param u The value u held constant over the integration period.
   * @param dtSeconds The time over which to integrate.
   * @param maxError The maximum acceptable truncation error. Usually a small number like 1e-6.
   * @return the integration of dx/dt = f(x, u) for dt.
   */
  @SuppressWarnings("overloads")
  public static <States extends Num, Inputs extends Num> Matrix<States, N1> tsit5(
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
      Matrix<States, N1> x,
      Matrix<Inputs, N1> u,
      double dtSeconds,
      double maxError) {
    // See http://users.uoa.gr/~tsitourasc/RK54_new_v2.pdf Table 1 for the
    // Butcher tableau the following arrays came from.

    // final double[6][6]
    final double[][] A = {
      {0.161},
      {-0.008480655492356989, 0.335480655492357},
      {2.8971530571054935, -6.359448489975075, 4.3622954328695815},
      {5.325864828439257, -11.748883564062828, 7.4955393428898365, -0.09249506636175525},
      {
        5.86145544294642,
        -12.92096931784711,
        8.159367898576159,
        -0.071584973281401,
        -0.028269050394068383
      },
      { // a₇ᵢ = bᵢ, i = 1, 2, ··· , 6
        0.09646076681806523,
        0.01,
        0.4798896504144996,
        1.379008574103742,
        -3.290069515436081,
        2.324710524099774
      }
    };

    // final double[7]
    // final double[] b1 = {
    //   0.09646076681806523,
    //   0.01,
    //   0.4798896504144996,
    //   1.379008574103742,
    //   -3.290069515436081,
    //   2.324710524099774,
    //   0.0
    // };

    // final double[7]
    final double[] b2 = {
      -0.00178001105222577714,
      -0.0008164344596567469,
      0.007880878010261995,
      -0.1447110071732629,
      0.5823571654525552,
      -0.45808210592918697,
      1.0 / 66.0
    };

    Matrix<States, N1> newX;
    double truncationError;

    double dtElapsed = 0.0;
    double h = dtSeconds;

    // Loop until we've gotten to our desired dt
    while (dtElapsed < dtSeconds) {
      do {
        // Only allow us to advance up to the dt remaining
        h = Math.min(h, dtSeconds - dtElapsed);

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
        newX =
            x.plus(
                k1.times(A[5][0])
                    .plus(k2.times(A[5][1]))
                    .plus(k3.times(A[5][2]))
                    .plus(k4.times(A[5][3]))
                    .plus(k5.times(A[5][4]))
                    .plus(k6.times(A[5][5]))
                    .times(h));
        var k7 = f.apply(newX, u);

        truncationError =
            (k1.times(b2[0])
                    .plus(k2.times(b2[1]))
                    .plus(k3.times(b2[2]))
                    .plus(k4.times(b2[3]))
                    .plus(k5.times(b2[4]))
                    .plus(k6.times(b2[5]))
                    .plus(k7.times(b2[6]))
                    .times(h))
                .normF();

        if (truncationError == 0.0) {
          h = dtSeconds - dtElapsed;
        } else {
          h *= 0.9 * Math.pow(maxError / truncationError, 1.0 / 5.0);
        }
      } while (truncationError > maxError);

      dtElapsed += h;
      x = newX;
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
   * @param dtSeconds The time over which to integrate.
   * @return the integration of dx/dt = f(x, u) for dt.
   */
  @SuppressWarnings("overloads")
  public static <Rows extends Num, Cols extends Num> Matrix<Rows, Cols> tsit5(
      BiFunction<Double, Matrix<Rows, Cols>, Matrix<Rows, Cols>> f,
      double t,
      Matrix<Rows, Cols> y,
      double dtSeconds) {
    return tsit5(f, t, y, dtSeconds, 1e-6);
  }

  /**
   * Performs adaptive Tsitouras 5th Order integration of dx/dt = f(t, y) for dt.
   *
   * @param <Rows> Rows in y.
   * @param <Cols> Columns in y.
   * @param f The function to integrate. It must take two arguments t and y.
   * @param t The initial value of t.
   * @param y The initial value of y.
   * @param dtSeconds The time over which to integrate.
   * @param maxError The maximum acceptable truncation error. Usually a small number like 1e-6.
   * @return the integration of dx/dt = f(x, u) for dt.
   */
  @SuppressWarnings("overloads")
  public static <Rows extends Num, Cols extends Num> Matrix<Rows, Cols> tsit5(
      BiFunction<Double, Matrix<Rows, Cols>, Matrix<Rows, Cols>> f,
      double t,
      Matrix<Rows, Cols> y,
      double dtSeconds,
      double maxError) {
    // See http://users.uoa.gr/~tsitourasc/RK54_new_v2.pdf Table 1 for the
    // Butcher tableau the following arrays came from.

    // final double[6][6]
    final double[][] A = {
      {0.161},
      {-0.008480655492356989, 0.335480655492357},
      {2.8971530571054935, -6.359448489975075, 4.3622954328695815},
      {5.325864828439257, -11.748883564062828, 7.4955393428898365, -0.09249506636175525},
      {
        5.86145544294642,
        -12.92096931784711,
        8.159367898576159,
        -0.071584973281401,
        -0.028269050394068383
      },
      { // a₇ᵢ = bᵢ, i = 1, 2, ··· , 6
        0.09646076681806523,
        0.01,
        0.4798896504144996,
        1.379008574103742,
        -3.290069515436081,
        2.324710524099774
      }
    };

    // final double[7]
    // final double[] b1 = {
    //   0.09646076681806523,
    //   0.01,
    //   0.4798896504144996,
    //   1.379008574103742,
    //   -3.290069515436081,
    //   2.324710524099774,
    //   0.0
    // };

    // final double[7]
    final double[] b2 = {
      -0.00178001105222577714,
      -0.0008164344596567469,
      0.007880878010261995,
      -0.1447110071732629,
      0.5823571654525552,
      -0.45808210592918697,
      1.0 / 66.0
    };

    // final double[6]
    final double[] c = {0.161, 0.327, 0.9, 0.9800255409045097, 1.0, 1.0};

    Matrix<Rows, Cols> newY;
    double truncationError;

    double dtElapsed = 0.0;
    double h = dtSeconds;

    // Loop until we've gotten to our desired dt
    while (dtElapsed < dtSeconds) {
      do {
        // Only allow us to advance up to the dt remaining
        h = Math.min(h, dtSeconds - dtElapsed);

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
        newY =
            y.plus(
                k1.times(A[5][0])
                    .plus(k2.times(A[5][1]))
                    .plus(k3.times(A[5][2]))
                    .plus(k4.times(A[5][3]))
                    .plus(k5.times(A[5][4]))
                    .plus(k6.times(A[5][5]))
                    .times(h));
        var k7 = f.apply(t + h * c[5], newY);

        truncationError =
            (k1.times(b2[0])
                    .plus(k2.times(b2[1]))
                    .plus(k3.times(b2[2]))
                    .plus(k4.times(b2[3]))
                    .plus(k5.times(b2[4]))
                    .plus(k6.times(b2[5]))
                    .plus(k7.times(b2[6]))
                    .times(h))
                .normF();

        if (truncationError == 0.0) {
          h = dtSeconds - dtElapsed;
        } else {
          h *= 0.9 * Math.pow(maxError / truncationError, 1.0 / 5.0);
        }

      } while (truncationError > maxError);

      dtElapsed += h;
      y = newY;
    }

    return y;
  }
}
