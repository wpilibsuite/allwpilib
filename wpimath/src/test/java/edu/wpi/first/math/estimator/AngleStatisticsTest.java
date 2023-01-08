// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import org.junit.jupiter.api.Test;

class AngleStatisticsTest {
  @Test
  void testMean() {
    // 3 states, 2 sigmas
    var sigmas =
        Matrix.mat(Nat.N3(), Nat.N2()).fill(1, 1.2, Math.toRadians(359), Math.toRadians(3), 1, 2);
    // Weights need to produce the mean of the sigmas
    var weights = new Matrix<>(Nat.N2(), Nat.N1());
    weights.fill(1.0 / sigmas.getNumCols());

    assertTrue(
        AngleStatistics.angleMean(sigmas, weights, 1)
            .isEqual(VecBuilder.fill(1.1, Math.toRadians(1), 1.5), 1e-6));
  }

  @Test
  void testResidual() {
    var first = VecBuilder.fill(1, Math.toRadians(1), 2);
    var second = VecBuilder.fill(1, Math.toRadians(359), 1);
    assertTrue(
        AngleStatistics.angleResidual(first, second, 1)
            .isEqual(VecBuilder.fill(0, Math.toRadians(2), 1), 1e-6));
  }

  @Test
  void testAdd() {
    var first = VecBuilder.fill(1, Math.toRadians(1), 2);
    var second = VecBuilder.fill(1, Math.toRadians(359), 1);
    assertTrue(AngleStatistics.angleAdd(first, second, 1).isEqual(VecBuilder.fill(2, 0, 3), 1e-6));
  }
}
