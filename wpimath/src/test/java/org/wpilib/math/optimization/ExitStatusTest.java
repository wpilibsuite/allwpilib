// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.math.autodiff.Variable.sqrt;
import static org.wpilib.math.optimization.Constraints.eq;
import static org.wpilib.math.optimization.Constraints.ge;

import org.junit.jupiter.api.Test;
import org.wpilib.math.autodiff.ExpressionType;
import org.wpilib.math.autodiff.Variable;
import org.wpilib.math.optimization.solver.ExitStatus;
import org.wpilib.math.optimization.solver.Options;

// These tests ensure coverage of the off-nominal exit statuses

class ExitStatusTest {
  @Test
  void testCallbackRequestedStop() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable();
      problem.minimize(x.times(x));

      problem.addCallback(info -> false);
      assertEquals(ExitStatus.SUCCESS, problem.solve());

      problem.addCallback(info -> true);
      assertEquals(ExitStatus.CALLBACK_REQUESTED_STOP, problem.solve());

      problem.clearCallbacks();
      problem.addCallback(info -> false);
      assertEquals(ExitStatus.SUCCESS, problem.solve());
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testTooFewDOFs() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable();
      var y = problem.decisionVariable();
      var z = problem.decisionVariable();

      problem.subjectTo(eq(x, 1));
      problem.subjectTo(eq(x, 2));
      problem.subjectTo(eq(y, 1));
      problem.subjectTo(eq(z, 1));

      assertEquals(ExpressionType.NONE, problem.costFunctionType());
      assertEquals(ExpressionType.LINEAR, problem.equalityConstraintType());
      assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

      assertEquals(ExitStatus.TOO_FEW_DOFS, problem.solve());
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testLocallyInfeasible() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    // Equality constraints
    try (var problem = new Problem()) {
      var x = problem.decisionVariable();
      var y = problem.decisionVariable();
      var z = problem.decisionVariable();

      problem.subjectTo(eq(x, y.plus(1)));
      problem.subjectTo(eq(y, z.plus(1)));
      problem.subjectTo(eq(z, x.plus(1)));

      assertEquals(ExpressionType.NONE, problem.costFunctionType());
      assertEquals(ExpressionType.LINEAR, problem.equalityConstraintType());
      assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

      assertEquals(ExitStatus.LOCALLY_INFEASIBLE, problem.solve());
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());

    // Inequality constraints
    try (var problem = new Problem()) {
      var x = problem.decisionVariable();
      var y = problem.decisionVariable();
      var z = problem.decisionVariable();

      problem.subjectTo(ge(x, y.plus(1)));
      problem.subjectTo(ge(y, z.plus(1)));
      problem.subjectTo(ge(z, x.plus(1)));

      assertEquals(ExpressionType.NONE, problem.costFunctionType());
      assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
      assertEquals(ExpressionType.LINEAR, problem.inequalityConstraintType());

      assertEquals(ExitStatus.LOCALLY_INFEASIBLE, problem.solve());
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testNonfiniteInitialCostOrConstraints() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable();
      x.setValue(-1.0);

      problem.minimize(sqrt(x));

      assertEquals(ExpressionType.NONLINEAR, problem.costFunctionType());
      assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
      assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

      assertEquals(ExitStatus.NONFINITE_INITIAL_COST_OR_CONSTRAINTS, problem.solve());
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testDivergingIterates() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable();

      problem.minimize(x);

      assertEquals(ExpressionType.LINEAR, problem.costFunctionType());
      assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
      assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

      assertEquals(ExitStatus.DIVERGING_ITERATES, problem.solve());
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testMaxIterationsExceeded() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable();

      problem.minimize(x.times(x));

      assertEquals(ExpressionType.QUADRATIC, problem.costFunctionType());
      assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
      assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

      assertEquals(
          ExitStatus.MAX_ITERATIONS_EXCEEDED, problem.solve(new Options().withMaxIterations(0)));
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testTimeout() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable();

      problem.minimize(x.times(x));

      assertEquals(ExpressionType.QUADRATIC, problem.costFunctionType());
      assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
      assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

      assertEquals(ExitStatus.TIMEOUT, problem.solve(new Options().withTimeout(0.0)));
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }
}
