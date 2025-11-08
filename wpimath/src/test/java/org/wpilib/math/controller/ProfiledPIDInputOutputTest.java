// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.trajectory.TrapezoidProfile;
import org.junit.jupiter.api.Test;

class ProfiledPIDInputOutputTest {
  @Test
  void continuousInputTest1() {
    var controller =
        new ProfiledPIDController(0.0, 0.0, 0.0, new TrapezoidProfile.Constraints(360, 180));

    controller.setP(1);
    controller.enableContinuousInput(-180, 180);

    final double kSetpoint = -179.0;
    final double kMeasurement = -179.0;
    final double kGoal = 179.0;

    controller.reset(kSetpoint);
    assertTrue(controller.calculate(kMeasurement, kGoal) < 0.0);

    // Error must be less than half the input range at all times
    assertTrue(Math.abs(controller.getSetpoint().position - kMeasurement) < 180.0);
  }

  @Test
  void continuousInputTest2() {
    var controller =
        new ProfiledPIDController(0.0, 0.0, 0.0, new TrapezoidProfile.Constraints(360, 180));

    controller.setP(1);
    controller.enableContinuousInput(-Math.PI, Math.PI);

    final double kSetpoint = -3.4826633343199735;
    final double kMeasurement = -3.1352207333939606;
    final double kGoal = -3.534162788601621;

    controller.reset(kSetpoint);
    assertTrue(controller.calculate(kMeasurement, kGoal) < 0.0);

    // Error must be less than half the input range at all times
    assertTrue(Math.abs(controller.getSetpoint().position - kMeasurement) < Math.PI);
  }

  @Test
  void continuousInputTest3() {
    var controller =
        new ProfiledPIDController(0.0, 0.0, 0.0, new TrapezoidProfile.Constraints(360, 180));

    controller.setP(1);
    controller.enableContinuousInput(-Math.PI, Math.PI);

    final double kSetpoint = -3.5176604690006377;
    final double kMeasurement = 3.1191729343822456;
    final double kGoal = 2.709680418117445;

    controller.reset(kSetpoint);
    assertTrue(controller.calculate(kMeasurement, kGoal) < 0.0);

    // Error must be less than half the input range at all times
    assertTrue(Math.abs(controller.getSetpoint().position - kMeasurement) < Math.PI);
  }

  @Test
  void continuousInputTest4() {
    var controller =
        new ProfiledPIDController(0.0, 0.0, 0.0, new TrapezoidProfile.Constraints(360, 180));

    controller.setP(1);
    controller.enableContinuousInput(0, 2.0 * Math.PI);

    final double kSetpoint = 2.78;
    final double kMeasurement = 3.12;
    final double kGoal = 2.71;

    controller.reset(kSetpoint);
    assertTrue(controller.calculate(kMeasurement, kGoal) < 0.0);

    // Error must be less than half the input range at all times
    assertTrue(Math.abs(controller.getSetpoint().position - kMeasurement) < Math.PI / 2.0);
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
}
