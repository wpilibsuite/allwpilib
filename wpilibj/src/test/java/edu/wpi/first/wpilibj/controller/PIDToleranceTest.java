/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class PIDToleranceTest {
  private static final double kSetpoint = 50.0;
  private static final double kTolerance = 10.0;
  private static final double kRange = 200;

  @Test
  void initialToleranceTest() {
    try (var controller = new PIDController(0.05, 0.0, 0.0)) {
      controller.enableContinuousInput(-kRange / 2, kRange / 2);

      assertTrue(controller.atSetpoint());
    }
  }

  @Test
  void absoluteToleranceTest() {
    try (var controller = new PIDController(0.05, 0.0, 0.0)) {
      controller.enableContinuousInput(-kRange / 2, kRange / 2);

      assertTrue(controller.atSetpoint(),
          "Error was not in tolerance when it should have been. Error was "
          + controller.getPositionError());

      controller.setTolerance(kTolerance);
      controller.setSetpoint(kSetpoint);

      assertFalse(controller.atSetpoint(),
          "Error was in tolerance when it should not have been. Error was "
          + controller.getPositionError());

      controller.calculate(0.0);

      assertFalse(controller.atSetpoint(),
          "Error was in tolerance when it should not have been. Error was "
          + controller.getPositionError());

      controller.calculate(kSetpoint + kTolerance / 2);

      assertTrue(controller.atSetpoint(),
          "Error was not in tolerance when it should have been. Error was "
          + controller.getPositionError());

      controller.calculate(kSetpoint + 10 * kTolerance);

      assertFalse(controller.atSetpoint(),
          "Error was in tolerance when it should not have been. Error was "
          + controller.getPositionError());
    }
  }
}
