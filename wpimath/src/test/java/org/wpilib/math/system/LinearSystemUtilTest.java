// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.system;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;
import org.wpilib.UtilityClassTest;
import org.wpilib.math.linalg.MatBuilder;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.util.Nat;

class LinearSystemUtilTest extends UtilityClassTest<LinearSystemUtil> {
  LinearSystemUtilTest() {
    super(LinearSystemUtil.class);
  }

  @Test
  void testIsStabilizable() {
    Matrix<N2, N2> A;
    Matrix<N2, N1> B = VecBuilder.fill(0, 1);

    // First eigenvalue is uncontrollable and unstable.
    // Second eigenvalue is controllable and stable.
    A = MatBuilder.fill(Nat.N2(), Nat.N2(), 1.2, 0, 0, 0.5);
    assertFalse(LinearSystemUtil.isStabilizable(A, B));

    // First eigenvalue is uncontrollable and marginally stable.
    // Second eigenvalue is controllable and stable.
    A = MatBuilder.fill(Nat.N2(), Nat.N2(), 1, 0, 0, 0.5);
    assertFalse(LinearSystemUtil.isStabilizable(A, B));

    // First eigenvalue is uncontrollable and stable.
    // Second eigenvalue is controllable and stable.
    A = MatBuilder.fill(Nat.N2(), Nat.N2(), 0.2, 0, 0, 0.5);
    assertTrue(LinearSystemUtil.isStabilizable(A, B));

    // First eigenvalue is uncontrollable and stable.
    // Second eigenvalue is controllable and unstable.
    A = MatBuilder.fill(Nat.N2(), Nat.N2(), 0.2, 0, 0, 1.2);
    assertTrue(LinearSystemUtil.isStabilizable(A, B));
  }

  @Test
  void testIsDetectable() {
    Matrix<N2, N2> A;
    Matrix<N1, N2> C = MatBuilder.fill(Nat.N1(), Nat.N2(), 0, 1);

    // First eigenvalue is unobservable and unstable.
    // Second eigenvalue is observable and stable.
    A = MatBuilder.fill(Nat.N2(), Nat.N2(), 1.2, 0, 0, 0.5);
    assertFalse(LinearSystemUtil.isDetectable(A, C));

    // First eigenvalue is unobservable and marginally stable.
    // Second eigenvalue is observable and stable.
    A = MatBuilder.fill(Nat.N2(), Nat.N2(), 1, 0, 0, 0.5);
    assertFalse(LinearSystemUtil.isDetectable(A, C));

    // First eigenvalue is unobservable and stable.
    // Second eigenvalue is observable and stable.
    A = MatBuilder.fill(Nat.N2(), Nat.N2(), 0.2, 0, 0, 0.5);
    assertTrue(LinearSystemUtil.isDetectable(A, C));

    // First eigenvalue is unobservable and stable.
    // Second eigenvalue is observable and unstable.
    A = MatBuilder.fill(Nat.N2(), Nat.N2(), 0.2, 0, 0, 1.2);
    assertTrue(LinearSystemUtil.isDetectable(A, C));
  }
}
