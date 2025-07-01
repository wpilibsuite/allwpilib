// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;
import org.wpilib.tunable.MockTunableBackend;
import org.wpilib.tunable.TunableConfig;
import org.wpilib.tunable.TunableRegistry;
import org.wpilib.tunable.Tunables;

class PIDToleranceTest {
  private static final double kSetpoint = 50.0;
  private static final double kTolerance = 10.0;
  private static final double kRange = 200;

  @Test
  void initialToleranceTest() {
    var controller = new PIDController(0.05, 0.0, 0.0);
    controller.enableContinuousInput(-kRange / 2, kRange / 2);

    assertFalse(controller.atSetpoint());
  }

  @Test
  void tunedSetpointUpdatesSetpointState() {
    var backend = new MockTunableBackend();
    TunableRegistry.reset();
    TunableRegistry.registerBackend("", backend);

    try {
      var controller = new PIDController(0.5, 0.0, 0.0);
      Tunables.publish("pid", controller);

      assertFalse(controller.atSetpoint());

      var pTunable = backend.getTunable("/pid/p");
      assertEquals(TunableConfig.Polling.GET_ON_CHANGE, pTunable.getConfig().getPolling());
      assertFalse(pTunable.hasChanged());

      controller.setP(0.6);
      assertTrue(pTunable.hasChanged());

      var setpointTunable = backend.getTunable("/pid/setpoint");
      assertEquals(TunableConfig.Polling.GET_ON_CHANGE, setpointTunable.getConfig().getPolling());

      controller.setSetpoint(1.0);
      assertTrue(setpointTunable.hasChanged());
      TunableRegistry.update();

      backend.setDouble("/pid/setpoint", kSetpoint);
      TunableRegistry.update();

      assertEquals(kSetpoint, controller.getSetpoint());
      assertEquals(kSetpoint, controller.getError());

      controller.calculate(kSetpoint);

      assertTrue(controller.atSetpoint());
    } finally {
      TunableRegistry.reset();
    }
  }

  @Test
  void absoluteToleranceTest() {
    var controller = new PIDController(0.05, 0.0, 0.0);
    controller.enableContinuousInput(-kRange / 2, kRange / 2);

    assertFalse(controller.atSetpoint());

    controller.setTolerance(kTolerance);
    controller.setSetpoint(kSetpoint);

    assertFalse(
        controller.atSetpoint(),
        "Error was in tolerance when it should not have been. Error was " + controller.getError());

    controller.calculate(0.0);

    assertFalse(
        controller.atSetpoint(),
        "Error was in tolerance when it should not have been. Error was " + controller.getError());

    controller.calculate(kSetpoint + kTolerance / 2);

    assertTrue(
        controller.atSetpoint(),
        "Error was not in tolerance when it should have been. Error was " + controller.getError());

    controller.calculate(kSetpoint + 10 * kTolerance);

    assertFalse(
        controller.atSetpoint(),
        "Error was in tolerance when it should not have been. Error was " + controller.getError());
  }
}
