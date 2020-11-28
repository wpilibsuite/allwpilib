/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.estimator;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.VecBuilder;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class AngleStatisticsTest {
  @Test
  public void testMean() {
    // 3 states, 2 sigmas
    var sigmas = Matrix.mat(Nat.N3(), Nat.N2()).fill(
            1, 1.2,
            Math.toRadians(359), Math.toRadians(3),
            1, 2
    );
    // Weights need to produce the mean of the sigmas
    var weights = new Matrix<>(Nat.N2(), Nat.N1());
    weights.fill(1.0 / sigmas.getNumCols());

    assertTrue(AngleStatistics.angleMean(sigmas, weights, 1).isEqual(
            VecBuilder.fill(1.1, Math.toRadians(1), 1.5), 1e-6));
  }

  @Test
  public void testResidual() {
    var first = VecBuilder.fill(1, Math.toRadians(1), 2);
    var second = VecBuilder.fill(1, Math.toRadians(359), 1);
    assertTrue(AngleStatistics.angleResidual(first, second, 1).isEqual(
            VecBuilder.fill(0, Math.toRadians(2), 1), 1e-6));
  }

  @Test
  public void testAdd() {
    var first = VecBuilder.fill(1, Math.toRadians(1), 2);
    var second = VecBuilder.fill(1, Math.toRadians(359), 1);
    assertTrue(AngleStatistics.angleAdd(first, second, 1).isEqual(VecBuilder.fill(2, 0, 3), 1e-6));
  }

  @Test
  public void testNormalize() {
    assertEquals(AngleStatistics.normalizeAngle(Math.toRadians(-2000)), Math.toRadians(160), 1e-6);
    assertEquals(AngleStatistics.normalizeAngle(Math.toRadians(358)), Math.toRadians(-2), 1e-6);
    assertEquals(AngleStatistics.normalizeAngle(Math.toRadians(360)), 0, 1e-6);
  }
}
