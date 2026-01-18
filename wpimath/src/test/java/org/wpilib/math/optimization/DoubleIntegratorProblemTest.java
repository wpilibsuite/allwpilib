// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.wpilib.math.autodiff.Variable.pow;
import static org.wpilib.math.optimization.Constraints.bounds;
import static org.wpilib.math.optimization.Constraints.eq;

import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;
import org.wpilib.math.autodiff.ExpressionType;
import org.wpilib.math.autodiff.Variable;
import org.wpilib.math.autodiff.VariableMatrix;
import org.wpilib.math.optimization.solver.ExitStatus;

class DoubleIntegratorProblemTest {
  @Test
  void testDoubleIntegratorProblem() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    final double TOTAL_TIME = 3.5; // s
    final double dt = 0.005; // s
    final int N = (int) (TOTAL_TIME / dt);

    final double r = 2.0; // m

    try (var problem = new Problem()) {
      // 2x1 state vector with N + 1 timesteps (includes last state)
      var X = problem.decisionVariable(2, N + 1);

      // 1x1 input vector with N timesteps (input at last state doesn't matter)
      var U = problem.decisionVariable(1, N);

      // Kinematics constraint assuming constant acceleration between timesteps
      for (int k = 0; k < N; ++k) {
        final double t = dt;
        var p_k1 = X.get(0, k + 1);
        var v_k1 = X.get(1, k + 1);
        var p_k = X.get(0, k);
        var v_k = X.get(1, k);
        var a_k = U.get(0, k);

        // pₖ₊₁ = pₖ + vₖt + 1/2aₖt²
        problem.subjectTo(eq(p_k1, p_k.plus(v_k.times(t)).plus(a_k.times(0.5 * t * t))));

        // vₖ₊₁ = vₖ + aₖt
        problem.subjectTo(eq(v_k1, v_k.plus(a_k.times(t))));
      }

      // Start and end at rest
      problem.subjectTo(eq(X.col(0), new VariableMatrix(new double[][] {{0.0}, {0.0}})));
      problem.subjectTo(eq(X.col(N), new VariableMatrix(new double[][] {{r}, {0.0}})));

      // Limit velocity
      problem.subjectTo(bounds(-1, X.row(1), 1));

      // Limit acceleration
      problem.subjectTo(bounds(-1, U, 1));

      // Cost function - minimize position error
      var J = new Variable(0.0);
      for (int k = 0; k < N + 1; ++k) {
        J = J.plus(pow(new Variable(r).minus(X.get(0, k)), 2));
      }
      problem.minimize(J);

      assertEquals(ExpressionType.QUADRATIC, problem.costFunctionType());
      assertEquals(ExpressionType.LINEAR, problem.equalityConstraintType());
      assertEquals(ExpressionType.LINEAR, problem.inequalityConstraintType());

      assertEquals(ExitStatus.SUCCESS, problem.solve());

      var A = new SimpleMatrix(new double[][] {{1.0, dt}, {0.0, 1.0}});
      var B = new SimpleMatrix(new double[][] {{0.5 * dt * dt}, {dt}});

      // Verify initial state
      assertEquals(0.0, X.value(0, 0), 1e-8);
      assertEquals(0.0, X.value(1, 0), 1e-8);

      // Verify solution
      var x = new SimpleMatrix(new double[][] {{0.0}, {0.0}});
      var u = new SimpleMatrix(new double[][] {{0.0}});
      for (int k = 0; k < N; ++k) {
        // Verify state
        assertEquals(x.get(0), X.value(0, k), 1e-2);
        assertEquals(x.get(1), X.value(1, k), 1e-2);

        // Determine expected input for this timestep
        if (k * dt < 1.0) {
          // Accelerate
          u.set(0, 0, 1.0);
        } else if (k * dt < 2.05) {
          // Maintain speed
          u.set(0, 0, 0.0);
        } else if (k * dt < 3.275) {
          // Decelerate
          u.set(0, 0, -1.0);
        } else {
          // Accelerate
          u.set(0, 0, 1.0);
        }

        // Verify input
        if (k > 0 && k < N - 1 && Math.abs(U.value(0, k - 1) - U.value(0, k + 1)) >= 1.0 - 1e-2) {
          // If control input is transitioning between -1, 0, or 1, ensure it's within (-1, 1)
          assertTrue(U.value(0, k) >= -1.0);
          assertTrue(U.value(0, k) <= 1.0);
        } else {
          assertEquals(u.get(0), U.value(0, k), 1e-4);
        }

        // Project state forward
        x = A.mult(x).plus(B.mult(u));
      }

      // Verify final state
      assertEquals(r, X.value(0, N), 1e-8);
      assertEquals(0.0, X.value(1, N), 1e-8);
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }
}
