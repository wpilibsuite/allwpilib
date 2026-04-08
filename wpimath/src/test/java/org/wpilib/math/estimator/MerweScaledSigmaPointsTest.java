// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.estimator;

import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;
import org.wpilib.math.linalg.MatBuilder;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.util.Nat;

class MerweScaledSigmaPointsTest {
  @Test
  void testZeroMeanPoints() {
    var merweScaledSigmaPoints = new MerweScaledSigmaPoints<>(Nat.N2());
    var points =
        merweScaledSigmaPoints.squareRootSigmaPoints(
            VecBuilder.fill(0, 0), MatBuilder.fill(Nat.N2(), Nat.N2(), 1, 0, 0, 1));

    assertTrue(
        points.isEqual(
            MatBuilder.fill(
                Nat.N2(),
                Nat.N5(),
                0.0,
                0.00173205,
                0.0,
                -0.00173205,
                0.0,
                0.0,
                0.0,
                0.00173205,
                0.0,
                -0.00173205),
            1E-6));
  }

  @Test
  void testNonzeroMeanPoints() {
    var merweScaledSigmaPoints = new MerweScaledSigmaPoints<>(Nat.N2());
    var points =
        merweScaledSigmaPoints.squareRootSigmaPoints(
            VecBuilder.fill(1, 2), MatBuilder.fill(Nat.N2(), Nat.N2(), 1, 0, 0, Math.sqrt(10)));

    assertTrue(
        points.isEqual(
            MatBuilder.fill(
                Nat.N2(),
                Nat.N5(),
                1.0,
                1.00173205,
                1.0,
                0.99826795,
                1.0,
                2.0,
                2.0,
                2.00547723,
                2.0,
                1.99452277),
            1E-6));
  }
}
