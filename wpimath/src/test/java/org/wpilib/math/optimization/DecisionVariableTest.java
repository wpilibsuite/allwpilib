// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.math.MatrixAssertions.assertEquals;

import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;
import org.wpilib.math.autodiff.Variable;

class DecisionVariableTest {
  @Test
  void testScalarInitAssign() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      // Scalar zero init
      var x = problem.decisionVariable();
      assertEquals(0.0, x.value());

      // Scalar assignment
      x.setValue(1.0);
      assertEquals(1.0, x.value());
      x.setValue(2.0);
      assertEquals(2.0, x.value());
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testVectorInitAssign() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      // Vector zero init
      var y = problem.decisionVariable(2);
      assertEquals(0.0, y.value(0));
      assertEquals(0.0, y.value(1));

      // Vector assignment
      y.get(0).setValue(1.0);
      y.get(1).setValue(2.0);
      assertEquals(1.0, y.value(0));
      assertEquals(2.0, y.value(1));
      y.get(0).setValue(3.0);
      y.get(1).setValue(4.0);
      assertEquals(3.0, y.value(0));
      assertEquals(4.0, y.value(1));
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testDynamicMatrixInitAssign() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      // Matrix zero init
      var z = problem.decisionVariable(3, 2);
      assertEquals(0.0, z.value(0, 0));
      assertEquals(0.0, z.value(0, 1));
      assertEquals(0.0, z.value(1, 0));
      assertEquals(0.0, z.value(1, 1));
      assertEquals(0.0, z.value(2, 0));
      assertEquals(0.0, z.value(2, 1));

      // Matrix assignment; element comparison
      z.setValue(new double[][] {{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}});
      assertEquals(1.0, z.value(0, 0));
      assertEquals(2.0, z.value(0, 1));
      assertEquals(3.0, z.value(1, 0));
      assertEquals(4.0, z.value(1, 1));
      assertEquals(5.0, z.value(2, 0));
      assertEquals(6.0, z.value(2, 1));

      // Matrix assignment; matrix comparison
      {
        var expected = new SimpleMatrix(new double[][] {{7.0, 8.0}, {9.0, 10.0}, {11.0, 12.0}});
        z.setValue(expected);
        assertEquals(expected, z.value());
      }

      // Block assignment
      {
        var expected_block = new double[][] {{1.0}, {1.0}};
        z.block(0, 0, 2, 1).setValue(expected_block);

        var expected_result =
            new SimpleMatrix(new double[][] {{1.0, 8.0}, {1.0, 10.0}, {11.0, 12.0}});
        assertEquals(expected_result, z.value());
      }

      // Segment assignment
      {
        var expected_block = new double[][] {{1.0}, {1.0}};
        z.block(0, 0, 3, 1).segment(0, 2).setValue(expected_block);

        var expected_result =
            new SimpleMatrix(new double[][] {{1.0, 8.0}, {1.0, 10.0}, {11.0, 12.0}});
        assertEquals(expected_result, z.value());
      }
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }

  @Test
  void testSymmetricMatrix() {
    assertEquals(0, Variable.totalNativeMemoryUsage());

    try (var problem = new Problem()) {
      // Matrix zero init
      var A = problem.symmetricDecisionVariable(2);
      assertEquals(0.0, A.value(0, 0));
      assertEquals(0.0, A.value(0, 1));
      assertEquals(0.0, A.value(1, 0));
      assertEquals(0.0, A.value(1, 1));

      // Assign to lower triangle
      A.get(0, 0).setValue(1.0);
      A.get(1, 0).setValue(2.0);
      A.get(1, 1).setValue(3.0);

      // Confirm whole matrix changed
      assertEquals(1.0, A.value(0, 0));
      assertEquals(2.0, A.value(0, 1));
      assertEquals(2.0, A.value(1, 0));
      assertEquals(3.0, A.value(1, 1));
    }

    assertEquals(0, Variable.totalNativeMemoryUsage());
  }
}
