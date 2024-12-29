// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import org.junit.jupiter.api.Test;

class ElevatorFeedforwardTest {
  private static final double ks = 0.5;
  private static final double kg = 1;
  private static final double kv = 1.5;
  private static final double ka = 2;

  private final ElevatorFeedforward m_elevatorFF = new ElevatorFeedforward(ks, kg, kv, ka);

  @Test
  void testCalculate() {
    assertEquals(1, m_elevatorFF.calculate(0.0), 0.002);
    assertEquals(4.5, m_elevatorFF.calculate(2.0), 0.002);

    var A = MatBuilder.fill(Nat.N1(), Nat.N1(), -kv / ka);
    var B = MatBuilder.fill(Nat.N1(), Nat.N1(), 1.0 / ka);
    final double dt = 0.02;
    var plantInversion = new LinearPlantInversionFeedforward<N1, N1, N1>(A, B, dt);

    var r = VecBuilder.fill(2.0);
    var nextR = VecBuilder.fill(3.0);
    assertEquals(
        plantInversion.calculate(r, nextR).get(0, 0) + ks + kg,
        m_elevatorFF.calculateWithVelocities(2.0, 3.0),
        0.002);
  }

  @Test
  void testAchievableVelocity() {
    assertEquals(5, m_elevatorFF.maxAchievableVelocity(11, 1), 0.002);
    assertEquals(-9, m_elevatorFF.minAchievableVelocity(11, 1), 0.002);
  }

  @Test
  void testAchievableAcceleration() {
    assertEquals(3.75, m_elevatorFF.maxAchievableAcceleration(12, 2), 0.002);
    assertEquals(7.25, m_elevatorFF.maxAchievableAcceleration(12, -2), 0.002);
    assertEquals(-8.25, m_elevatorFF.minAchievableAcceleration(12, 2), 0.002);
    assertEquals(-4.75, m_elevatorFF.minAchievableAcceleration(12, -2), 0.002);
  }

  @Test
  void testNegativeGains() {
    assertAll(
        () ->
            assertThrows(
                IllegalArgumentException.class, () -> new ElevatorFeedforward(ks, kg, -kv, ka)),
        () ->
            assertThrows(
                IllegalArgumentException.class, () -> new ElevatorFeedforward(ks, kg, kv, -ka)));
  }
}
