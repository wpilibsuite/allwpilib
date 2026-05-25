// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.autodiff.Variable;

class CurrentManagerTest {
  @Test
  void testEnoughCurrent() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var manager = new CurrentManager(new double[] {1.0, 5.0, 10.0, 5.0}, 40.0)) {
      var currents = manager.calculate(new double[] {25.0, 10.0, 5.0, 0.0});

      assertEquals(25.0, currents[0], 1e-3);
      assertEquals(10.0, currents[1], 1e-3);
      assertEquals(5.0, currents[2], 1e-3);
      assertEquals(0.0, currents[3], 1e-3);
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testNotEnoughCurrent() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var manager = new CurrentManager(new double[] {1.0, 5.0, 10.0, 5.0}, 40.0)) {
      var currents = manager.calculate(new double[] {30.0, 10.0, 5.0, 0.0});

      // Expected values are from the following program:
      //
      // #!/usr/bin/env python3
      //
      // from scipy.optimize import minimize
      //
      // r = [30.0, 10.0, 5.0, 0.0]
      // q = [1.0, 5.0, 10.0, 5.0]
      //
      // result = minimize(
      //     lambda x: sum((r[i] - x[i]) ** 2 / q[i] ** 2 for i in range(4)),
      //     [0.0, 0.0, 0.0, 0.0],
      //     constraints=[
      //         {"type": "ineq", "fun": lambda x: x},
      //         {"type": "ineq", "fun": lambda x: 40.0 - sum(x)},
      //     ],
      // )
      // print(result.x)
      assertEquals(29.960, currents[0], 1e-3);
      assertEquals(9.008, currents[1], 1e-3);
      assertEquals(1.032, currents[2], 1e-3);
      assertEquals(0.0, currents[3], 1e-3);
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }
}
