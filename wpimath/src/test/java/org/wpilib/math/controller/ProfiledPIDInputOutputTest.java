// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;
import org.wpilib.math.trajectory.TrapezoidProfile;
import org.wpilib.tunable.MockTunableBackend;
import org.wpilib.tunable.TunableConfig;
import org.wpilib.tunable.TunableRegistry;
import org.wpilib.tunable.Tunables;

class ProfiledPIDInputOutputTest {
  @Test
  void continuousInputTest1() {
    var controller =
        new ProfiledPIDController(0.0, 0.0, 0.0, new TrapezoidProfile.Constraints(360, 180));

    controller.setP(1);
    controller.enableContinuousInput(-180, 180);

    final double SETPOINT = -179.0;
    final double MEASUREMENT = -179.0;
    final double GOAL = 179.0;

    controller.reset(SETPOINT);
    assertTrue(controller.calculate(MEASUREMENT, GOAL) < 0.0);

    // Error must be less than half the input range at all times
    assertTrue(Math.abs(controller.getSetpoint().position - MEASUREMENT) < 180.0);
  }

  @Test
  void continuousInputTest2() {
    var controller =
        new ProfiledPIDController(0.0, 0.0, 0.0, new TrapezoidProfile.Constraints(360, 180));

    controller.setP(1);
    controller.enableContinuousInput(-Math.PI, Math.PI);

    final double SETPOINT = -3.4826633343199735;
    final double MEASUREMENT = -3.1352207333939606;
    final double GOAL = -3.534162788601621;

    controller.reset(SETPOINT);
    assertTrue(controller.calculate(MEASUREMENT, GOAL) < 0.0);

    // Error must be less than half the input range at all times
    assertTrue(Math.abs(controller.getSetpoint().position - MEASUREMENT) < Math.PI);
  }

  @Test
  void continuousInputTest3() {
    var controller =
        new ProfiledPIDController(0.0, 0.0, 0.0, new TrapezoidProfile.Constraints(360, 180));

    controller.setP(1);
    controller.enableContinuousInput(-Math.PI, Math.PI);

    final double SETPOINT = -3.5176604690006377;
    final double MEASUREMENT = 3.1191729343822456;
    final double GOAL = 2.709680418117445;

    controller.reset(SETPOINT);
    assertTrue(controller.calculate(MEASUREMENT, GOAL) < 0.0);

    // Error must be less than half the input range at all times
    assertTrue(Math.abs(controller.getSetpoint().position - MEASUREMENT) < Math.PI);
  }

  @Test
  void continuousInputTest4() {
    var controller =
        new ProfiledPIDController(0.0, 0.0, 0.0, new TrapezoidProfile.Constraints(360, 180));

    controller.setP(1);
    controller.enableContinuousInput(0, 2.0 * Math.PI);

    final double SETPOINT = 2.78;
    final double MEASUREMENT = 3.12;
    final double GOAL = 2.71;

    controller.reset(SETPOINT);
    assertTrue(controller.calculate(MEASUREMENT, GOAL) < 0.0);

    // Error must be less than half the input range at all times
    assertTrue(Math.abs(controller.getSetpoint().position - MEASUREMENT) < Math.PI / 2.0);
  }

  @Test
  void proportionalGainOutputTest() {
    var controller =
        new ProfiledPIDController(0.0, 0.0, 0.0, new TrapezoidProfile.Constraints(360, 180));

    controller.setP(4);

    assertEquals(-0.1, controller.calculate(0.025, 0), 1e-5);
  }

  @Test
  void integralGainOutputTest() {
    var controller =
        new ProfiledPIDController(0.0, 0.0, 0.0, new TrapezoidProfile.Constraints(360, 180));

    controller.setI(4);

    double out = 0;

    for (int i = 0; i < 5; i++) {
      out = controller.calculate(0.025, 0);
    }

    assertEquals(-0.5 * controller.getPeriod(), out, 1e-5);
  }

  @Test
  void derivativeGainOutputTest() {
    var controller =
        new ProfiledPIDController(0.0, 0.0, 0.0, new TrapezoidProfile.Constraints(360, 180));

    controller.setD(4);

    controller.calculate(0, 0);

    assertEquals(-0.01 / controller.getPeriod(), controller.calculate(0.0025, 0), 1e-5);
  }

  @Test
  void tunedConstraintsRebuildProfile() {
    var backend = new MockTunableBackend();
    TunableRegistry.reset();
    TunableRegistry.registerBackend("", backend);

    try {
      var controller =
          new ProfiledPIDController(0.0, 0.0, 0.0, new TrapezoidProfile.Constraints(1.0, 1.0));
      Tunables.publish("profiled", controller);

      var constraintsTunable = backend.getTunable("/profiled/constraints");
      assertEquals(
          TunableConfig.Polling.GET_ON_CHANGE, constraintsTunable.getConfig().getPolling());
      assertFalse(constraintsTunable.hasChanged());

      controller.setConstraints(new TrapezoidProfile.Constraints(2.0, 2.0));
      assertTrue(constraintsTunable.hasChanged());

      backend.setValue("/profiled/constraints", new TrapezoidProfile.Constraints(10.0, 10.0));
      TunableRegistry.update();

      assertEquals(10.0, controller.getConstraints().maxVelocity);
      assertEquals(10.0, controller.getConstraints().maxAcceleration);

      controller.reset(0.0);
      controller.calculate(0.0, 10.0);
      assertEquals(0.2, controller.getSetpoint().velocity, 1e-9);
    } finally {
      TunableRegistry.reset();
    }
  }

  @Test
  void tunedGoalUpdatesGoal() {
    var backend = new MockTunableBackend();
    TunableRegistry.reset();
    TunableRegistry.registerBackend("", backend);

    try {
      var controller =
          new ProfiledPIDController(0.0, 0.0, 0.0, new TrapezoidProfile.Constraints(1.0, 1.0));
      Tunables.publish("profiled", controller);

      var goalTunable = backend.getTunable("/profiled/goal");
      assertEquals(TunableConfig.Polling.GET_ON_CHANGE, goalTunable.getConfig().getPolling());
      assertFalse(goalTunable.hasChanged());

      backend.setDouble("/profiled/goal", 2.0);
      TunableRegistry.update();

      assertEquals(2.0, controller.getGoal().position);
      assertEquals(0.0, controller.getGoal().velocity);

      controller.setGoal(3.0);
      assertTrue(goalTunable.hasChanged());
      TunableRegistry.update();

      assertEquals(3.0, backend.getDouble("/profiled/goal"));
    } finally {
      TunableRegistry.reset();
    }
  }
}
