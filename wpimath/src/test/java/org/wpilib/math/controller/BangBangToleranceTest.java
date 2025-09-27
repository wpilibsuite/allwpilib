// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

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
}
