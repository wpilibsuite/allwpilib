// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.function.BiFunction;
import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;
import org.wpilib.math.autodiff.ExpressionType;
import org.wpilib.math.autodiff.Variable;
import org.wpilib.math.autodiff.VariableMatrix;
import org.wpilib.math.optimization.ocp.DynamicsType;
import org.wpilib.math.optimization.ocp.TimestepMethod;
import org.wpilib.math.optimization.ocp.TranscriptionMethod;
import org.wpilib.math.optimization.solver.ExitStatus;

class FlywheelOCPTest {
  private boolean near(double expected, double actual, double tolerance) {
    return Math.abs(expected - actual) < tolerance;
  }

  void flywheelTest(
      double A,
      double B,
      BiFunction<VariableMatrix, VariableMatrix, VariableMatrix> f,
      DynamicsType dynamicsType,
      TranscriptionMethod transcriptionMethod) {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    final double TOTAL_TIME = 5.0; // s
    final double dt = 0.005; // s
    final int N = (int) (TOTAL_TIME / dt);

    // Flywheel model:
    // States: [velocity]
    // Inputs: [voltage]
    final double A_discrete = Math.exp(A * dt);
    final double B_discrete = (1.0 - A_discrete) * B;

    final double r = 10.0;

    try (var problem =
        new OCP(1, 1, dt, N, f, dynamicsType, TimestepMethod.FIXED, transcriptionMethod)) {
      problem.constrainInitialState(0.0);
      problem.setUpperInputBound(12);
      problem.setLowerInputBound(-12);

      // Set up cost
      var r_mat = new VariableMatrix(SimpleMatrix.filled(1, N + 1, r));
      problem.minimize(r_mat.minus(problem.X()).times(r_mat.minus(problem.X()).T()));

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
      double u_ss = 1.0 / B_discrete * (1.0 - A_discrete) * r;

      // Verify initial state
      assertEquals(0.0, problem.X().value(0, 0), 1e-8);

      // Verify solution
      double x = 0.0;
      double u;
      for (int k = 0; k < N; ++k) {
        // Verify state
        assertEquals(x, problem.X().value(0, k), 1e-2);

        // Determine expected input for this timestep
        double error = r - x;
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
            && near(12.0, problem.U().value(0, k - 1), 1e-2)
            && near(u_ss, problem.U().value(0, k + 1), 1e-2)) {
          // If control input is transitioning between 12 and u_ss, ensure it's
          // within (u_ss, 12)
          assertTrue(problem.U().value(0, k) >= u_ss);
          assertTrue(problem.U().value(0, k) <= 12.0);
        } else {
          if (transcriptionMethod == TranscriptionMethod.DIRECT_COLLOCATION) {
            // The tolerance is large because the trajectory is represented by a
            // spline, and splines chatter when transitioning quickly between
            // steady-states.
            assertEquals(u, problem.U().value(0, k), 2.0);
          } else {
            assertEquals(u, problem.U().value(0, k), 1e-4);
          }
        }

        // Project state forward
        x = A_discrete * x + B_discrete * u;
      }

      // Verify final state
      assertEquals(r, problem.X().value(0, N), 1e-6);
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  static final double A = -1.0;
  static final double B = 1.0;

  static final double dt = 0.005; // s

  static final double A_discrete = Math.exp(A * dt);
  static final double B_discrete = (1.0 - A_discrete) * B;

  private static VariableMatrix f_ode(VariableMatrix x, VariableMatrix u) {
    return new VariableMatrix(new double[][] {{A}})
        .times(x)
        .plus(new VariableMatrix(new double[][] {{B}}).times(u));
  }

  private static VariableMatrix f_discrete(VariableMatrix x, VariableMatrix u) {
    return new VariableMatrix(new double[][] {{A_discrete}})
        .times(x)
        .plus(new VariableMatrix(new double[][] {{B_discrete}}).times(u));
  }

  @Test
  void testFlywheelExplicit() {
    flywheelTest(
        A,
        B,
        FlywheelOCPTest::f_ode,
        DynamicsType.EXPLICIT_ODE,
        TranscriptionMethod.DIRECT_COLLOCATION);
    flywheelTest(
        A,
        B,
        FlywheelOCPTest::f_ode,
        DynamicsType.EXPLICIT_ODE,
        TranscriptionMethod.DIRECT_TRANSCRIPTION);
    flywheelTest(
        A,
        B,
        FlywheelOCPTest::f_ode,
        DynamicsType.EXPLICIT_ODE,
        TranscriptionMethod.SINGLE_SHOOTING);
  }

  @Test
  void testFlywheelDiscrete() {
    flywheelTest(
        A,
        B,
        FlywheelOCPTest::f_discrete,
        DynamicsType.DISCRETE,
        TranscriptionMethod.DIRECT_TRANSCRIPTION);
    flywheelTest(
        A,
        B,
        FlywheelOCPTest::f_discrete,
        DynamicsType.DISCRETE,
        TranscriptionMethod.SINGLE_SHOOTING);
  }
}
