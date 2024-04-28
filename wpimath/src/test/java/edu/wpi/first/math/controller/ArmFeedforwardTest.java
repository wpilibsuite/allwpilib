// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.NumericalIntegration;
import java.util.function.BiFunction;
import org.junit.jupiter.api.Test;

class ArmFeedforwardTest {
  private static final double ks = 0.5;
  private static final double kg = 1;
  private static final double kv = 1.5;
  private static final double ka = 2;
  private final ArmFeedforward m_armFF = new ArmFeedforward(ks, kg, kv, ka);

  @Test
  void testCalculate() {
    // calculate(angle, angular velocity, dt)
    {
      assertEquals(0.5, m_armFF.calculate(Math.PI / 3, 0), 0.002);
      assertEquals(2.5, m_armFF.calculate(Math.PI / 3, 1), 0.002);
    }

    // calculate(angle, angular velocity, angular acceleration, dt)
    {
      assertEquals(6.5, m_armFF.calculate(Math.PI / 3, 1, 2), 0.002);
      assertEquals(2.5, m_armFF.calculate(Math.PI / 3, -1, 2), 0.002);
    }

    // calculate(currentAngle, currentVelocity, nextAngle, dt)
    {
      final double currentAngleRadians = Math.PI / 3;
      final double currentVelocityRadPerSec = 1.0;
      final double nextVelocityRadPerSec = 1.05;
      final double dtSeconds = 0.020;

      final double u =
          m_armFF.calculate(
              currentAngleRadians, currentVelocityRadPerSec, nextVelocityRadPerSec, dtSeconds);

      final Matrix<N2, N2> A =
          new Matrix<>(Nat.N2(), Nat.N2(), new double[] {0.0, 1.0, 0.0, -kv / ka});
      final Matrix<N2, N1> B = new Matrix<>(Nat.N2(), Nat.N1(), new double[] {0.0, 1.0 / ka});

      final BiFunction<Matrix<N2, N1>, Matrix<N1, N1>, Matrix<N2, N1>> f =
          (x_, u_) -> {
            Matrix<N2, N1> c =
                MatBuilder.fill(
                    Nat.N2(),
                    Nat.N1(),
                    0.0,
                    Math.signum(x_.get(1, 0)) * (-ks / ka) - (kg / ka) * Math.cos(x_.get(0, 0)));
            return A.times(x_).plus(B.times(u_)).plus(c);
          };

      final Matrix<N2, N1> actual_x_k1 =
          NumericalIntegration.rk4(
              f,
              MatBuilder.fill(Nat.N2(), Nat.N1(), currentAngleRadians, currentVelocityRadPerSec),
              MatBuilder.fill(Nat.N1(), Nat.N1(), u),
              dtSeconds);

      assertEquals(nextVelocityRadPerSec, actual_x_k1.get(1, 0), 1e-12);
    }
  }

  @Test
  void testAcheviableVelocity() {
    assertEquals(6, m_armFF.maxAchievableVelocity(12, Math.PI / 3, 1), 0.002);
    assertEquals(-9, m_armFF.minAchievableVelocity(11.5, Math.PI / 3, 1), 0.002);
  }

  @Test
  void testAcheviableAcceleration() {
    assertEquals(4.75, m_armFF.maxAchievableAcceleration(12, Math.PI / 3, 1), 0.002);
    assertEquals(6.75, m_armFF.maxAchievableAcceleration(12, Math.PI / 3, -1), 0.002);
    assertEquals(-7.25, m_armFF.minAchievableAcceleration(12, Math.PI / 3, 1), 0.002);
    assertEquals(-5.25, m_armFF.minAchievableAcceleration(12, Math.PI / 3, -1), 0.002);
  }
}
