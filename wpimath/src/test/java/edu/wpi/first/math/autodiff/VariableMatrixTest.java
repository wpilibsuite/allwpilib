// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.autodiff;

import static edu.wpi.first.math.MatrixAssertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;

class VariableMatrixTest {
  @Test
  void testConstructFromSimpleMatrix() {
    var mat = new VariableMatrix(new double[][] {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}});

    var expected = new SimpleMatrix(new double[][] {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}});
    assertEquals(expected, mat.value());
  }

  @Test
  void testAssignmentToDefault() {
    var mat = new VariableMatrix();

    assertEquals(0, mat.rows());
    assertEquals(0, mat.cols());

    mat = new VariableMatrix(2, 2);

    assertEquals(2, mat.rows());
    assertEquals(2, mat.cols());
    assertEquals(0.0, mat.get(0, 0).value());
    assertEquals(0.0, mat.get(0, 1).value());
    assertEquals(0.0, mat.get(1, 0).value());
    assertEquals(0.0, mat.get(1, 1).value());

    mat.set(0, 0, 1.0);
    mat.set(0, 1, 2.0);
    mat.set(1, 0, 3.0);
    mat.set(1, 1, 4.0);

    assertEquals(1.0, mat.get(0, 0).value());
    assertEquals(2.0, mat.get(0, 1).value());
    assertEquals(3.0, mat.get(1, 0).value());
    assertEquals(4.0, mat.get(1, 1).value());
  }

  @Test
  void testBlockMemberFunction() {
    var A = new VariableMatrix(new double[][] {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0}});

    // Block assignment
    A.block(1, 1, 2, 2).set(new SimpleMatrix(new double[][] {{10.0, 11.0}, {12.0, 13.0}}));

    var expected1 =
        new SimpleMatrix(new double[][] {{1.0, 2.0, 3.0}, {4.0, 10.0, 11.0}, {7.0, 12.0, 13.0}});
    assertEquals(expected1, A.value());

    // Block-of-block assignment
    A.block(1, 1, 2, 2).block(1, 1, 1, 1).set(0, 0, 14.0);

    var expected2 =
        new SimpleMatrix(new double[][] {{1.0, 2.0, 3.0}, {4.0, 10.0, 11.0}, {7.0, 12.0, 14.0}});
    assertEquals(expected2, A.value());
  }

  @Test
  void testSlicing() {
    var mat =
        new VariableMatrix(
            new double[][] {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}});
    assertEquals(4, mat.rows());
    assertEquals(4, mat.cols());

    // Single-arg index operator on full matrix
    for (int i = 0; i < mat.rows() * mat.cols(); ++i) {
      assertEquals(i + 1, mat.get(i).value());
    }

    // Slice from start
    {
      var s = mat.get(new Slice(1, Slice.__), new Slice(2, Slice.__));
      assertEquals(3, s.rows());
      assertEquals(2, s.cols());
      // Single-arg index operator on forward slice
      assertEquals(7.0, s.get(0).value());
      assertEquals(8.0, s.get(1).value());
      assertEquals(11.0, s.get(2).value());
      assertEquals(12.0, s.get(3).value());
      assertEquals(15.0, s.get(4).value());
      assertEquals(16.0, s.get(5).value());
      // Double-arg index operator on forward slice
      assertEquals(7.0, s.get(0, 0).value());
      assertEquals(8.0, s.get(0, 1).value());
      assertEquals(11.0, s.get(1, 0).value());
      assertEquals(12.0, s.get(1, 1).value());
      assertEquals(15.0, s.get(2, 0).value());
      assertEquals(16.0, s.get(2, 1).value());
    }

    // Slice from end
    {
      var s = mat.get(new Slice(-1, Slice.__), new Slice(-2, Slice.__));
      assertEquals(1, s.rows());
      assertEquals(2, s.cols());
      // Single-arg index operator on reverse slice
      assertEquals(15.0, s.get(0).value());
      assertEquals(16.0, s.get(1).value());
      // Double-arg index operator on reverse slice
      assertEquals(15.0, s.get(0, 0).value());
      assertEquals(16.0, s.get(0, 1).value());
    }

    // Slice from start with step of 2
    {
      var s = mat.get(Slice.__, new Slice(Slice.__, Slice.__, 2));
      assertEquals(4, s.rows());
      assertEquals(2, s.cols());
      assertEquals(
          new SimpleMatrix(new double[][] {{1.0, 3.0}, {5.0, 7.0}, {9.0, 11.0}, {13.0, 15.0}}),
          s.value());
    }

    // Slice from end with negative step for row and column
    {
      var s = mat.get(new Slice(Slice.__, Slice.__, -1), new Slice(Slice.__, Slice.__, -2));
      assertEquals(4, s.rows());
      assertEquals(2, s.cols());
      assertEquals(
          new SimpleMatrix(new double[][] {{16.0, 14.0}, {12.0, 10.0}, {8.0, 6.0}, {4.0, 2.0}}),
          s.value());
    }

    // Slice from start and column -1
    {
      var s = mat.get(new Slice(1, Slice.__), -1);
      assertEquals(3, s.rows());
      assertEquals(1, s.cols());
      assertEquals(new SimpleMatrix(new double[][] {{8.0}, {12.0}, {16.0}}), s.value());
    }

    // Slice from start and column -2
    {
      var s = mat.get(new Slice(1, Slice.__), -2);
      assertEquals(3, s.rows());
      assertEquals(1, s.cols());
      assertEquals(new SimpleMatrix(new double[][] {{7.0}, {11.0}, {15.0}}), s.value());
    }
  }

  @Test
  void testSubslicing() {
    var A = new VariableMatrix(new double[][] {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0}});

    // Block assignment
    assertEquals(2, A.get(new Slice(1, 3), new Slice(1, 3)).rows());
    assertEquals(2, A.get(new Slice(1, 3), new Slice(1, 3)).cols());
    A.get(new Slice(1, 3), new Slice(1, 3))
        .set(new SimpleMatrix(new double[][] {{10.0, 11.0}, {12.0, 13.0}}));

    var expected1 =
        new SimpleMatrix(new double[][] {{1.0, 2.0, 3.0}, {4.0, 10.0, 11.0}, {7.0, 12.0, 13.0}});
    assertEquals(expected1, A.value());

    // Block-of-block assignment
    assertEquals(
        1,
        A.get(new Slice(1, 3), new Slice(1, 3))
            .get(new Slice(1, Slice.__), new Slice(1, Slice.__))
            .rows());
    assertEquals(
        1,
        A.get(new Slice(1, 3), new Slice(1, 3))
            .get(new Slice(1, Slice.__), new Slice(1, Slice.__))
            .cols());
    A.get(new Slice(1, 3), new Slice(1, 3))
        .get(new Slice(1, Slice.__), new Slice(1, Slice.__))
        .set(14.0);

    var expected2 =
        new SimpleMatrix(new double[][] {{1.0, 2.0, 3.0}, {4.0, 10.0, 11.0}, {7.0, 12.0, 14.0}});
    assertEquals(expected2, A.value());
  }

  @SuppressWarnings("PMD.UnusedLocalVariable")
  @Test
  void testIterators() {
    var A = new VariableMatrix(new double[][] {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0}});

    // VariableMatrix iterator
    int distance = 0;
    for (var elem : A) {
      ++distance;
    }
    assertEquals(9, distance);

    int i = 1;
    for (var elem : A) {
      assertEquals(i, elem.value());
      ++i;
    }

    var sub_A = A.block(2, 1, 1, 2);

    // VariableBlock iterator
    distance = 0;
    for (var elem : sub_A) {
      ++distance;
    }
    assertEquals(2, distance);

    i = 8;
    for (var elem : sub_A) {
      assertEquals(i, elem.value());
      ++i;
    }
  }

  @Test
  void testValue() {
    var A = new VariableMatrix(new double[][] {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0}});
    var expected =
        new SimpleMatrix(new double[][] {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0}});

    // Full matrix
    assertEquals(expected, A.value());
    assertEquals(4.0, A.value(3));
    assertEquals(2.0, A.T().value(3));

    // Block
    assertEquals(expected.extractMatrix(1, 3, 1, 3), A.block(1, 1, 2, 2).value());
    assertEquals(8.0, A.block(1, 1, 2, 2).value(2));
    assertEquals(6.0, A.T().block(1, 1, 2, 2).value(2));

    // Slice
    assertEquals(
        expected.extractMatrix(1, 3, 1, 3), A.get(new Slice(1, 3), new Slice(1, 3)).value());
    assertEquals(8.0, A.get(new Slice(1, 3), new Slice(1, 3)).value(2));
    assertEquals(6.0, A.get(new Slice(1, 3), new Slice(1, 3)).T().value(2));

    // Block-of-block
    assertEquals(
        expected.extractMatrix(1, 3, 1, 3).extractMatrix(0, 2, 1, 2),
        A.block(1, 1, 2, 2).block(0, 1, 2, 1).value());
    assertEquals(9.0, A.block(1, 1, 2, 2).block(0, 1, 2, 1).value(1));
    assertEquals(9.0, A.block(1, 1, 2, 2).T().block(0, 1, 2, 1).value(1));

    // Slice-of-slice
    assertEquals(
        expected.extractMatrix(1, 3, 1, 3).extractMatrix(0, 2, 1, 2),
        A.get(new Slice(1, 3), new Slice(1, 3)).get(Slice.__, new Slice(1, Slice.__)).value());
    assertEquals(
        9.0,
        A.get(new Slice(1, 3), new Slice(1, 3)).get(Slice.__, new Slice(1, Slice.__)).value(1));
    assertEquals(
        9.0,
        A.get(new Slice(1, 3), new Slice(1, 3)).T().get(Slice.__, new Slice(1, Slice.__)).value(1));
  }

  @Test
  void testCwiseTransform() {
    // VariableMatrix cwiseTransform
    var A = new VariableMatrix(new double[][] {{-2.0, -3.0, -4.0}, {-5.0, -6.0, -7.0}});

    var result1 = A.cwiseTransform(Variable::abs);
    var expected1 = new SimpleMatrix(new double[][] {{2.0, 3.0, 4.0}, {5.0, 6.0, 7.0}});

    // Don't modify original matrix
    assertEquals(expected1.scale(-1.0), A.value());

    assertEquals(expected1, result1.value());

    // VariableBlock cwiseTransform
    var sub_A = A.block(0, 0, 2, 2);

    var result2 = sub_A.cwiseTransform(Variable::abs);
    var expected2 = new SimpleMatrix(new double[][] {{2.0, 3.0}, {5.0, 6.0}});

    // Don't modify original matrix
    assertEquals(expected1.scale(-1.0), A.value());
    assertEquals(expected2.scale(-1.0), sub_A.value());

    assertEquals(expected2, result2.value());
  }

  @Test
  void testZeroStaticFunction() {
    var A = VariableMatrix.zero(2, 3);

    for (var elem : A) {
      assertEquals(0.0, elem.value());
    }
  }

  @Test
  void testOnesStaticFunction() {
    var A = VariableMatrix.ones(2, 3);

    for (var elem : A) {
      assertEquals(1.0, elem.value());
    }
  }

  @Test
  void testCwiseReduce() {
    var A = new VariableMatrix(new double[][] {{2.0, 3.0, 4.0}, {5.0, 6.0, 7.0}});
    var B = new VariableMatrix(new double[][] {{8.0, 9.0, 10.0}, {11.0, 12.0, 13.0}});
    var result = VariableMatrix.cwiseReduce(A, B, (Variable x, Variable y) -> x.times(y));

    var expected = new SimpleMatrix(new double[][] {{16.0, 27.0, 40.0}, {55.0, 72.0, 91.0}});
    assertEquals(expected, result.value());
  }

  @Test
  void testBlockFreeFunction() {
    var A = new VariableMatrix(new double[][] {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}});
    var B = new VariableMatrix(new double[][] {{7.0}, {8.0}});

    var mat1 = VariableMatrix.block(new VariableMatrix[][] {{A, B}});
    var expected1 = new SimpleMatrix(new double[][] {{1.0, 2.0, 3.0, 7.0}, {4.0, 5.0, 6.0, 8.0}});
    assertEquals(2, mat1.rows());
    assertEquals(4, mat1.cols());
    assertEquals(expected1, mat1.value());

    var C = new VariableMatrix(new double[][] {{9.0, 10.0, 11.0, 12.0}});

    var mat2 = VariableMatrix.block(new VariableMatrix[][] {{A, B}, {C}});
    var expected2 =
        new SimpleMatrix(
            new double[][] {{1.0, 2.0, 3.0, 7.0}, {4.0, 5.0, 6.0, 8.0}, {9.0, 10.0, 11.0, 12.0}});
    assertEquals(3, mat2.rows());
    assertEquals(4, mat2.cols());
    assertEquals(expected2, mat2.value());
  }

  private void assertSolve(SimpleMatrix A, SimpleMatrix B) {
    var slp_A = new VariableMatrix(A);
    var slp_B = new VariableMatrix(B);
    var slp_X = VariableMatrix.solve(slp_A, slp_B);

    assertEquals(A.getNumCols(), slp_X.rows());
    assertEquals(B.getNumCols(), slp_X.cols());
    assertTrue(slp_A.value().mult(slp_X.value()).minus(slp_B.value()).normF() < 1e-12);
  }

  @Test
  void testSolveFreeFunction() {
    // 1x1 special case
    assertSolve(new SimpleMatrix(new double[][] {{2.0}}), new SimpleMatrix(new double[][] {{5.0}}));

    // 2x2 special case
    assertSolve(
        new SimpleMatrix(new double[][] {{1.0, 2.0}, {3.0, 4.0}}),
        new SimpleMatrix(new double[][] {{5.0}, {6.0}}));

    // 3x3 special case
    assertSolve(
        new SimpleMatrix(new double[][] {{1.0, 2.0, 3.0}, {-4.0, -5.0, 6.0}, {7.0, 8.0, 9.0}}),
        new SimpleMatrix(new double[][] {{10.0}, {11.0}, {12.0}}));

    // 4x4 special case
    assertSolve(
        new SimpleMatrix(
            new double[][] {
              {1.0, 2.0, 3.0, -4.0},
              {-5.0, 6.0, 7.0, 8.0},
              {9.0, 10.0, 11.0, 12.0},
              {13.0, 14.0, 15.0, 16.0}
            }),
        new SimpleMatrix(new double[][] {{17.0}, {18.0}, {19.0}, {20.0}}));

    // 5x5 general case
    assertSolve(
        new SimpleMatrix(
            new double[][] {
              {1.0, 2.0, 3.0, -4.0, 5.0},
              {-5.0, 6.0, 7.0, 8.0, 9.0},
              {9.0, 10.0, 11.0, 12.0, 13.0},
              {13.0, 14.0, 15.0, 16.0, 17.0},
              {17.0, 18.0, 19.0, 20.0, 21.0}
            }),
        new SimpleMatrix(new double[][] {{21.0}, {22.0}, {23.0}, {24.0}, {25.0}}));
  }
}
