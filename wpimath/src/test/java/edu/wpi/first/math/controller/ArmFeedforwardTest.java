// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class ArmFeedforwardTest {
  private static final double ks = 0.5;
  private static final double kg = 1;
  private static final double kv = 1.5;
  private static final double ka = 2;
  private final ArmFeedforward m_armFF = new ArmFeedforward(ks, kg, kv, ka);

  @Test
  void testCalculate() {
    assertEquals(0.5, m_armFF.calculate(Math.PI / 3, 0), 0.002);
    assertEquals(2.5, m_armFF.calculate(Math.PI / 3, 1), 0.002);
    assertEquals(6.5, m_armFF.calculate(Math.PI / 3, 1, 2), 0.002);
    assertEquals(2.5, m_armFF.calculate(Math.PI / 3, -1, 2), 0.002);
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
