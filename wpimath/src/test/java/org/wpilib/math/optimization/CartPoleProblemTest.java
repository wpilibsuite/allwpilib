// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.wpilib.math.autodiff.NumericalIntegration.rk4;
import static org.wpilib.math.optimization.Constraints.bounds;
import static org.wpilib.math.optimization.Constraints.eq;
import static org.wpilib.math.system.NumericalIntegration.rk4;

import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;
import org.wpilib.math.autodiff.ExpressionType;
import org.wpilib.math.autodiff.Variable;
import org.wpilib.math.optimization.solver.ExitStatus;
import org.wpilib.math.util.MathUtil;

class CartPoleProblemTest {
  @Test
  void testCartPoleProblem() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    final double TOTAL_TIME = 5.0; // s
    final double dt = 0.05; // s
    final int N = (int) (TOTAL_TIME / dt);

    final double u_max = 20.0; // N
    final double d_max = 2.0; // m

    final var x_initial = new SimpleMatrix(new double[][] {{0.0}, {0.0}, {0.0}, {0.0}});
    final var x_final = new SimpleMatrix(new double[][] {{1.0}, {Math.PI}, {0.0}, {0.0}});

    try (var problem = new Problem()) {
      // x = [q, q̇]ᵀ = [x, θ, ẋ, θ̇]ᵀ
      var X = problem.decisionVariable(4, N + 1);

      // Initial guess
      for (int k = 0; k < N + 1; ++k) {
        X.get(0, k).setValue(MathUtil.lerp(x_initial.get(0), x_final.get(0), (double) k / N));
        X.get(1, k).setValue(MathUtil.lerp(x_initial.get(1), x_final.get(1), (double) k / N));
      }

      // u = f_x
      var U = problem.decisionVariable(1, N);

      // Initial conditions
      problem.subjectTo(eq(X.col(0), x_initial));

      // Final conditions
      problem.subjectTo(eq(X.col(N), x_final));

      // Cart position constraints
      problem.subjectTo(bounds(0.0, X.row(0), d_max));

      // Input constraints
      problem.subjectTo(bounds(-u_max, U, u_max));

      // Dynamics constraints - RK4 integration
      for (int k = 0; k < N; ++k) {
        problem.subjectTo(
            eq(X.col(k + 1), rk4(CartPoleUtil::cartPoleDynamics, X.col(k), U.col(k), dt)));
      }

      // Minimize sum squared inputs
      var J = new Variable(0.0);
      for (int k = 0; k < N; ++k) {
        J = J.plus(U.col(k).T().times(U.col(k)).get(0));
      }
      problem.minimize(J);

      assertEquals(ExpressionType.QUADRATIC, problem.costFunctionType());
      assertEquals(ExpressionType.NONLINEAR, problem.equalityConstraintType());
      assertEquals(ExpressionType.LINEAR, problem.inequalityConstraintType());

      assertEquals(ExitStatus.SUCCESS, problem.solve());

      // Verify initial state
      assertEquals(x_initial.get(0), X.value(0, 0), 1e-8);
      assertEquals(x_initial.get(1), X.value(1, 0), 1e-8);
      assertEquals(x_initial.get(2), X.value(2, 0), 1e-8);
      assertEquals(x_initial.get(3), X.value(3, 0), 1e-8);

      // Verify solution
      for (int k = 0; k < N; ++k) {
        // Cart position constraints
        assertTrue(X.get(0, k).value() >= 0.0);
        assertTrue(X.get(0, k).value() <= d_max);

        // Input constraints
        assertTrue(U.get(0, k).value() >= -u_max);
        assertTrue(U.get(0, k).value() <= u_max);

        // Dynamics constraints
        var expected_x_k1 =
            rk4(CartPoleUtil::cartPoleDynamics, X.col(k).value(), U.col(k).value(), dt);
        var actual_x_k1 = X.col(k + 1).value();
        for (int row = 0; row < actual_x_k1.getNumRows(); ++row) {
          assertEquals(expected_x_k1.get(row), actual_x_k1.get(row), 1e-8);
        }
      }

      // Verify final state
      assertEquals(x_final.get(0), X.value(0, N), 1e-8);
      assertEquals(x_final.get(1), X.value(1, N), 1e-8);
      assertEquals(x_final.get(2), X.value(2, N), 1e-8);
      assertEquals(x_final.get(3), X.value(3, N), 1e-8);
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }
}
