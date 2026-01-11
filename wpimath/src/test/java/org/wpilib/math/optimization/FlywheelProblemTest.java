// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.wpilib.math.optimization.Constraints.bounds;
import static org.wpilib.math.optimization.Constraints.eq;

import org.junit.jupiter.api.Test;
import org.wpilib.math.autodiff.ExpressionType;
import org.wpilib.math.autodiff.Variable;
import org.wpilib.math.autodiff.VariableMatrix;
import org.wpilib.math.optimization.solver.ExitStatus;

class FlywheelProblemTest {
  private boolean near(double expected, double actual, double tolerance) {
    return Math.abs(expected - actual) < tolerance;
  }

  @Test
  void testFlywheelProblem() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    final double TOTAL_TIME = 5.0; // s
    final double dt = 0.005; // s
    final int N = (int) (TOTAL_TIME / dt);

    // Flywheel model:
    // States: [velocity]
    // Inputs: [voltage]
    double A = Math.exp(-dt);
    double B = 1.0 - Math.exp(-dt);

    try (var problem = new Problem()) {
      var X = problem.decisionVariable(1, N + 1);
      var U = problem.decisionVariable(1, N);

      // Dynamics constraint
      for (int k = 0; k < N; ++k) {
        problem.subjectTo(
            eq(
                X.col(k + 1),
                new Variable(A)
                    .times(X.col(k).get(0))
                    .plus(new Variable(B).times(U.col(k).get(0)))));
      }

      // State and input constraints
      problem.subjectTo(eq(X.col(0), 0.0));
      problem.subjectTo(bounds(-12, U, 12));

      // Cost function - minimize error
      final var r = new VariableMatrix(new double[][] {{10.0}});
      var J = new Variable(0.0);
      for (int k = 0; k < N + 1; ++k) {
        J = J.plus(r.minus(X.col(k)).T().times(r.minus(X.col(k))).get(0));
      }
      problem.minimize(J);

      assertEquals(ExpressionType.QUADRATIC, problem.costFunctionType());
      assertEquals(ExpressionType.LINEAR, problem.equalityConstraintType());
      assertEquals(ExpressionType.LINEAR, problem.inequalityConstraintType());

      assertEquals(ExitStatus.SUCCESS, problem.solve());

      // Voltage for steady-state velocity:
      //
      // rₖ₊₁ = Arₖ + Buₖ
      // uₖ = B⁺(rₖ₊₁ − Arₖ)
      // uₖ = B⁺(rₖ − Arₖ)
      // uₖ = B⁺(I − A)rₖ
      double u_ss = 1.0 / B * (1.0 - A) * r.value(0);

      // Verify initial state
      assertEquals(0.0, X.value(0, 0), 1e-8);

      // Verify solution
      double x = 0.0;
      double u;
      for (int k = 0; k < N; ++k) {
        // Verify state
        assertEquals(x, X.value(0, k), 1e-2);

        // Determine expected input for this timestep
        double error = r.value(0) - x;
        if (error > 1e-2) {
          // Max control input until the reference is reached
          u = 12.0;
        } else {
          // Maintain speed
          u = u_ss;
        }

        // Verify input
        if (k > 0
            && k < N - 1
            && near(12.0, U.value(0, k - 1), 1e-2)
            && near(u_ss, U.value(0, k + 1), 1e-2)) {
          // If control input is transitioning between 12 and u_ss, ensure it's
          // within (u_ss, 12)
          assertTrue(U.value(0, k) >= u_ss);
          assertTrue(U.value(0, k) <= 12.0);
        } else {
          assertEquals(u, U.value(0, k), 1e-4);
        }

        // Project state forward
        x = A * x + B * u;
      }

      // Verify final state
      assertEquals(r.value(0), X.value(0, N), 1e-7);
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }
}
