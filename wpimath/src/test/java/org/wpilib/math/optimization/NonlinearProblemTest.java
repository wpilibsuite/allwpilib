// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.wpilib.math.DoubleRange.range;
import static org.wpilib.math.autodiff.Variable.hypot;
import static org.wpilib.math.autodiff.Variable.pow;
import static org.wpilib.math.autodiff.Variable.sqrt;
import static org.wpilib.math.optimization.Constraints.bounds;
import static org.wpilib.math.optimization.Constraints.eq;
import static org.wpilib.math.optimization.Constraints.ge;
import static org.wpilib.math.optimization.Constraints.le;

import org.junit.jupiter.api.Test;
import org.wpilib.math.autodiff.ExpressionType;
import org.wpilib.math.autodiff.Variable;
import org.wpilib.math.optimization.solver.ExitStatus;

class NonlinearProblemTest {
  @Test
  void testQuartic() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable();
      x.setValue(20.0);

      problem.minimize(pow(x, 4));

      problem.subjectTo(ge(x, 1));

      assertEquals(ExpressionType.NONLINEAR, problem.costFunctionType());
      assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
      assertEquals(ExpressionType.LINEAR, problem.inequalityConstraintType());

      assertEquals(ExitStatus.SUCCESS, problem.solve());

      assertEquals(1.0, x.value(), 1e-6);
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  private boolean near(double expected, double actual, double tolerance) {
    return Math.abs(expected - actual) < tolerance;
  }

  @Test
  void testRosenbrockWithCubicAndLineConstraint() {
    // https://en.wikipedia.org/wiki/Test_functions_for_optimization#Test_functions_for_constrained_optimization

    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable();
      var y = problem.decisionVariable();

      problem.minimize(
          pow(y.minus(pow(x, 2)), 2).times(100).plus(pow(new Variable(1).minus(x), 2)));

      problem.subjectTo(ge(y, pow(x.minus(1), 3).plus(1)));
      problem.subjectTo(le(y, x.unaryMinus().plus(2)));

      assertEquals(ExpressionType.NONLINEAR, problem.costFunctionType());
      assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
      assertEquals(ExpressionType.NONLINEAR, problem.inequalityConstraintType());

      for (var x0 : range(-1.5, 1.5, 0.1)) {
        for (var y0 : range(-0.5, 2.5, 0.1)) {
          x.setValue(x0);
          y.setValue(y0);

          assertEquals(ExitStatus.SUCCESS, problem.solve());

          // Local minimum at (0.0, 0.0)
          // Global minimum at (1.0, 1.0)
          assertTrue(near(0.0, x.value(), 1e-2) || near(1.0, x.value(), 1e-2));
          assertTrue(near(0.0, y.value(), 1e-2) || near(1.0, y.value(), 1e-2));
        }
      }
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testRosenbrockWithDiskConstraint() {
    // https://en.wikipedia.org/wiki/Test_functions_for_optimization#Test_functions_for_constrained_optimization

    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable();
      var y = problem.decisionVariable();

      problem.minimize(
          pow(new Variable(1).minus(x), 2).plus(pow(y.minus(pow(x, 2)), 2).times(100)));

      problem.subjectTo(le(pow(x, 2).plus(pow(y, 2)), 2));

      assertEquals(ExpressionType.NONLINEAR, problem.costFunctionType());
      assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
      assertEquals(ExpressionType.QUADRATIC, problem.inequalityConstraintType());

      for (var x0 : range(-1.5, 1.5, 0.1)) {
        for (var y0 : range(-1.5, 1.5, 0.1)) {
          x.setValue(x0);
          y.setValue(y0);

          assertEquals(ExitStatus.SUCCESS, problem.solve());

          assertEquals(1.0, x.value(), 1e-3);
          assertEquals(1.0, y.value(), 1e-3);
        }
      }
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testMinimum2DDistanceWithLinearConstraint() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable();
      var y = problem.decisionVariable();
      x.setValue(20.0);
      y.setValue(50.0);

      problem.minimize(sqrt(x.times(x).plus(y.times(y))));

      problem.subjectTo(eq(y, x.unaryMinus().plus(5.0)));

      assertEquals(ExpressionType.NONLINEAR, problem.costFunctionType());
      assertEquals(ExpressionType.LINEAR, problem.equalityConstraintType());
      assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

      assertEquals(ExitStatus.SUCCESS, problem.solve());

      assertEquals(2.5, x.value(), 1e-2);
      assertEquals(2.5, y.value(), 1e-2);
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testConflictingBounds() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable();
      var y = problem.decisionVariable();

      problem.minimize(hypot(x, y));

      problem.subjectTo(le(hypot(x, y), 1));
      problem.subjectTo(bounds(0.5, x, -0.5));

      assertEquals(ExpressionType.NONLINEAR, problem.costFunctionType());
      assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
      assertEquals(ExpressionType.NONLINEAR, problem.inequalityConstraintType());

      assertEquals(ExitStatus.GLOBALLY_INFEASIBLE, problem.solve());
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testWachterAndBieglerLineSearchFailure() {
    // See example 19.2 of [1]
    //
    // [1] Nocedal, J. and Wright, S. "Numerical Optimization", 2nd. ed., Ch. 19. Springer, 2006.

    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable();
      var s1 = problem.decisionVariable();
      var s2 = problem.decisionVariable();

      x.setValue(-2);
      s1.setValue(3);
      s2.setValue(1);

      problem.minimize(x);

      problem.subjectTo(eq(pow(x, 2).minus(s1).minus(1), 0));
      problem.subjectTo(eq(x.minus(s2).minus(0.5), 0));
      problem.subjectTo(ge(s1, 0));
      problem.subjectTo(ge(s2, 0));

      assertEquals(ExpressionType.LINEAR, problem.costFunctionType());
      assertEquals(ExpressionType.QUADRATIC, problem.equalityConstraintType());
      assertEquals(ExpressionType.LINEAR, problem.inequalityConstraintType());

      // FIXME: Fails with "line search failed"
      assertEquals(ExitStatus.LINE_SEARCH_FAILED, problem.solve());

      // assertEquals(1.0, x.value(), 1e-6);
      // assertEquals(0.0, s1.value(), 1e-6);
      // assertEquals(0.5, s2.value(), 1e-6);
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }
}
