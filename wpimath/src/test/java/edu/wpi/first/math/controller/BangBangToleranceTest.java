// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class BangBangToleranceTest {
  private BangBangController m_controller;

  @BeforeEach
  void setUp() {
    m_controller = new BangBangController(0.1);
  }

  @Test
  void inTolerance() {
    m_controller.setSetpoint(1);
    m_controller.calculate(1);
    assertTrue(m_controller.atSetpoint());
  }

  @Test
  void outOfTolerance() {
    m_controller.setSetpoint(1);
    m_controller.calculate(0);
    assertFalse(m_controller.atSetpoint());
  }
}
