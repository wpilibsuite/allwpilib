// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class PIDInputOutputTest {
  @Test
  void continuousInputTest() {
    var controller = new PIDController(0.0, 0.0, 0.0);

    controller.setP(1);
    controller.enableContinuousInput(-180, 180);
    assertEquals(controller.calculate(-179, 179), -2, 1e-5);

    controller.enableContinuousInput(0, 360);
    assertEquals(controller.calculate(1, 359), -2, 1e-5);
  }

  @Test
  void proportionalGainOutputTest() {
    var controller = new PIDController(0.0, 0.0, 0.0);

    controller.setP(4);

    assertEquals(-0.1, controller.calculate(0.025, 0), 1e-5);
  }

  @Test
  void integralGainOutputTest() {
    var controller = new PIDController(0.0, 0.0, 0.0);

    controller.setI(4);

    double out = 0;

    for (int i = 0; i < 5; i++) {
      out = controller.calculate(0.025, 0);
    }

    assertEquals(-0.5 * controller.getPeriod(), out, 1e-5);
  }

  @Test
  void derivativeGainOutputTest() {
    var controller = new PIDController(0.0, 0.0, 0.0);

    controller.setD(4);

    controller.calculate(0, 0);

    assertEquals(-0.01 / controller.getPeriod(), controller.calculate(0.0025, 0), 1e-5);
  }

  @Test
  void iZoneNoOutputTest() {
    var controller = new PIDController(0.0, 0.0, 0.0);

    controller.setI(1);
    controller.setIZone(1);

    double out = controller.calculate(2, 0);

    assertEquals(0, out, 1e-5);
  }

  @Test
  void iZoneOutputTest() {
    var controller = new PIDController(0.0, 0.0, 0.0);

    controller.setI(1);
    controller.setIZone(1);

    double out = controller.calculate(1, 0);

    assertEquals(-1 * controller.getPeriod(), out, 1e-5);
  }
}
