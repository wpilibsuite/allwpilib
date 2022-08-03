// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.math.numbers.N4;
import org.ejml.data.SingularMatrixException;
import org.junit.jupiter.api.Test;

class MatrixTest {
  @Test
  void testMatrixMultiplication() {
    var mat1 = Matrix.mat(Nat.N2(), Nat.N2()).fill(2.0, 1.0, 0.0, 1.0);
    var mat2 = Matrix.mat(Nat.N2(), Nat.N2()).fill(3.0, 0.0, 0.0, 2.5);

    Matrix<N2, N2> result = mat1.times(mat2);

    assertEquals(result, Matrix.mat(Nat.N2(), Nat.N2()).fill(6.0, 2.5, 0.0, 2.5));

    var mat3 = Matrix.mat(Nat.N2(), Nat.N3()).fill(1.0, 3.0, 0.5, 2.0, 4.3, 1.2);
    var mat4 =
        Matrix.mat(Nat.N3(), Nat.N4())
            .fill(3.0, 1.5, 2.0, 4.5, 2.3, 1.0, 1.6, 3.1, 5.2, 2.1, 2.0, 1.0);

    Matrix<N2, N4> result2 = mat3.times(mat4);

    assertTrue(
        Matrix.mat(Nat.N2(), Nat.N4())
            .fill(12.5, 5.55, 7.8, 14.3, 22.13, 9.82, 13.28, 23.53)
            .isEqual(result2, 1E-9));
  }

  @Test
  void testMatrixVectorMultiplication() {
    var mat = Matrix.mat(Nat.N2(), Nat.N2()).fill(1.0, 1.0, 0.0, 1.0);

    var vec = VecBuilder.fill(3.0, 2.0);

    Matrix<N2, N1> result = mat.times(vec);
    assertEquals(VecBuilder.fill(5.0, 2.0), result);
  }

  @Test
  void testTranspose() {
    Matrix<N3, N1> vec = VecBuilder.fill(1.0, 2.0, 3.0);

    Matrix<N1, N3> transpose = vec.transpose();

    assertEquals(Matrix.mat(Nat.N1(), Nat.N3()).fill(1.0, 2.0, 3.0), transpose);
  }

  @Test
  void testSolve() {
    var mat1 = Matrix.mat(Nat.N2(), Nat.N2()).fill(1.0, 2.0, 3.0, 4.0);
    var vec1 = VecBuilder.fill(1.0, 2.0);

    var solve1 = mat1.solve(vec1);

    assertEquals(VecBuilder.fill(0.0, 0.5), solve1);

    var mat2 = Matrix.mat(Nat.N3(), Nat.N2()).fill(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
    var vec2 = VecBuilder.fill(1.0, 2.0, 3.0);

    var solve2 = mat2.solve(vec2);

    assertEquals(VecBuilder.fill(0.0, 0.5), solve2);
  }

  @Test
  void testInverse() {
    var mat = Matrix.mat(Nat.N3(), Nat.N3()).fill(1.0, 3.0, 2.0, 5.0, 2.0, 1.5, 0.0, 1.3, 2.5);

    var inv = mat.inv();

    assertTrue(Matrix.eye(Nat.N3()).isEqual(mat.times(inv), 1E-9));

    assertTrue(Matrix.eye(Nat.N3()).isEqual(inv.times(mat), 1E-9));
  }

  @Test
  void testUninvertableMatrix() {
    var singularMatrix = Matrix.mat(Nat.N2(), Nat.N2()).fill(2.0, 1.0, 2.0, 1.0);

    assertThrows(SingularMatrixException.class, singularMatrix::inv);
  }

  @Test
  void testMatrixScalarArithmetic() {
    var mat = Matrix.mat(Nat.N2(), Nat.N2()).fill(1.0, 2.0, 3.0, 4.0);

    assertEquals(Matrix.mat(Nat.N2(), Nat.N2()).fill(3.0, 4.0, 5.0, 6.0), mat.plus(2.0));

    assertEquals(Matrix.mat(Nat.N2(), Nat.N2()).fill(0.0, 1.0, 2.0, 3.0), mat.minus(1.0));

    assertEquals(Matrix.mat(Nat.N2(), Nat.N2()).fill(2.0, 4.0, 6.0, 8.0), mat.times(2.0));

    assertTrue(Matrix.mat(Nat.N2(), Nat.N2()).fill(0.5, 1.0, 1.5, 2.0).isEqual(mat.div(2.0), 1E-3));
  }

  @Test
  void testMatrixMatrixArithmetic() {
    var mat1 = Matrix.mat(Nat.N2(), Nat.N2()).fill(1.0, 2.0, 3.0, 4.0);

    var mat2 = Matrix.mat(Nat.N2(), Nat.N2()).fill(5.0, 6.0, 7.0, 8.0);

    assertEquals(Matrix.mat(Nat.N2(), Nat.N2()).fill(-4.0, -4.0, -4.0, -4.0), mat1.minus(mat2));

    assertEquals(Matrix.mat(Nat.N2(), Nat.N2()).fill(6.0, 8.0, 10.0, 12.0), mat1.plus(mat2));
  }

  @Test
  void testMatrixExponential() {
    var matrix = Matrix.eye(Nat.N2());
    var result = matrix.exp();

    assertTrue(result.isEqual(Matrix.mat(Nat.N2(), Nat.N2()).fill(Math.E, 0, 0, Math.E), 1E-9));

    matrix = Matrix.mat(Nat.N2(), Nat.N2()).fill(1, 2, 3, 4);
    result = matrix.times(0.01).exp();

    assertTrue(
        result.isEqual(
            Matrix.mat(Nat.N2(), Nat.N2()).fill(1.01035625, 0.02050912, 0.03076368, 1.04111993),
            1E-8));
  }
}
