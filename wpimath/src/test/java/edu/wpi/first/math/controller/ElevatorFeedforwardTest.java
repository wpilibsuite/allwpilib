// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.numbers.N1;


class ElevatorFeedforwardTest {
  private static final double ks = 0.5;
  private static final double kg = 1;
  private static final double kv = 1.5;
  private static final double ka = 2;

  private final ElevatorFeedforward m_elevatorFF = new ElevatorFeedforward(ks, kg, kv, ka);

  @Test
  void testCalculate() {
    assertEquals(1, m_elevatorFF.calculate(0), 0.002);
    assertEquals(4.5, m_elevatorFF.calculate(2), 0.002);
    assertEquals(6.5, m_elevatorFF.calculate(2, 1), 0.002);
    assertEquals(-0.5, m_elevatorFF.calculate(-2, 1), 0.002);
    var r = VecBuilder.fill(2.0);
    var nextR = VecBuilder.fill(3.0);
    var A = Matrix.mat(Nat.N1(), Nat.N1()).fill(-kv / ka);
    var B = Matrix.mat(Nat.N1(), Nat.N1()).fill(1.0 / ka);

    var plantInversion = new LinearPlantInversionFeedforward<N1, N1, N1>(A, B, 0.020);
    var elevatorMotor = new ElevatorFeedforward(ks, kg, kv, ka);

    assertEquals(
        plantInversion.calculate(r, nextR).get(0, 0) + ks + kg,
        elevatorMotor.calculate(2.0, 3.0, 0.020),
        0.002);


  }

  @Test
  void testAcheviableVelocity() {
    assertEquals(5, m_elevatorFF.maxAchievableVelocity(11, 1), 0.002);
    assertEquals(-9, m_elevatorFF.minAchievableVelocity(11, 1), 0.002);
  }

  @Test
  void testAcheviableAcceleration() {
    assertEquals(3.75, m_elevatorFF.maxAchievableAcceleration(12, 2), 0.002);
    assertEquals(7.25, m_elevatorFF.maxAchievableAcceleration(12, -2), 0.002);
    assertEquals(-8.25, m_elevatorFF.minAchievableAcceleration(12, 2), 0.002);
    assertEquals(-4.75, m_elevatorFF.minAchievableAcceleration(12, -2), 0.002);
  }
}
