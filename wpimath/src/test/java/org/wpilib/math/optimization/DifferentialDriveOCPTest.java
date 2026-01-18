// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;
import org.wpilib.math.autodiff.ExpressionType;
import org.wpilib.math.autodiff.Variable;
import org.wpilib.math.autodiff.VariableMatrix;
import org.wpilib.math.optimization.ocp.DynamicsType;
import org.wpilib.math.optimization.ocp.TimestepMethod;
import org.wpilib.math.optimization.ocp.TranscriptionMethod;
import org.wpilib.math.optimization.solver.ExitStatus;
import org.wpilib.math.optimization.solver.Options;

class DifferentialDriveOCPTest {
  @Test
  void testDifferentialDrive() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    final int N = 50;

    final double minTimestep = 0.05; // s
    final var x_initial = new SimpleMatrix(new double[][] {{0.0}, {0.0}, {0.0}, {0.0}, {0.0}});
    final var x_final = new SimpleMatrix(new double[][] {{1.0}, {1.0}, {0.0}, {0.0}, {0.0}});
    final var u_min = new SimpleMatrix(new double[][] {{-12.0}, {-12.0}});
    final var u_max = new SimpleMatrix(new double[][] {{12.0}, {12.0}});

    try (var problem =
        new OCP(
            5,
            2,
            minTimestep,
            N,
            DifferentialDriveUtil::differentialDriveDynamics,
            DynamicsType.EXPLICIT_ODE,
            TimestepMethod.VARIABLE_SINGLE,
            TranscriptionMethod.DIRECT_TRANSCRIPTION)) {
      // Seed the min time formulation with lerp between waypoints
      for (int i = 0; i < N + 1; ++i) {
        problem.X().get(0, i).setValue((double) i / (N + 1));
        problem.X().get(1, i).setValue((double) i / (N + 1));
      }

      problem.constrainInitialState(x_initial);
      problem.constrainFinalState(x_final);

      problem.setLowerInputBound(u_min);
      problem.setUpperInputBound(u_max);

      problem.setMinTimestep(minTimestep);
      problem.setMaxTimestep(3.0);

      // Set up cost
      problem.minimize(problem.dt().times(VariableMatrix.ones(N + 1, 1)));

      assertEquals(ExpressionType.LINEAR, problem.costFunctionType());
      assertEquals(ExpressionType.NONLINEAR, problem.equalityConstraintType());
      assertEquals(ExpressionType.LINEAR, problem.inequalityConstraintType());

      assertEquals(ExitStatus.SUCCESS, problem.solve(new Options().withMaxIterations(1000)));

      var X = problem.X();

      // Verify initial state
      assertEquals(x_initial.get(0), X.value(0, 0), 1e-8);
      assertEquals(x_initial.get(1), X.value(1, 0), 1e-8);
      assertEquals(x_initial.get(2), X.value(2, 0), 1e-8);
      assertEquals(x_initial.get(3), X.value(3, 0), 1e-8);
      assertEquals(x_initial.get(4), X.value(4, 0), 1e-8);

      // Verify final state
      assertEquals(x_final.get(0), X.value(0, N), 1e-8);
      assertEquals(x_final.get(1), X.value(1, N), 1e-8);
      assertEquals(x_final.get(2), X.value(2, N), 1e-8);
      assertEquals(x_final.get(3), X.value(3, N), 1e-8);
      assertEquals(x_final.get(4), X.value(4, N), 1e-8);
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }
}
