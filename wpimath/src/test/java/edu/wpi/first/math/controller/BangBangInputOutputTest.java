// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class BangBangInputOutputTest {
  @Test
  void shouldOutput() {
    var controller = new BangBangController();

    assertEquals(controller.calculate(0, 1), 1);
  }

  @Test
  void shouldNotOutput() {
    var controller = new BangBangController();

    assertEquals(controller.calculate(1, 0), 0);
  }
}
