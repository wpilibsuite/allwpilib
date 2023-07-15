// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.numbers.N1;
import org.junit.jupiter.api.Test;

class RungeKuttaTimeVaryingTest {
  private static Matrix<N1, N1> rungeKuttaTimeVaryingSolution(double t) {
    return new MatBuilder<>(Nat.N1(), Nat.N1())
        .fill(12.0 * Math.exp(t) / Math.pow(Math.exp(t) + 1.0, 2.0));
  }

  // Tests RK4 with a time varying solution. From
  // http://www2.hawaii.edu/~jmcfatri/math407/RungeKuttaTest.html:
  //   x' = x (2/(eᵗ + 1) - 1)
  //
  // The true (analytical) solution is:
  //
  // x(t) = 12eᵗ/(eᵗ + 1)²
  @Test
  void rungeKuttaTimeVaryingTest() {
    final var y0 = rungeKuttaTimeVaryingSolution(5.0);

    final var y1 =
        RungeKuttaTimeVarying.rungeKuttaTimeVarying(
            (Double t, Matrix<N1, N1> x) -> {
              return new MatBuilder<>(Nat.N1(), Nat.N1())
                  .fill(x.get(0, 0) * (2.0 / (Math.exp(t) + 1.0) - 1.0));
            },
            5.0,
            y0,
            1.0);
    assertEquals(rungeKuttaTimeVaryingSolution(6.0).get(0, 0), y1.get(0, 0), 1e-3);
  }
}
