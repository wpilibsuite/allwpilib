// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;
import org.wpilib.math.autodiff.ExpressionType;
import org.wpilib.math.autodiff.Variable;
import org.wpilib.math.optimization.solver.ExitStatus;

class TrivialProblemTest {
  @Test
  void testEmpty() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      assertEquals(ExpressionType.NONE, problem.costFunctionType());
      assertEquals(ExpressionType.NONE, problem.equalityConstraintType());
      assertEquals(ExpressionType.NONE, problem.inequalityConstraintType());

      assertEquals(ExitStatus.SUCCESS, problem.solve());
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testNoCostUnconstrained() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
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

    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      var X = problem.decisionVariable(2, 3);
      X.setValue(SimpleMatrix.ones(2, 3));

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

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }
}
