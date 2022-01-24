// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system;

import static org.junit.jupiter.api.Assertions.assertEquals;

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

  @Test
  void testExponentialRKF45() {
    Matrix<N1, N1> y0 = VecBuilder.fill(0.0);

    var y1 =
        NumericalIntegration.rkf45(
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
}
