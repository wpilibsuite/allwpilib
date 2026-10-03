// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.counter;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;
import org.wpilib.hardware.hal.HAL;

class TachometerTest {
  @Test
  void testSetRateWindow() {
    HAL.initialize();

    try (Tachometer tachometer = new Tachometer(0, EdgeConfiguration.RISING_EDGE)) {
      tachometer.setRateWindow(50);
    }
  }

  @Test
  void testStoppedStateMatchesRate() {
    HAL.initialize();

    try (Tachometer tachometer = new Tachometer(0, EdgeConfiguration.RISING_EDGE)) {
      assertEquals(0.0, tachometer.getFrequency());
      assertTrue(tachometer.getStopped());
    }
  }
}
