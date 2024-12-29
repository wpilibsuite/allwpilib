// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

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

  /**
   * Simulates a single-jointed arm and returns the final state.
   *
   * @param currentAngle The starting angle in radians.
   * @param currentVelocity The starting angular velocity in radians per second.
   * @param input The input voltage.
   * @param dt The simulation time in seconds.
   * @return The final state as a 2-vector of angle and angular velocity.
   */
  private Matrix<N2, N1> simulate(
      double currentAngle, double currentVelocity, double input, double dt) {
    final Matrix<N2, N2> A =
        new Matrix<>(Nat.N2(), Nat.N2(), new double[] {0.0, 1.0, 0.0, -kv / ka});
    final Matrix<N2, N1> B = new Matrix<>(Nat.N2(), Nat.N1(), new double[] {0.0, 1.0 / ka});

    final BiFunction<Matrix<N2, N1>, Matrix<N1, N1>, Matrix<N2, N1>> f =
        (x, u) -> {
          Matrix<N2, N1> c =
              MatBuilder.fill(
                  Nat.N2(),
                  Nat.N1(),
                  0.0,
                  Math.signum(x.get(1, 0)) * (-ks / ka) - (kg / ka) * Math.cos(x.get(0, 0)));
          return A.times(x).plus(B.times(u)).plus(c);
        };

    return NumericalIntegration.rk4(
        f,
        MatBuilder.fill(Nat.N2(), Nat.N1(), currentAngle, currentVelocity),
        MatBuilder.fill(Nat.N1(), Nat.N1(), input),
        dt);
  }

  /**
   * Calculates a feedforward voltage using overload taking angle, two angular velocities, and dt;
   * then simulates an arm using that voltage to verify correctness.
   *
   * @param currentAngle The starting angle in radians.
   * @param currentVelocity The starting angular velocity in radians per second.
   * @param input The input voltage.
   * @param dt The simulation time in seconds.
   */
  private void calculateAndSimulate(
      double currentAngle, double currentVelocity, double nextVelocity, double dt) {
    final double input =
        m_armFF.calculateWithVelocities(currentAngle, currentVelocity, nextVelocity);
    assertEquals(nextVelocity, simulate(currentAngle, currentVelocity, input, dt).get(1, 0), 1e-12);
  }

  @Test
  void testCalculate() {
    // calculate(angle, angular velocity)
    assertEquals(0.5, m_armFF.calculate(Math.PI / 3, 0.0), 0.002);
    assertEquals(2.5, m_armFF.calculate(Math.PI / 3, 1.0), 0.002);

    // calculate(currentAngle, currentVelocity, nextAngle, dt)
    calculateAndSimulate(Math.PI / 3, 1.0, 1.05, 0.020);
    calculateAndSimulate(Math.PI / 3, 1.0, 0.95, 0.020);
    calculateAndSimulate(-Math.PI / 3, 1.0, 1.05, 0.020);
    calculateAndSimulate(-Math.PI / 3, 1.0, 0.95, 0.020);
  }

  @Test
  void testAchievableVelocity() {
    assertEquals(6, m_armFF.maxAchievableVelocity(12, Math.PI / 3, 1), 0.002);
    assertEquals(-9, m_armFF.minAchievableVelocity(11.5, Math.PI / 3, 1), 0.002);
  }

  @Test
  void testAchievableAcceleration() {
    assertEquals(4.75, m_armFF.maxAchievableAcceleration(12, Math.PI / 3, 1), 0.002);
    assertEquals(6.75, m_armFF.maxAchievableAcceleration(12, Math.PI / 3, -1), 0.002);
    assertEquals(-7.25, m_armFF.minAchievableAcceleration(12, Math.PI / 3, 1), 0.002);
    assertEquals(-5.25, m_armFF.minAchievableAcceleration(12, Math.PI / 3, -1), 0.002);
  }

  @Test
  void testNegativeGains() {
    assertAll(
        () ->
            assertThrows(IllegalArgumentException.class, () -> new ArmFeedforward(ks, kg, -kv, ka)),
        () ->
            assertThrows(
                IllegalArgumentException.class, () -> new ArmFeedforward(ks, kg, kv, -ka)));
  }
}
