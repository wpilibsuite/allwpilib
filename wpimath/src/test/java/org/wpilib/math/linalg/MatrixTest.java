// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.linalg;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.ejml.data.SingularMatrixException;
import org.junit.jupiter.api.Test;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.numbers.N3;
import org.wpilib.math.numbers.N4;
import org.wpilib.math.util.Nat;

class MatrixTest {
  @Test
  void testMatrixMultiplication() {
    var mat1 = MatBuilder.fill(Nat.N2(), Nat.N2(), 2.0, 1.0, 0.0, 1.0);
    var mat2 = MatBuilder.fill(Nat.N2(), Nat.N2(), 3.0, 0.0, 0.0, 2.5);

    Matrix<N2, N2> result = mat1.times(mat2);

    assertEquals(result, MatBuilder.fill(Nat.N2(), Nat.N2(), 6.0, 2.5, 0.0, 2.5));

    var mat3 = MatBuilder.fill(Nat.N2(), Nat.N3(), 1.0, 3.0, 0.5, 2.0, 4.3, 1.2);
    var mat4 =
        MatBuilder.fill(
            Nat.N3(), Nat.N4(), 3.0, 1.5, 2.0, 4.5, 2.3, 1.0, 1.6, 3.1, 5.2, 2.1, 2.0, 1.0);

    Matrix<N2, N4> result2 = mat3.times(mat4);

    assertTrue(
        MatBuilder.fill(Nat.N2(), Nat.N4(), 12.5, 5.55, 7.8, 14.3, 22.13, 9.82, 13.28, 23.53)
            .isEqual(result2, 1E-9));
  }

  @Test
  void testMatrixVectorMultiplication() {
    var mat = MatBuilder.fill(Nat.N2(), Nat.N2(), 1.0, 1.0, 0.0, 1.0);

    var vec = VecBuilder.fill(3.0, 2.0);

    Matrix<N2, N1> result = mat.times(vec);
    assertEquals(VecBuilder.fill(5.0, 2.0), result);
  }

  @Test
  void testTranspose() {
    Matrix<N3, N1> vec = VecBuilder.fill(1.0, 2.0, 3.0);

    Matrix<N1, N3> transpose = vec.transpose();

    assertEquals(MatBuilder.fill(Nat.N1(), Nat.N3(), 1.0, 2.0, 3.0), transpose);
  }

