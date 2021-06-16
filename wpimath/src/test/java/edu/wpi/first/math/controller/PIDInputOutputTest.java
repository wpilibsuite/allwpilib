// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class PIDInputOutputTest {
  private PIDController m_controller;

  @BeforeEach
  void setUp() {
    m_controller = new PIDController(0, 0, 0);
  }

  @Test
  void continuousInputTest() {
    m_controller.setP(1);
    m_controller.enableContinuousInput(-180, 180);
    assertEquals(m_controller.calculate(-179, 179), -2, 1e-5);

    m_controller.enableContinuousInput(0, 360);
    assertEquals(m_controller.calculate(1, 359), -2, 1e-5);
  }

  @Test
  void proportionalGainOutputTest() {
    m_controller.setP(4);

    assertEquals(-0.1, m_controller.calculate(0.025, 0), 1e-5);
  }

  @Test
  void integralGainOutputTest() {
    m_controller.setI(4);

    double out = 0;

    for (int i = 0; i < 5; i++) {
      out = m_controller.calculate(0.025, 0);
    }

    assertEquals(-0.5 * m_controller.getPeriod(), out, 1e-5);
  }

  @Test
  void derivativeGainOutputTest() {
    m_controller.setD(4);

    m_controller.calculate(0, 0);

    assertEquals(-0.01 / m_controller.getPeriod(), m_controller.calculate(0.0025, 0), 1e-5);
  }
}
