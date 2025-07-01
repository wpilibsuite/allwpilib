// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.optimization;

import static edu.wpi.first.math.optimization.Constraints.eq;
import static edu.wpi.first.math.optimization.Constraints.ge;
import static edu.wpi.first.math.optimization.Constraints.le;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.autodiff.ExpressionType;
import edu.wpi.first.math.optimization.solver.ExitStatus;
import org.junit.jupiter.api.Test;

class LinearProblemTest {
  @Test
  void testMaximize() {
    var problem = new Problem();

    var x = problem.decisionVariable();
    x.setValue(1.0);

    var y = problem.decisionVariable();
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

  @Test
  void testFreeVariable() {
    var problem = new Problem();

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
}
