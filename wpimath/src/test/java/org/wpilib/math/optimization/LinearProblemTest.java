// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.math.optimization.Constraints.eq;
import static org.wpilib.math.optimization.Constraints.ge;
import static org.wpilib.math.optimization.Constraints.le;

import org.junit.jupiter.api.Test;
import org.wpilib.math.autodiff.ExpressionType;
import org.wpilib.math.autodiff.Variable;
import org.wpilib.math.optimization.solver.ExitStatus;

class LinearProblemTest {
  @Test
  void testMaximize() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable();
      var y = problem.decisionVariable();
      x.setValue(1.0);
      y.setValue(1.0);

      problem.maximize(x.times(50).plus(y.times(40)));

      problem.subjectTo(le(x.plus(y.times(1.5)), 750));
      problem.subjectTo(le(x.times(2).plus(y.times(3)), 1500));
      problem.subjectTo(le(x.times(2).plus(y), 1000));
      problem.subjectTo(ge(x, 0));
      problem.subjectTo(ge(y, 0));

      assertEquals(ExpressionType.LINEAR, problem.costFunctionType());
      assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
      assertEquals(ExpressionType.LINEAR, problem.inequalityConstraintType());

      assertEquals(ExitStatus.SUCCESS, problem.solve());

      assertEquals(375.0, x.value(), 1e-6);
      assertEquals(250.0, y.value(), 1e-6);
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testFreeVariable() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var x = problem.decisionVariable(2);
      x.get(0).setValue(1.0);
      x.get(1).setValue(2.0);

      problem.subjectTo(eq(x.get(0), 0));

      assertEquals(ExpressionType.NONE, problem.costFunctionType());
      assertEquals(ExpressionType.LINEAR, problem.equalityConstraintType());
      assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

      assertEquals(ExitStatus.SUCCESS, problem.solve());

      assertEquals(0.0, x.get(0).value(), 1e-6);
      assertEquals(2.0, x.get(1).value(), 1e-6);
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }
}
