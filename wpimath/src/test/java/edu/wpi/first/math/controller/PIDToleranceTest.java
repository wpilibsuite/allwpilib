// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class PIDToleranceTest {
  private static final double SETPOINT = 50.0;
  private static final double TOLERANCE = 10.0;
  private static final double RANGE = 200;

  @Test
  void initialToleranceTest() {
    try (var controller = new PIDController(0.05, 0.0, 0.0)) {
      controller.enableContinuousInput(-RANGE / 2, RANGE / 2);

      assertFalse(controller.atSetpoint());
    }
  }

  @Test
  void absoluteToleranceTest() {
    try (var controller = new PIDController(0.05, 0.0, 0.0)) {
      controller.enableContinuousInput(-RANGE / 2, RANGE / 2);

      assertFalse(controller.atSetpoint());

      controller.setTolerance(TOLERANCE);
      controller.setSetpoint(SETPOINT);

      assertFalse(
          controller.atSetpoint(),
          "Error was in tolerance when it should not have been. Error was "
              + controller.getError());

      controller.calculate(0.0);

      assertFalse(
          controller.atSetpoint(),
          "Error was in tolerance when it should not have been. Error was "
              + controller.getError());

      controller.calculate(SETPOINT + TOLERANCE / 2);

      assertTrue(
          controller.atSetpoint(),
          "Error was not in tolerance when it should have been. Error was "
              + controller.getError());

      controller.calculate(SETPOINT + 10 * TOLERANCE);

      assertFalse(
          controller.atSetpoint(),
          "Error was in tolerance when it should not have been. Error was "
              + controller.getError());
    }
  }
}
