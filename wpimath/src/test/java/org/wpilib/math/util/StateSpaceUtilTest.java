// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.util;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;
import org.wpilib.UtilityClassTest;
import org.wpilib.math.linalg.MatBuilder;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N2;

class StateSpaceUtilTest extends UtilityClassTest<StateSpaceUtil> {
  StateSpaceUtilTest() {
    super(StateSpaceUtil.class);
  }

  @Test
  void testCostArray() {
    var mat = StateSpaceUtil.costMatrix(VecBuilder.fill(1.0, 2.0, 3.0));

    assertEquals(1.0, mat.get(0, 0), 1e-3);
    assertEquals(0.0, mat.get(0, 1), 1e-3);
    assertEquals(0.0, mat.get(0, 2), 1e-3);
    assertEquals(0.0, mat.get(1, 0), 1e-3);
    assertEquals(1.0 / 4.0, mat.get(1, 1), 1e-3);
    assertEquals(0.0, mat.get(1, 2), 1e-3);
    assertEquals(0.0, mat.get(0, 2), 1e-3);
    assertEquals(0.0, mat.get(1, 2), 1e-3);
    assertEquals(1.0 / 9.0, mat.get(2, 2), 1e-3);
  }

  @Test
  void testCovArray() {
    var mat = StateSpaceUtil.covarianceMatrix(Nat.N3(), VecBuilder.fill(1.0, 2.0, 3.0));

    assertEquals(1.0, mat.get(0, 0), 1e-3);
    assertEquals(0.0, mat.get(0, 1), 1e-3);
    assertEquals(0.0, mat.get(0, 2), 1e-3);
    assertEquals(0.0, mat.get(1, 0), 1e-3);
    assertEquals(4.0, mat.get(1, 1), 1e-3);
    assertEquals(0.0, mat.get(1, 2), 1e-3);
    assertEquals(0.0, mat.get(0, 2), 1e-3);
    assertEquals(0.0, mat.get(1, 2), 1e-3);
    assertEquals(9.0, mat.get(2, 2), 1e-3);
  }

  @Test
  void testMatrixExp() {
    Matrix<N2, N2> wrappedMatrix = Matrix.eye(Nat.N2());
    var wrappedResult = wrappedMatrix.exp();

    assertTrue(
        wrappedResult.isEqual(MatBuilder.fill(Nat.N2(), Nat.N2(), Math.E, 0, 0, Math.E), 1E-9));

    var matrix = MatBuilder.fill(Nat.N2(), Nat.N2(), 1, 2, 3, 4);
    wrappedResult = matrix.times(0.01).exp();

    assertTrue(
        wrappedResult.isEqual(
            MatBuilder.fill(Nat.N2(), Nat.N2(), 1.01035625, 0.02050912, 0.03076368, 1.04111993),
            1E-8));
  }

  @Test
  void testDesaturateInputVector() {
    final var vec1 = MatBuilder.fill(Nat.N2(), Nat.N1(), 10.0, 12.0);
    assertEquals(vec1, StateSpaceUtil.desaturateInputVector(vec1, 12.0));
    assertEquals(
        MatBuilder.fill(Nat.N2(), Nat.N1(), 25.0 / 3.0, 10.0),
        StateSpaceUtil.desaturateInputVector(vec1, 10.0));

    final var vec2 = MatBuilder.fill(Nat.N2(), Nat.N1(), 10.0, -12.0);
    assertEquals(vec2, StateSpaceUtil.desaturateInputVector(vec2, 12.0));
    assertEquals(
        MatBuilder.fill(Nat.N2(), Nat.N1(), 25.0 / 3.0, -10.0),
        StateSpaceUtil.desaturateInputVector(vec2, 10.0));

    final var vec3 = MatBuilder.fill(Nat.N2(), Nat.N1(), 0.0, 0.0);
    assertEquals(vec3, StateSpaceUtil.desaturateInputVector(vec3, 12.0));
  }
}
