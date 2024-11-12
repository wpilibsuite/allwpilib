// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.numbers.N1;
import java.util.function.BiFunction;
import java.util.function.DoubleBinaryOperator;
import java.util.function.DoubleUnaryOperator;
import java.util.function.UnaryOperator;

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
   * @param dtSeconds The time over which to integrate.
   * @return the integration of dx/dt = f(x) for dt.
   */
  public static double rk4(DoubleUnaryOperator f, double x, double dtSeconds) {
    final var h = dtSeconds;
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
   * @param dtSeconds The time over which to integrate.
   * @return the integration of dx/dt = f(x, u) for dt.
   */
  public static double rk4(DoubleBinaryOperator f, double x, double u, double dtSeconds) {
    final var h = dtSeconds;

    final var k1 = f.applyAsDouble(x, u);
    final var k2 = f.applyAsDouble(x + h * k1 * 0.5, u);
    final var k3 = f.applyAsDouble(x + h * k2 * 0.5, u);
    final var k4 = f.applyAsDouble(x + h * k3, u);

    return x + h / 6.0 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
  }

  /**
   * Performs 4th order Runge-Kutta integration of dx/dt = f(x, u) for dt.
   *
   * @param <States> A Num representing the states of the system to integrate.
   * @param <Inputs> A Num representing the inputs of the system to integrate.
   * @param f The function to integrate. It must take two arguments x and u.
   * @param x The initial value of x.
   * @param u The value u held constant over the integration period.
   * @param dtSeconds The time over which to integrate.
   * @return the integration of dx/dt = f(x, u) for dt.
   */
  public static <States extends Num, Inputs extends Num> Matrix<States, N1> rk4(
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
      Matrix<States, N1> x,
      Matrix<Inputs, N1> u,
      double dtSeconds) {
    final var h = dtSeconds;

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
   * @param dtSeconds The time over which to integrate.
   * @return the integration of dx/dt = f(x) for dt.
   */
  public static <States extends Num> Matrix<States, N1> rk4(
      UnaryOperator<Matrix<States, N1>> f, Matrix<States, N1> x, double dtSeconds) {
    final var h = dtSeconds;

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
   * @param dtSeconds The time over which to integrate.
   * @return the integration of dx/dt = f(x) for dt.
   */
  public static <Rows extends Num, Cols extends Num> Matrix<Rows, Cols> rk4(
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

  /**
   * Performs adaptive Dormand-Prince integration of dx/dt = f(x, u) for dt. By default, the max
   * error is 1e-6.
   *
   * @param <States> A Num representing the states of the system to integrate.
   * @param <Inputs> A Num representing the inputs of the system to integrate.
   * @param f The function to integrate. It must take two arguments x and u.
   * @param x The initial value of x.
   * @param u The value u held constant over the integration period.
   * @param dtSeconds The time over which to integrate.
   * @return the integration of dx/dt = f(x, u) for dt.
   * @deprecated Use tsit5() instead.
   */
  @SuppressWarnings("overloads")
  @Deprecated(forRemoval = true, since = "2026")
  public static <States extends Num, Inputs extends Num> Matrix<States, N1> rkdp(
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
      Matrix<States, N1> x,
      Matrix<Inputs, N1> u,
      double dtSeconds) {
    return rkdp(f, x, u, dtSeconds, 1e-6);
  }

  /**
   * Performs adaptive Dormand-Prince integration of dx/dt = f(x, u) for dt.
   *
   * @param <States> A Num representing the states of the system to integrate.
   * @param <Inputs> A Num representing the inputs of the system to integrate.
   * @param f The function to integrate. It must take two arguments x and u.
   * @param x The initial value of x.
   * @param u The value u held constant over the integration period.
   * @param dtSeconds The time over which to integrate.
   * @param maxError The maximum acceptable truncation error. Usually a small number like 1e-6.
   * @return the integration of dx/dt = f(x, u) for dt.
   * @deprecated use tsit5() instead.
   */
  @SuppressWarnings("overloads")
  @Deprecated(forRemoval = true, since = "2026")
  public static <States extends Num, Inputs extends Num> Matrix<States, N1> rkdp(
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
      Matrix<States, N1> x,
      Matrix<Inputs, N1> u,
      double dtSeconds,
      double maxError) {
    // See https://en.wikipedia.org/wiki/Dormand%E2%80%93Prince_method for the
    // Butcher tableau the following arrays came from.

    // final double[6][6]
    final double[][] A = {
      {1.0 / 5.0},
      {3.0 / 40.0, 9.0 / 40.0},
      {44.0 / 45.0, -56.0 / 15.0, 32.0 / 9.0},
      {19372.0 / 6561.0, -25360.0 / 2187.0, 64448.0 / 6561.0, -212.0 / 729.0},
      {9017.0 / 3168.0, -355.0 / 33.0, 46732.0 / 5247.0, 49.0 / 176.0, -5103.0 / 18656.0},
      {35.0 / 384.0, 0.0, 500.0 / 1113.0, 125.0 / 192.0, -2187.0 / 6784.0, 11.0 / 84.0}
    };

    // final double[7]
    final double[] b1 = {
      35.0 / 384.0, 0.0, 500.0 / 1113.0, 125.0 / 192.0, -2187.0 / 6784.0, 11.0 / 84.0, 0.0
    };

    // final double[7]
    final double[] b2 = {
      5179.0 / 57600.0,
      0.0,
      7571.0 / 16695.0,
      393.0 / 640.0,
      -92097.0 / 339200.0,
      187.0 / 2100.0,
      1.0 / 40.0
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
            (k1.times(b1[0] - b2[0])
                    .plus(k2.times(b1[1] - b2[1]))
                    .plus(k3.times(b1[2] - b2[2]))
                    .plus(k4.times(b1[3] - b2[3]))
                    .plus(k5.times(b1[4] - b2[4]))
                    .plus(k6.times(b1[5] - b2[5]))
                    .plus(k7.times(b1[6] - b2[6]))
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
   * Performs adaptive Dormand-Prince integration of dx/dt = f(t, y) for dt.
   *
   * @param <Rows> Rows in y.
   * @param <Cols> Columns in y.
   * @param f The function to integrate. It must take two arguments t and y.
   * @param t The initial value of t.
   * @param y The initial value of y.
   * @param dtSeconds The time over which to integrate.
   * @param maxError The maximum acceptable truncation error. Usually a small number like 1e-6.
   * @return the integration of dx/dt = f(x, u) for dt.
   * @deprecated Use tsit5() instead
   */
  @SuppressWarnings("overloads")
  @Deprecated(forRemoval = true, since = "2026")
  public static <Rows extends Num, Cols extends Num> Matrix<Rows, Cols> rkdp(
      BiFunction<Double, Matrix<Rows, Cols>, Matrix<Rows, Cols>> f,
      double t,
      Matrix<Rows, Cols> y,
      double dtSeconds,
      double maxError) {
    // See https://en.wikipedia.org/wiki/Dormand%E2%80%93Prince_method for the
    // Butcher tableau the following arrays came from.

    // final double[6][6]
    final double[][] A = {
      {1.0 / 5.0},
      {3.0 / 40.0, 9.0 / 40.0},
      {44.0 / 45.0, -56.0 / 15.0, 32.0 / 9.0},
      {19372.0 / 6561.0, -25360.0 / 2187.0, 64448.0 / 6561.0, -212.0 / 729.0},
      {9017.0 / 3168.0, -355.0 / 33.0, 46732.0 / 5247.0, 49.0 / 176.0, -5103.0 / 18656.0},
      {35.0 / 384.0, 0.0, 500.0 / 1113.0, 125.0 / 192.0, -2187.0 / 6784.0, 11.0 / 84.0}
    };

    // final double[7]
    final double[] b1 = {
      35.0 / 384.0, 0.0, 500.0 / 1113.0, 125.0 / 192.0, -2187.0 / 6784.0, 11.0 / 84.0, 0.0
    };

    // final double[7]
    final double[] b2 = {
      5179.0 / 57600.0,
      0.0,
      7571.0 / 16695.0,
      393.0 / 640.0,
      -92097.0 / 339200.0,
      187.0 / 2100.0,
      1.0 / 40.0
    };

    // final double[6]
    final double[] c = {1.0 / 5.0, 3.0 / 10.0, 4.0 / 5.0, 8.0 / 9.0, 1.0, 1.0};

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
            (k1.times(b1[0] - b2[0])
                    .plus(k2.times(b1[1] - b2[1]))
                    .plus(k3.times(b1[2] - b2[2]))
                    .plus(k4.times(b1[3] - b2[3]))
                    .plus(k5.times(b1[4] - b2[4]))
                    .plus(k6.times(b1[5] - b2[5]))
                    .plus(k7.times(b1[6] - b2[6]))
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

  /**
   * Performs adaptive Dormand-Prince integration of dx/dt = f(x, u) for dt. By default, the max
   * error is 1e-6.
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
