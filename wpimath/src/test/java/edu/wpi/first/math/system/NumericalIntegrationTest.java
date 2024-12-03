// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import org.junit.jupiter.api.Test;

class NumericalIntegrationTest {
  @Test
  void testExponential() {
    Matrix<N1, N1> y0 = VecBuilder.fill(0.0);

    var y1 =
        NumericalIntegration.rk4(
            (Matrix<N1, N1> x) -> {
              var y = new Matrix<>(Nat.N1(), Nat.N1());
              y.set(0, 0, Math.exp(x.get(0, 0)));
              return y;
            },
            y0,
            0.1);

    assertEquals(Math.exp(0.1) - Math.exp(0.0), y1.get(0, 0), 1e-3);
  }

  // Tests RK4 with a time varying solution. From
  // http://www2.hawaii.edu/~jmcfatri/math407/RungeKuttaTest.html:
  //   x' = x (2/(eᵗ + 1) - 1)
  //
  // The true (analytical) solution is:
  //
  // x(t) = 12eᵗ/(eᵗ + 1)²
  @Test
  void testRK4TimeVarying() {
    final var y0 = VecBuilder.fill(12.0 * Math.exp(5.0) / Math.pow(Math.exp(5.0) + 1.0, 2.0));

    final var y1 =
        NumericalIntegration.rk4(
            (Double t, Matrix<N1, N1> y) ->
                MatBuilder.fill(
                    Nat.N1(), Nat.N1(), y.get(0, 0) * (2.0 / (Math.exp(t) + 1.0) - 1.0)),
            5.0,
            y0,
            1.0);
    assertEquals(12.0 * Math.exp(6.0) / Math.pow(Math.exp(6.0) + 1.0, 2.0), y1.get(0, 0), 1e-3);
  }

  @Test
  void testZeroRKDP() {
    var y1 =
        NumericalIntegration.rkdp(
            (x, u) -> VecBuilder.fill(0), VecBuilder.fill(0), VecBuilder.fill(0), 0.1);

    assertEquals(0.0, y1.get(0, 0), 1e-3);
  }

  @Test
  void testExponentialRKDP() {
    Matrix<N1, N1> y0 = VecBuilder.fill(0.0);

    var y1 =
        NumericalIntegration.rkdp(
            (x, u) -> {
              var y = new Matrix<>(Nat.N1(), Nat.N1());
              y.set(0, 0, Math.exp(x.get(0, 0)));
              return y;
            },
            y0,
            VecBuilder.fill(0),
            0.1);

    assertEquals(Math.exp(0.1) - Math.exp(0.0), y1.get(0, 0), 1e-3);
  }

  // Tests RKDP with a time varying solution. From
  // http://www2.hawaii.edu/~jmcfatri/math407/RungeKuttaTest.html:
  //
  //   dx/dt = x(2/(eᵗ + 1) - 1)
  //
  // The true (analytical) solution is:
  //
  //   x(t) = 12eᵗ/(eᵗ + 1)²
  @Test
  void testRKDPTimeVarying() {
    final var y0 = VecBuilder.fill(12.0 * Math.exp(5.0) / Math.pow(Math.exp(5.0) + 1.0, 2.0));

    final var y1 =
        NumericalIntegration.rkdp(
            (Double t, Matrix<N1, N1> y) ->
                MatBuilder.fill(
                    Nat.N1(), Nat.N1(), y.get(0, 0) * (2.0 / (Math.exp(t) + 1.0) - 1.0)),
            5.0,
            y0,
            1.0,
            1e-12);
    assertEquals(12.0 * Math.exp(6.0) / Math.pow(Math.exp(6.0) + 1.0, 2.0), y1.get(0, 0), 1e-3);
  }
}
