// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import org.junit.jupiter.api.Test;

class LinearPlantInversionFeedforwardTest {
  @Test
  void testCalculate() {
    Matrix<N2, N2> A = Matrix.mat(Nat.N2(), Nat.N2()).fill(1, 0, 0, 1);
    Matrix<N2, N1> B = VecBuilder.fill(0, 1);

    LinearPlantInversionFeedforward<N2, N1, N1> feedforward =
        new LinearPlantInversionFeedforward<N2, N1, N1>(A, B, 0.02);

    assertEquals(
        47.502599,
        feedforward.calculate(VecBuilder.fill(2, 2), VecBuilder.fill(3, 3)).get(0, 0),
        0.002);
  }
}
