// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.optimization;

import static edu.wpi.first.math.autodiff.NumericalIntegration.rk4;
import static edu.wpi.first.math.optimization.Constraints.eq;
import static edu.wpi.first.math.optimization.Constraints.ge;
import static edu.wpi.first.math.optimization.Constraints.le;
import static edu.wpi.first.math.system.NumericalIntegration.rk4;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.autodiff.ExpressionType;
import edu.wpi.first.math.autodiff.Variable;
import edu.wpi.first.math.optimization.solver.ExitStatus;
import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;

class DifferentialDriveProblemTest {
  @Test
  void testDifferentialDriveProblem() {
    final double TOTAL_TIME = 5.0; // s
    final double dt = 0.05; // s
    final int N = (int) (TOTAL_TIME / dt);

    final double u_max = 12.0; // V

    final var x_initial = new SimpleMatrix(new double[][] {{0.0}, {0.0}, {0.0}, {0.0}, {0.0}});
    final var x_final = new SimpleMatrix(new double[][] {{1.0}, {1.0}, {0.0}, {0.0}, {0.0}});

    var problem = new Problem();

    // x = [x, y, heading, left velocity, right velocity]ᵀ
    var X = problem.decisionVariable(5, N + 1);

    // Initial guess
    for (int k = 0; k < N; ++k) {
      X.get(0, k).setValue(MathUtil.lerp(x_initial.get(0), x_final.get(0), (double) k / N));
      X.get(1, k).setValue(MathUtil.lerp(x_initial.get(1), x_final.get(1), (double) k / N));
    }

    // u = [left voltage, right voltage]ᵀ
    var U = problem.decisionVariable(2, N);

    // Initial conditions
    problem.subjectTo(eq(X.col(0), x_initial));

    // Final conditions
    problem.subjectTo(eq(X.col(N), x_final));

    // Input constraints
    problem.subjectTo(ge(U, -u_max));
    problem.subjectTo(le(U, u_max));

    // Dynamics constraints - RK4 integration
    for (int k = 0; k < N; ++k) {
      problem.subjectTo(
          eq(
              X.col(k + 1),
              rk4(DifferentialDriveUtil::differentialDriveDynamics, X.col(k), U.col(k), dt)));
    }

    // Minimize sum squared states and inputs
    var J = new Variable(0.0);
    for (int k = 0; k < N; ++k) {
      J = J.plus(X.col(k).T().times(X.col(k)).plus(U.col(k).T().times(U.col(k))).get(0));
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
    assertEquals(x_initial.get(4), X.value(4, 0), 1e-8);

    // Verify solution
    var x = new SimpleMatrix(new double[][] {{0.0}, {0.0}, {0.0}, {0.0}, {0.0}});
    for (int k = 0; k < N; ++k) {
      // Input constraints
      assertTrue(U.get(0, k).value() >= -u_max);
      assertTrue(U.get(0, k).value() <= u_max);
      assertTrue(U.get(1, k).value() >= -u_max);
      assertTrue(U.get(1, k).value() <= u_max);

      // Verify state
      assertEquals(x.get(0), X.value(0, k), 1e-8);
      assertEquals(x.get(1), X.value(1, k), 1e-8);
      assertEquals(x.get(2), X.value(2, k), 1e-8);
      assertEquals(x.get(3), X.value(3, k), 1e-8);
      assertEquals(x.get(4), X.value(4, k), 1e-8);

      // Project state forward
      var u = U.col(k).value();
      x = rk4(DifferentialDriveUtil::differentialDriveDynamics, x, u, dt);
    }

    // Verify final state
    assertEquals(x_final.get(0), X.value(0, N), 1e-8);
    assertEquals(x_final.get(1), X.value(1, N), 1e-8);
    assertEquals(x_final.get(2), X.value(2, N), 1e-8);
    assertEquals(x_final.get(3), X.value(3, N), 1e-8);
    assertEquals(x_final.get(4), X.value(4, N), 1e-8);
  }
}
