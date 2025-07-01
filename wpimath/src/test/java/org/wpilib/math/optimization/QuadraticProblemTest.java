// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.math.optimization.Constraints.eq;
import static org.wpilib.math.optimization.Constraints.ge;

import org.junit.jupiter.api.Test;
import org.wpilib.math.autodiff.ExpressionType;
import org.wpilib.math.autodiff.Variable;
import org.wpilib.math.optimization.solver.ExitStatus;

class QuadraticProblemTest {
  @Test
  void testUnconstrained1D() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable();
      x.setValue(2.0);

      problem.minimize(x.times(x).minus(x.times(6.0)));

      assertEquals(ExpressionType.QUADRATIC, problem.costFunctionType());
      assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
      assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

      assertEquals(ExitStatus.SUCCESS, problem.solve());

      assertEquals(3.0, x.value(), 1e-6);
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testUnconstrained2D() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable();
      var y = problem.decisionVariable();
      x.setValue(1.0);
      y.setValue(2.0);

      problem.minimize(x.times(x).plus(y.times(y)));

      assertEquals(ExpressionType.QUADRATIC, problem.costFunctionType());
      assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
      assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

      assertEquals(ExitStatus.SUCCESS, problem.solve());

      assertEquals(0.0, x.value(), 1e-6);
      assertEquals(0.0, y.value(), 1e-6);
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable(2);
      x.get(0).setValue(1.0);
      x.get(1).setValue(2.0);

      problem.minimize(x.T().times(x).get(0));

      assertEquals(ExpressionType.QUADRATIC, problem.costFunctionType());
      assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
      assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

      assertEquals(ExitStatus.SUCCESS, problem.solve());

      assertEquals(0.0, x.value(0), 1e-6);
      assertEquals(0.0, x.value(1), 1e-6);
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testEqualityConstrained() {
    // Maximize xy subject to x + 3y = 36.
    //
    // Maximize f(x,y) = xy
    // subject to g(x,y) = x + 3y - 36 = 0
    //
    //         value func  constraint
    //              |          |
    //              v          v
    // L(x,y,λ) = f(x,y) - λg(x,y)
    // L(x,y,λ) = xy - λ(x + 3y - 36)
    // L(x,y,λ) = xy - xλ - 3yλ + 36λ
    //
    // ∇_x,y,λ L(x,y,λ) = 0
    //
    // ∂L/∂x = y - λ
    // ∂L/∂y = x - 3λ
    // ∂L/∂λ = -x - 3y + 36
    //
    //  0x + 1y - 1λ = 0
    //  1x + 0y - 3λ = 0
    // -1x - 3y + 0λ + 36 = 0
    //
    // [ 0  1 -1][x]   [  0]
    // [ 1  0 -3][y] = [  0]
    // [-1 -3  0][λ]   [-36]
    //
    // Solve with:
    //
    // ```python
    //   np.linalg.solve(
    //     np.array([[0,1,-1],
    //               [1,0,-3],
    //               [-1,-3,0]]),
    //     np.array([[0], [0], [-36]]))
    // ```
    //
    // [x]   [18]
    // [y] = [ 6]
    // [λ]   [ 6]

    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable();
      var y = problem.decisionVariable();

      problem.maximize(x.times(y));

      problem.subjectTo(eq(x.plus(y.times(3)), 36));

      assertEquals(ExpressionType.QUADRATIC, problem.costFunctionType());
      assertEquals(ExpressionType.LINEAR, problem.equalityConstraintType());
      assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

      assertEquals(ExitStatus.SUCCESS, problem.solve());

      assertEquals(18.0, x.value(), 1e-5);
      assertEquals(6.0, y.value(), 1e-5);
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable(2);
      x.get(0).setValue(1.0);
      x.get(1).setValue(2.0);

      problem.minimize(x.T().times(x).get(0));

      problem.subjectTo(eq(x, new double[][] {{3.0}, {3.0}}));

      assertEquals(ExpressionType.QUADRATIC, problem.costFunctionType());
      assertEquals(ExpressionType.LINEAR, problem.equalityConstraintType());
      assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

      assertEquals(ExitStatus.SUCCESS, problem.solve());

      assertEquals(3.0, x.value(0), 1e-5);
      assertEquals(3.0, x.value(1), 1e-5);
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testInequalityConstrained2D() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable();
      var y = problem.decisionVariable();
      x.setValue(5.0);
      y.setValue(5.0);

      problem.minimize(x.times(x).plus(y.times(2).times(y)));
      problem.subjectTo(ge(y, x.unaryMinus().plus(5)));

      assertEquals(ExpressionType.QUADRATIC, problem.costFunctionType());
      assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
      assertEquals(ExpressionType.LINEAR, problem.inequalityConstraintType());

      assertEquals(ExitStatus.SUCCESS, problem.solve());

      assertEquals(3.0 + 1.0 / 3.0, x.value(), 1e-6);
      assertEquals(1.0 + 2.0 / 3.0, y.value(), 1e-6);
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }
}