  @Test
  void testSolve() {
    var mat1 = MatBuilder.fill(Nat.N2(), Nat.N2(), 1.0, 2.0, 3.0, 4.0);
    var vec1 = VecBuilder.fill(1.0, 2.0);

    var solve1 = mat1.solve(vec1);

    assertEquals(VecBuilder.fill(0.0, 0.5), solve1);

    var mat2 = MatBuilder.fill(Nat.N3(), Nat.N2(), 1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
    var vec2 = VecBuilder.fill(1.0, 2.0, 3.0);

    var solve2 = mat2.solve(vec2);

    assertEquals(VecBuilder.fill(0.0, 0.5), solve2);
  }

  @Test
  void testExp() {
    var A1 = MatBuilder.fill(Nat.N1(), Nat.N1(), 4.0);
    assertTrue(A1.exp().isEqual(MatBuilder.fill(Nat.N1(), Nat.N1(), Math.exp(4.0)), 1E-14));

    var A2 = MatBuilder.fill(Nat.N2(), Nat.N2(), 0.0, 1.0, 0.0, -0.5);
    assertTrue(A2.exp().times(A2.times(-1).exp()).isEqual(Matrix.eye(Nat.N2()), 1E-15));

    var A3 = MatBuilder.fill(Nat.N2(), Nat.N2(), 0.0, 1.0, 0.0, 10.0);
    assertTrue(A3.exp().times(A3.times(-1).exp()).isEqual(Matrix.eye(Nat.N2()), 1E-14));

    var A4 = MatBuilder.fill(Nat.N2(), Nat.N2(), 1.0, 10.0, 0.0, 0.0);
    assertTrue(A4.exp().times(A4.times(-1).exp()).isEqual(Matrix.eye(Nat.N2()), 2.5E-14));

    var A5 = MatBuilder.fill(Nat.N2(), Nat.N2(), 2.0, 3.0, 4.0, 5.0);
    assertTrue(A5.exp().times(A5.times(-1).exp()).isEqual(Matrix.eye(Nat.N2()), 1E-12));

    // Pascal matrix
    //
    //    ([0  0  0  0  0  0  0])   [1  0   0   0   0  0  0]
    //    ([1  0  0  0  0  0  0])   [1  1   0   0   0  0  0]
    //    ([0  2  0  0  0  0  0])   [1  2   1   0   0  0  0]
    // exp([0  0  3  0  0  0  0]) = [1  3   3   1   0  0  0]
    //    ([0  0  0  4  0  0  0])   [1  4   6   4   1  0  0]
    //    ([0  0  0  0  5  0  0])   [1  5  10  10   5  1  0]
    //    ([0  0  0  0  0  6  0])   [1  6  15  20  15  6  1]
    var pascal = new Matrix<>(Nat.N7(), Nat.N7());
    for (int col = 0; col < 6; ++col) {
      pascal.set(col + 1, col, col + 1);
    }
    var expectedPascal = new Matrix<>(Nat.N7(), Nat.N7());
    for (int row = 0; row < 7; ++row) {
      expectedPascal.set(row, 0, 1.0);
    }
    for (int col = 1; col < 7; ++col) {
      for (int row = col; row < 7; ++row) {
        expectedPascal.set(
            row, col, expectedPascal.get(row - 1, col - 1) + expectedPascal.get(row - 1, col));
      }
    }
    assertTrue(pascal.exp().isEqual(expectedPascal, 1E-14));
  }

  @Test
  void testInverse() {
    var mat = MatBuilder.fill(Nat.N3(), Nat.N3(), 1.0, 3.0, 2.0, 5.0, 2.0, 1.5, 0.0, 1.3, 2.5);

    var inv = mat.inv();

    assertTrue(Matrix.eye(Nat.N3()).isEqual(mat.times(inv), 1E-9));

    assertTrue(Matrix.eye(Nat.N3()).isEqual(inv.times(mat), 1E-9));
  }

  @Test
  void testUninvertableMatrix() {
    var singularMatrix = MatBuilder.fill(Nat.N2(), Nat.N2(), 2.0, 1.0, 2.0, 1.0);

    assertThrows(SingularMatrixException.class, singularMatrix::inv);
  }

  @Test
  void testMatrixScalarArithmetic() {
    var mat = MatBuilder.fill(Nat.N2(), Nat.N2(), 1.0, 2.0, 3.0, 4.0);

    assertEquals(MatBuilder.fill(Nat.N2(), Nat.N2(), 3.0, 4.0, 5.0, 6.0), mat.plus(2.0));

    assertEquals(MatBuilder.fill(Nat.N2(), Nat.N2(), 0.0, 1.0, 2.0, 3.0), mat.minus(1.0));

    assertEquals(MatBuilder.fill(Nat.N2(), Nat.N2(), 2.0, 4.0, 6.0, 8.0), mat.times(2.0));

    assertTrue(MatBuilder.fill(Nat.N2(), Nat.N2(), 0.5, 1.0, 1.5, 2.0).isEqual(mat.div(2.0), 1E-3));
  }

  @Test
  void testMatrixMatrixArithmetic() {
    var mat1 = MatBuilder.fill(Nat.N2(), Nat.N2(), 1.0, 2.0, 3.0, 4.0);

    var mat2 = MatBuilder.fill(Nat.N2(), Nat.N2(), 5.0, 6.0, 7.0, 8.0);

    assertEquals(MatBuilder.fill(Nat.N2(), Nat.N2(), -4.0, -4.0, -4.0, -4.0), mat1.minus(mat2));

    assertEquals(MatBuilder.fill(Nat.N2(), Nat.N2(), 6.0, 8.0, 10.0, 12.0), mat1.plus(mat2));
  }
}
