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
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.Pair;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N5;
import edu.wpi.first.wpiutil.math.numbers.N6;

public final class NumericalIntegration {
    private NumericalIntegration() {
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
     * @param <States>  A Num representing the states of the system to integrate.
     * @param <Inputs>  A Num representing the inputs of the system to integrate.
     * @param f         The function to integrate. It must take two arguments x and u.
     * @param x         The initial value of x.
     * @param u         The value u held constant over the integration period.
     * @param dtSeconds The time over which to integrate.
     * @return the integration of dx/dt = f(x, u) for dt.
     */
    @SuppressWarnings({"ParameterName", "MethodTypeParameterName"})
    public static <States extends Num, Inputs extends Num> Matrix<States, N1> rungeKutta(
        BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
        Matrix<States, N1> x, Matrix<Inputs, N1> u, double dtSeconds) {

        final var halfDt = 0.5 * dtSeconds;
        Matrix<States, N1> k1 = f.apply(x, u);
        Matrix<States, N1> k2 = f.apply(x.plus(k1.times(halfDt)), u);
        Matrix<States, N1> k3 = f.apply(x.plus(k2.times(halfDt)), u);
        Matrix<States, N1> k4 = f.apply(x.plus(k3.times(dtSeconds)), u);
        return x.plus((k1.plus(k2.times(2.0)).plus(k3.times(2.0)).plus(k4)).times(dtSeconds).div(6.0));
    }

    /**
     * Performs 4th order Runge-Kutta integration of dx/dt = f(x) for dt.
     *
     * @param <States>  A Num prepresenting the states of the system.
     * @param f         The function to integrate. It must take one argument x.
     * @param x         The initial value of x.
     * @param dtSeconds The time over which to integrate.
     * @return 4th order Runge-Kutta integration of dx/dt = f(x) for dt.
     */
    @SuppressWarnings({"ParameterName", "MethodTypeParameterName"})
    public static <States extends Num> Matrix<States, N1> rungeKutta(
        Function<Matrix<States, N1>, Matrix<States, N1>> f,
        Matrix<States, N1> x, double dtSeconds) {

        final var halfDt = 0.5 * dtSeconds;
        Matrix<States, N1> k1 = f.apply(x);
        Matrix<States, N1> k2 = f.apply(x.plus(k1.times(halfDt)));
        Matrix<States, N1> k3 = f.apply(x.plus(k2.times(halfDt)));
        Matrix<States, N1> k4 = f.apply(x.plus(k3.times(dtSeconds)));
        return x.plus((k1.plus(k2.times(2.0)).plus(k3.times(2.0)).plus(k4)).times(dtSeconds).div(6.0));
    }

    /**
     * Performs adaptive RKF45 integration of dx/dt = f(x, u) for dt.
     *
     * @param <States>  A Num representing the states of the system to integrate.
     * @param <Inputs>  A Num representing the inputs of the system to integrate.
     * @param f         The function to integrate. It must take two arguments x and u.
     * @param x         The initial value of x.
     * @param u         The value u held constant over the integration period.
     * @param dtSeconds The time over which to integrate.
     * @param maxError  The maximum acceptable truncation error. Usually a small number like 1e-6.
     * @return the integration of dx/dt = f(x, u) for dt.
     */
    public static <States extends Num, Inputs extends Num> Matrix<States, N1> rungeKuttaAdaptive(
        BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
        Matrix<States, N1> x, Matrix<Inputs, N1> u, double dtSeconds, double maxError) {

        double dtElapsed = 0;
        double previousH = dtSeconds;
        // Loop until we've gotten to our desired dt
        while (dtElapsed < dtSeconds) {
            // RKF45 will give us an updated x and a dt back.
            // We use the new dt (h) as the initial dt for our next loop
            var ret = rungeKuttaAdaptiveImpl(f, x, u, previousH, maxError, dtSeconds - dtElapsed);
            dtElapsed += ret.getSecond();
            previousH = ret.getSecond();
            x = ret.getFirst();
        }
        return x;
    }

    static final double[] ch = {47 / 450., 0, 12 / 25., 32 / 225., 1 / 30., 6 / 25.};
    static final double[] ct = {-1 / 150., 0, 3 / 100., -16 / 75., -1 / 20., 6 / 25.};
    static final Matrix<N6, N5> Bs = Matrix.mat(Nat.N6(), Nat.N5())
        .fill(0, 0, 0, 0, 0,
            2 / 9., 0, 0, 0, 0,
            1 / 12., 1 / 4., 0, 0, 0,
            69 / 128., -243 / 128., 135 / 64., 0, 0,
            -17 / 12., 27 / 4., -27 / 5., 16 / 15., 0,
            65 / 432., -5 / 16., 13 / 16., 4 / 27., 5 / 144.);

    /**
     * Implements one loop of RKF45. This takes an initial state, dt guess, and max truncation error,
     * and returns a new x and the dt over which that x was updated. This should be called until there is no
     * dt remaining.
     *
     * @param <States>           Num representing the states of the system to integrate.
     * @param <Inputs>           A Num representing the inputs of the system to integrate.
     * @param f                  The function to integrate. It must take two arguments x and u.
     * @param x                  The initial value of x.
     * @param u                  The value u held constant over the integration period.
     * @param initialH           The initial dt guess. This is refined to clamp truncation error to the specified max.
     * @param maxTruncationError The max truncation error acceptable. Usually a small number like 1e-6.
     * @param dtRemaining        How much time is left to integrate over. Used to clamp h.
     * @return the integration of dx/dt = f(x, u) for dt.
     */
    private static <States extends Num, Inputs extends Num> Pair<Matrix<States, N1>, Double> rungeKuttaAdaptiveImpl(
        BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
        Matrix<States, N1> x, Matrix<Inputs, N1> u, double initialH, double maxTruncationError, double dtRemaining) {

        double truncationErr;
        double h = initialH;
        Matrix<States, N1> newX;

        do {
            // only allow us to advance up to the dt remaining
            h = Math.min(h, dtRemaining);

            // Notice how the derivative in the Wikipedia notation is dy/dx.
            // That means their y is our x and their x is our t
            Matrix<States, N1> k1 = f.apply(x, u).times(h);
            Matrix<States, N1> k2 = f.apply(
                x.plus(k1.times(Bs.get(1, 0))),
                u).times(h);
            Matrix<States, N1> k3 = f.apply(
                x.plus(k1.times(Bs.get(2, 0)))
                    .plus(k2.times(Bs.get(2, 1))),
                u).times(h);
            Matrix<States, N1> k4 = f.apply(
                x.plus(k1.times(Bs.get(3, 0)))
                    .plus(k2.times(Bs.get(3, 1)))
                    .plus(k3.times(Bs.get(3, 2))),
                u).times(h);
            Matrix<States, N1> k5 = f.apply(
                x.plus(k1.times(Bs.get(4, 0)))
                    .plus(k2.times(Bs.get(4, 1)))
                    .plus(k3.times(Bs.get(4, 2)))
                    .plus(k4.times(Bs.get(4, 3))),
                u).times(h);
            Matrix<States, N1> k6 = f.apply(
                x.plus(k1.times(Bs.get(5, 0)))
                    .plus(k2.times(Bs.get(5, 1)))
                    .plus(k3.times(Bs.get(5, 2)))
                    .plus(k4.times(Bs.get(5, 3)))
                    .plus(k5.times(Bs.get(5, 4))),
                u).times(h);

            newX = x.plus(k1.times(ch[0]))
                .plus(k2.times(ch[1]))
                .plus(k3.times(ch[2]))
                .plus(k4.times(ch[3]))
                .plus(k5.times(ch[4]))
                .plus(k6.times(ch[5]));

            truncationErr = k1.times(ct[0])
                .plus(k2.times(ct[1]))
                .plus(k3.times(ct[2]))
                .plus(k4.times(ct[3]))
                .plus(k5.times(ct[4]))
                .plus(k6.times(ct[5]))
                .normF();

            var hNew = 0.9 * h * Math.pow(maxTruncationError / truncationErr, 1 / 5.);
            h = hNew;
        } while (truncationErr > maxTruncationError);

        // Return the new x, and the timestep
        return Pair.of(newX, h);
    }
}
