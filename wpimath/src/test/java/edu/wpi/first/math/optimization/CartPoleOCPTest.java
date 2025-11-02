// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.optimization;

import static edu.wpi.first.math.optimization.Constraints.ge;
import static edu.wpi.first.math.optimization.Constraints.le;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.autodiff.ExpressionType;
import edu.wpi.first.math.autodiff.Variable;
import edu.wpi.first.math.optimization.ocp.DynamicsType;
import edu.wpi.first.math.optimization.ocp.TimestepMethod;
import edu.wpi.first.math.optimization.ocp.TranscriptionMethod;
import edu.wpi.first.math.optimization.solver.ExitStatus;
import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;

class CartPoleOCPTest {
  @Test
  void testCartPole() {
    final double TOTAL_TIME = 5.0; // s
    final double dt = 0.05; // s
    final int N = (int) (TOTAL_TIME / dt);

    final double u_max = 20.0; // N
    final double d_max = 2.0; // m

    final var x_initial = new SimpleMatrix(new double[][] {{0.0}, {0.0}, {0.0}, {0.0}});
    final var x_final = new SimpleMatrix(new double[][] {{1.0}, {Math.PI}, {0.0}, {0.0}});

    var problem =
        new OCP(
            4,
            1,
            dt,
            N,
            CartPoleUtil::cartPoleDynamics,
            DynamicsType.EXPLICIT_ODE,
            TimestepMethod.VARIABLE_SINGLE,
            TranscriptionMethod.DIRECT_COLLOCATION);

    // x = [q, q̇]ᵀ = [x, θ, ẋ, θ̇]ᵀ
    var X = problem.X();

    // Initial guess
    for (int k = 0; k < N + 1; ++k) {
      X.get(0, k).setValue(MathUtil.lerp(x_initial.get(0), x_final.get(0), (double) k / N));
      X.get(1, k).setValue(MathUtil.lerp(x_initial.get(1), x_final.get(1), (double) k / N));
    }

    // Initial conditions
    problem.constrainInitialState(x_initial);

    // Final conditions
    problem.constrainFinalState(x_final);

    // Cart position constraints
    problem.forEachStep(
        (x, u) -> {
          problem.subjectTo(ge(x.get(0), 0.0));
          problem.subjectTo(le(x.get(0), d_max));
        });

    // Input constraints
    problem.setLowerInputBound(-u_max);
    problem.setUpperInputBound(u_max);

    // u = f_x
    var U = problem.U();

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

    // Verify final state
    assertEquals(x_final.get(0), X.value(0, N), 1e-8);
    assertEquals(x_final.get(1), X.value(1, N), 1e-8);
    assertEquals(x_final.get(2), X.value(2, N), 1e-8);
    assertEquals(x_final.get(3), X.value(3, N), 1e-8);
  }
}
