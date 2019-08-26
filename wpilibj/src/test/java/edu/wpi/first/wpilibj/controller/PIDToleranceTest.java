/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class PIDToleranceTest {
  private PIDController m_pidController;
  private static final double kSetpoint = 50.0;
  private static final double kTolerance = 10.0;
  private static final double kRange = 200;

  @BeforeEach
  void setUp() {
    m_pidController = new PIDController(0.05, 0.0, 0.0);
    m_pidController.setInputRange(-kRange / 2, kRange / 2);
  }

  @AfterEach
  void tearDown() {
    m_pidController.close();
    m_pidController = null;
  }

  @Test
  void initialToleranceTest() {
    assertTrue(m_pidController.atSetpoint());
  }

  @Test
  void absoluteToleranceTest() {
    m_pidController.setAbsoluteTolerance(kTolerance);
    m_pidController.setSetpoint(kSetpoint);

    m_pidController.calculate(0.0);

    assertFalse(m_pidController.atSetpoint(),
        "Error was in tolerance when it should not have been. Error was " + m_pidController
            .getPositionError());

    m_pidController.calculate(kSetpoint + kTolerance / 2);

    assertTrue(m_pidController.atSetpoint(),
        "Error was not in tolerance when it should have been. Error was " + m_pidController
            .getPositionError());

    m_pidController.calculate(kSetpoint + 10 * kTolerance);

    assertFalse(m_pidController.atSetpoint(),
        "Error was in tolerance when it should not have been. Error was " + m_pidController
            .getPositionError());
  }

  @Test
  void percentToleranceTest() {
    m_pidController.setPercentTolerance(kTolerance);
    m_pidController.setSetpoint(kSetpoint);

    m_pidController.calculate(0.0);

    assertFalse(m_pidController.atSetpoint(),
        "Error was in tolerance when it should not have been. Error was " + m_pidController
            .getPositionError());

    // Half of percent tolerance away from setpoint
    m_pidController.calculate(kSetpoint + (kTolerance / 2) / 100 * kRange);

    assertTrue(m_pidController.atSetpoint(),
        "Error was not in tolerance when it should have been. Error was " + m_pidController
            .getPositionError());

    // Double percent tolerance away from setpoint
    m_pidController.calculate(kSetpoint + (kTolerance * 2) / 100 * kRange);

    assertFalse(m_pidController.atSetpoint(),
        "Error was in tolerance when it should not have been. Error was " + m_pidController
            .getPositionError());
  }
}
