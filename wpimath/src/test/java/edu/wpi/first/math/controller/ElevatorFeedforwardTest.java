// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

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
