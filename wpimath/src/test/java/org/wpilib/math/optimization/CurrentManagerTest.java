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

      // Expected values are from the following CasADi program:
      //
      // #!/usr/bin/env python3
      //
      // import casadi as ca
      // import numpy as np
      //
      // opti = ca.Opti()
      // allocated_currents = opti.variable(4, 1)
      //
      // current_tolerances = np.array([[1.0], [5.0], [10.0], [5.0]])
      // desired_currents = np.array([[30.0], [10.0], [5.0], [0.0]])
      //
      // J = 0.0
      // current_sum = 0.0
      // for i in range(4):
      //     error = desired_currents[i, 0] - allocated_currents[i, 0]
      //     J += error**2 / current_tolerances[i] ** 2
      //
      //     current_sum += allocated_currents[i, 0]
      //
      //     # Currents must be nonnegative
      //     opti.subject_to(allocated_currents[i, 0] >= 0.0)
      // opti.minimize(J)
      //
      // # Keep total current below maximum
      // opti.subject_to(current_sum <= 40.0)
      //
      // opti.solver("ipopt")
      // print(opti.solve().value(allocated_currents))
      assertEquals(29.960, currents[0], 1e-3);
      assertEquals(9.007, currents[1], 1e-3);
      assertEquals(1.032, currents[2], 1e-3);
      assertEquals(0.0, currents[3], 1e-3);
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }
}
