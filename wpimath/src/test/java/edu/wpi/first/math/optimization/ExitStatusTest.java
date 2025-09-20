// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.optimization;

import static edu.wpi.first.math.autodiff.Variable.sqrt;
import static edu.wpi.first.math.optimization.Constraints.eq;
import static edu.wpi.first.math.optimization.Constraints.ge;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.autodiff.ExpressionType;
import edu.wpi.first.math.optimization.solver.ExitStatus;
import edu.wpi.first.math.optimization.solver.Options;
import org.junit.jupiter.api.Test;

// These tests ensure coverage of the off-nominal exit statuses

class ExitStatusTest {
  @Test
  void testCallbackRequestedStop() {
    var problem = new Problem();

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

  @Test
  void testTooFewDOFs() {
    var problem = new Problem();

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

  @Test
  void testLocallyInfeasible() {
    // Equality constraints
    {
      var problem = new Problem();

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

    // Inequality constraints
    {
      var problem = new Problem();

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
  }

  @Test
  void testNonfiniteInitialCostOrConstraints() {
    var problem = new Problem();

    var x = problem.decisionVariable();
    x.setValue(-1.0);
    problem.minimize(sqrt(x));

    assertEquals(ExpressionType.NONLINEAR, problem.costFunctionType());
    assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
    assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

    assertEquals(ExitStatus.NONFINITE_INITIAL_COST_OR_CONSTRAINTS, problem.solve());
  }

  @Test
  void testDivergingIterates() {
    var problem = new Problem();

    var x = problem.decisionVariable();
    problem.minimize(x);

    assertEquals(ExpressionType.LINEAR, problem.costFunctionType());
    assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
    assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

    assertEquals(ExitStatus.DIVERGING_ITERATES, problem.solve());
  }

  @Test
  void testMaxIterationsExceeded() {
    var problem = new Problem();

    var x = problem.decisionVariable();
    problem.minimize(x.times(x));

    assertEquals(ExpressionType.QUADRATIC, problem.costFunctionType());
    assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
    assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

    assertEquals(
        ExitStatus.MAX_ITERATIONS_EXCEEDED, problem.solve(new Options().withMaxIterations(0)));
  }

  @Test
  void testTimeout() {
    var problem = new Problem();

    var x = problem.decisionVariable();
    problem.minimize(x.times(x));

    assertEquals(ExpressionType.QUADRATIC, problem.costFunctionType());
    assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
    assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

    assertEquals(ExitStatus.TIMEOUT, problem.solve(new Options().withTimeout(0.0)));
  }
}
