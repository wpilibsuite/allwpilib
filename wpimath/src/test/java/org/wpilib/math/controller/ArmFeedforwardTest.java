// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import java.util.function.BiFunction;
import org.junit.jupiter.api.Test;
import org.wpilib.math.linalg.MatBuilder;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.system.NumericalIntegration;
import org.wpilib.math.util.Nat;

class ArmFeedforwardTest {
  /**
   * Simulates a single-jointed arm and returns the final state.
   *
   * @param ks The static gain, in volts.
   * @param kv The velocity gain, in volt seconds per radian.
   * @param ka The acceleration gain, in volt seconds² per radian.
   * @param kg The gravity gain, in volts.
   * @param currentAngle The starting angle in radians.
   * @param currentVelocity The starting angular velocity in radians per second.
   * @param input The input voltage.
   * @param dt The simulation time in seconds.
   * @return The final state as a 2-vector of angle and angular velocity.
   */
  private Matrix<N2, N1> simulate(
      double ks,
      double kv,
      double ka,
      double kg,
      double currentAngle,
      double currentVelocity,
      double input,
      double dt) {
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
   * @param armFF The feedforward object.
   * @param ks The static gain, in volts.
   * @param kv The velocity gain, in volt seconds per radian.
   * @param ka The acceleration gain, in volt seconds² per radian.
   * @param kg The gravity gain, in volts.
   * @param currentAngle The starting angle in radians.
   * @param currentVelocity The starting angular velocity in radians per second.
   * @param input The input voltage.
   * @param dt The simulation time in seconds.
   */
  private void calculateAndSimulate(
      ArmFeedforward armFF,
      double ks,
      double kv,
      double ka,
      double kg,
      double currentAngle,
      double currentVelocity,
      double nextVelocity,
      double dt) {
    final double input = armFF.calculate(currentAngle, currentVelocity, nextVelocity);
    assertEquals(
        nextVelocity,
        simulate(ks, kv, ka, kg, currentAngle, currentVelocity, input, dt).get(1, 0),
        1e-4);
  }

  @Test
  void testCalculate() {
    final double ks = 0.5;
    final double kv = 1.5;
    final double ka = 2;
    final double kg = 1;
    final ArmFeedforward armFF = new ArmFeedforward(ks, kg, kv, ka);

    // calculate(angle, angular velocity)
    assertEquals(0.5, armFF.calculate(Math.PI / 3, 0.0), 0.002);
    assertEquals(2.5, armFF.calculate(Math.PI / 3, 1.0), 0.002);

    // calculate(currentAngle, currentVelocity, nextAngle, dt)
    calculateAndSimulate(armFF, ks, kv, ka, kg, Math.PI / 3, 1.0, 1.05, 0.020);
    calculateAndSimulate(armFF, ks, kv, ka, kg, Math.PI / 3, 1.0, 0.95, 0.020);
    calculateAndSimulate(armFF, ks, kv, ka, kg, -Math.PI / 3, 1.0, 1.05, 0.020);
    calculateAndSimulate(armFF, ks, kv, ka, kg, -Math.PI / 3, 1.0, 0.95, 0.020);
  }

  @Test
  void testCalculateIllConditionedModel() {
    final double ks = 0.39671;
    final double kv = 2.7167;
    final double ka = 1e-2;
    final double kg = 0.2708;
    final ArmFeedforward armFF = new ArmFeedforward(ks, kg, kv, ka);

    final double currentAngle = 1.0;
    final double currentVelocity = 0.02;
    final double nextVelocity = 0.0;

    var averageAccel = (nextVelocity - currentVelocity) / 0.02;

    assertEquals(
        armFF.calculate(currentAngle, currentVelocity, nextVelocity),
        ks + kv * currentVelocity + ka * averageAccel + kg * Math.cos(currentAngle));
  }

  @Test
  void testCalculateIllConditionedGradient() {
    final double ks = 0.39671;
    final double kv = 2.7167;
    final double ka = 0.50799;
    final double kg = 0.2708;
    final ArmFeedforward armFF = new ArmFeedforward(ks, kg, kv, ka);

    calculateAndSimulate(armFF, ks, kv, ka, kg, 1.0, 0.02, 0.0, 0.02);
  }

  @Test
  void testAchievableVelocity() {
    final double ks = 0.5;
    final double kv = 1.5;
    final double ka = 2;
    final double kg = 1;
    final ArmFeedforward armFF = new ArmFeedforward(ks, kg, kv, ka);

    assertEquals(6, armFF.maxAchievableVelocity(12, Math.PI / 3, 1), 0.002);
    assertEquals(-9, armFF.minAchievableVelocity(11.5, Math.PI / 3, 1), 0.002);
  }

  @Test
  void testAchievableAcceleration() {
    final double ks = 0.5;
    final double kv = 1.5;
    final double ka = 2;
    final double kg = 1;
    final ArmFeedforward armFF = new ArmFeedforward(ks, kg, kv, ka);

    assertEquals(4.75, armFF.maxAchievableAcceleration(12, Math.PI / 3, 1), 0.002);
    assertEquals(6.75, armFF.maxAchievableAcceleration(12, Math.PI / 3, -1), 0.002);
    assertEquals(-7.25, armFF.minAchievableAcceleration(12, Math.PI / 3, 1), 0.002);
    assertEquals(-5.25, armFF.minAchievableAcceleration(12, Math.PI / 3, -1), 0.002);
  }

  @Test
  void testNegativeGains() {
    final double ks = 0.5;
    final double kv = 1.5;
    final double ka = 2;
    final double kg = 1;

    assertAll(
        () ->
            assertThrows(IllegalArgumentException.class, () -> new ArmFeedforward(ks, kg, -kv, ka)),
        () ->
            assertThrows(
                IllegalArgumentException.class, () -> new ArmFeedforward(ks, kg, kv, -ka)));
  }
}
