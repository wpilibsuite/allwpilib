/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;

import static org.junit.jupiter.api.Assertions.assertEquals;

class LinearPlantInversionFeedforwardTest {
  @SuppressWarnings("LocalVariableName")
  @Test
  void testCalculate() {
    Matrix<N2, N2> A = Matrix.mat(Nat.N2(), Nat.N2()).fill(1, 0, 0, 1);
    Matrix<N2, N1> B = VecBuilder.fill(0, 1);

    LinearPlantInversionFeedforward<N2, N1, N1> feedforward =
            new LinearPlantInversionFeedforward<N2, N1, N1>(A, B, 0.02);

    assertEquals(47.502599, feedforward.calculate(
            VecBuilder.fill(2, 2),
            VecBuilder.fill(3, 3)).get(0, 0),
            0.002);
  }
}
