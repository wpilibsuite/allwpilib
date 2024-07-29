// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.trajectory.TrapezoidProfile;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class ProfiledPIDInputOutputTest {
  private ProfiledPIDController m_controller;

  @BeforeEach
  void setUp() {
    m_controller = new ProfiledPIDController(0, 0, 0, new TrapezoidProfile.Constraints(360, 180));
  }

  @Test
  void continuousInputTest1() {
    m_controller.setP(1);
    m_controller.enableContinuousInput(-180, 180);

    final double kSetpoint = -179.0;
    final double kMeasurement = -179.0;
    final double kGoal = 179.0;

    m_controller.reset(kSetpoint);
    assertTrue(m_controller.calculate(kMeasurement, kGoal) < 0.0);

    // Error must be less than half the input range at all times
    assertTrue(Math.abs(m_controller.getSetpoint().position - kMeasurement) < 180.0);
  }

  @Test
  void continuousInputTest2() {
    m_controller.setP(1);
    m_controller.enableContinuousInput(-Math.PI, Math.PI);

    final double kSetpoint = -3.4826633343199735;
    final double kMeasurement = -3.1352207333939606;
    final double kGoal = -3.534162788601621;

    m_controller.reset(kSetpoint);
    assertTrue(m_controller.calculate(kMeasurement, kGoal) < 0.0);

    // Error must be less than half the input range at all times
    assertTrue(Math.abs(m_controller.getSetpoint().position - kMeasurement) < Math.PI);
  }

  @Test
  void continuousInputTest3() {
    m_controller.setP(1);
    m_controller.enableContinuousInput(-Math.PI, Math.PI);

    final double kSetpoint = -3.5176604690006377;
    final double kMeasurement = 3.1191729343822456;
    final double kGoal = 2.709680418117445;

    m_controller.reset(kSetpoint);
    assertTrue(m_controller.calculate(kMeasurement, kGoal) < 0.0);

    // Error must be less than half the input range at all times
    assertTrue(Math.abs(m_controller.getSetpoint().position - kMeasurement) < Math.PI);
  }

  @Test
  void continuousInputTest4() {
    m_controller.setP(1);
    m_controller.enableContinuousInput(0, 2.0 * Math.PI);

    final double kSetpoint = 2.78;
    final double kMeasurement = 3.12;
    final double kGoal = 2.71;

    m_controller.reset(kSetpoint);
    assertTrue(m_controller.calculate(kMeasurement, kGoal) < 0.0);

    // Error must be less than half the input range at all times
    assertTrue(Math.abs(m_controller.getSetpoint().position - kMeasurement) < Math.PI / 2.0);
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
