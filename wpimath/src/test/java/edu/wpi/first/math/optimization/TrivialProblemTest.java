// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.optimization;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.autodiff.ExpressionType;
import edu.wpi.first.math.optimization.solver.ExitStatus;
import org.junit.jupiter.api.Test;

class TrivialProblemTest {
  @Test
  void testEmpty() {
    var problem = new Problem();

    assertEquals(ExpressionType.NONE, problem.costFunctionType());
    assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
    assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

    assertEquals(ExitStatus.SUCCESS, problem.solve());
  }

  @Test
  void testNoCostUnconstrained() {
    {
      var problem = new Problem();

      @SuppressWarnings("VariableDeclarationUsageDistance")
      var X = problem.decisionVariable(2, 3);

      assertEquals(ExpressionType.NONE, problem.costFunctionType());
      assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
      assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

      assertEquals(ExitStatus.SUCCESS, problem.solve());

      for (int row = 0; row < X.rows(); ++row) {
        for (int col = 0; col < X.cols(); ++col) {
          assertEquals(0.0, X.value(row, col));
        }
      }
    }

    {
      var problem = new Problem();

      var X = problem.decisionVariable(2, 3);
      X.setValue(new double[][] {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}});

      assertEquals(ExpressionType.NONE, problem.costFunctionType());
      assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
      assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

      assertEquals(ExitStatus.SUCCESS, problem.solve());

      for (int row = 0; row < X.rows(); ++row) {
        for (int col = 0; col < X.cols(); ++col) {
          assertEquals(1.0, X.value(row, col));
        }
      }
    }
  }
}
