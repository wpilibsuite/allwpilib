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

class BangBangToleranceTest {
  @Test
  void inTolerance() {
    var controller = new BangBangController(0.1);

    controller.setSetpoint(1);
    controller.calculate(1);
    assertTrue(controller.atSetpoint());
  }

  @Test
  void outOfTolerance() {
    var controller = new BangBangController(0.1);

    controller.setSetpoint(1);
    controller.calculate(0);
    assertFalse(controller.atSetpoint());
  }

  @Test
  void tunedValuesPublishOnChange() {
    var backend = new MockTunableBackend();
    TunableRegistry.reset();
    TunableRegistry.registerBackend("", backend);

    try {
      var controller = new BangBangController(0.1);
      Tunables.publish("bang", controller);

      var toleranceTunable = backend.getTunable("/bang/tolerance");
      assertEquals(TunableConfig.Polling.GET_ON_CHANGE, toleranceTunable.getConfig().getPolling());
      assertFalse(toleranceTunable.hasChanged());

      controller.setTolerance(0.2);
      assertTrue(toleranceTunable.hasChanged());
      TunableRegistry.update();
      assertEquals(0.2, backend.getDouble("/bang/tolerance"));

      var setpointTunable = backend.getTunable("/bang/setpoint");
      assertEquals(TunableConfig.Polling.GET_ON_CHANGE, setpointTunable.getConfig().getPolling());
      assertFalse(setpointTunable.hasChanged());

      controller.calculate(0.0, 1.0);
      assertTrue(setpointTunable.hasChanged());
      TunableRegistry.update();
      assertEquals(1.0, backend.getDouble("/bang/setpoint"));

      backend.setDouble("/bang/tolerance", 0.3);
      backend.setDouble("/bang/setpoint", 2.0);
      TunableRegistry.update();

      assertEquals(0.3, controller.getTolerance());
      assertEquals(2.0, controller.getSetpoint());
    } finally {
      TunableRegistry.reset();
    }
  }
}
